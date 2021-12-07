#include <stdio.h>
#include <stdbool.h>
#include <sndfile.h>
#include <math.h>
#include <fftw3.h>
#include "filter.h"

void main()
{
	char *inFileName;
	SNDFILE *inFile;
	SF_INFO inFileInfo;
	int fs;
	
	inFileName = "idk.wav";
	inFile = sf_open(inFileName, SFM_READ, &inFileInfo);
	int samp_count = inFileInfo.frames;
	int samp_rate = inFileInfo.samplerate;
	float *samples = calloc(samp_count, sizeof(float));
	sf_readf_float(inFile, samples, samp_count); //??? fix this pointer thing pls why it produce warning error idk
	sf_close(inFile);
	printf("Be careful! This code will break with WAV files at 48000 Hz that are longer than about 12h\n");
	printf("Sample Rate = %d Hz\n", samp_rate);
	printf("Sample Count = %d\n", samp_count);
	sf_close(inFile);
	if(samp_rate != 48000)
	{
		printf("Sample rate is not 48000 Hz, this code will not work\n");
		return;
	}

	  

	//Convert real array into complex, set Imaginary to zero
	printf("Converting to complex\n");
	float *inputComplex = calloc(samp_count * 2, sizeof(float));
	for (unsigned int i = 0; i < samp_count; i++)
	{
		inputComplex[(2*i)] = 0; //imaginary
		inputComplex[(2*i)+1] = samples[i]; //real
	}

	//Generate sine and cosine for complex mixing
	printf("Generating sine and cosine 1\n");
	float pi = 3.14159265358979323846;
	float frequency = 8200;
	float *ComplexSine = calloc(samp_count * 2, sizeof(float));
	for(int i = 0; i < samp_count; i++)
	{
		ComplexSine[(2*i)] = cos(frequency * (2 * pi) * i / samp_rate); // imaginary
		ComplexSine[(2*i)+1] = sin(frequency * (2 * pi) * i / samp_rate); //real
	}

	//Mix complex signals together
	printf("Mixing signals\n");
	float *outputComplex = calloc(samp_count * 2, sizeof(float));
	for(int i = 0; i < samp_count; i++)
	{
		outputComplex[(2*i)] = inputComplex[(2*i)] * ComplexSine[(2*i)] - inputComplex[(2*i)+1] * ComplexSine[(2*i)+1]; //imaginary
		outputComplex[(2*i)+1] = inputComplex[(2*i)] * ComplexSine[(2*i)+1] + inputComplex[(2*i)+1] * ComplexSine[(2*i)]; //real
	}

	
	//lowpass struggle
	//Apply filters to I and Q
	printf("Applying filters\n");
	BWLowPass* filterR = create_bw_low_pass_filter(50, samp_rate, 100);
	BWLowPass* filterI = create_bw_low_pass_filter(50, samp_rate, 100);
	for(int i = 0; i < samp_count; i++)
	{
		outputComplex[(2*i)] = bw_low_pass(filterI, outputComplex[(2*i)]);
		outputComplex[(2*i) + 1] = bw_low_pass(filterR, outputComplex[(2*i) + 1]);
	}
	free_bw_low_pass(filterR);
	free_bw_low_pass(filterI);



	//Generate Another sine
	printf("Generating sine and cosine 2\n");
	frequency = 100;
	float *ComplexSine2 = calloc(samp_count * 2, sizeof(float));
	for(int i = 0; i < samp_count; i++)
	{
		ComplexSine2[(2*i)+1] = cos(frequency * (2 * pi) * i / samp_rate); // real
		ComplexSine2[(2*i)] = sin(frequency * (2 * pi) * i / samp_rate); //Imaginary
	}

	//Mix again
	printf("Mixing signals\n");
	float *outputComplex2 = calloc(samp_count * 2, sizeof(float));
	for(int i = 0; i < samp_count; i++)
	{
		outputComplex2[(2*i)] = outputComplex[(2*i)] * ComplexSine2[(2*i)] - outputComplex[(2*i)+1] * ComplexSine2[(2*i)+1]; //imaginary
		outputComplex2[(2*i)+1] = outputComplex[(2*i)] * ComplexSine2[(2*i)+1] + outputComplex[(2*i)+1] * ComplexSine2[(2*i)]; //real
	}




	//Convert complex array back to real, will flip over imaginary!
	printf("Converting back to real\n");
	float *outputReal = calloc(samp_count, sizeof(float));
	for(int i = 0; i < samp_count; i++)
	{
		outputReal[i] = outputComplex2[(2*i)+1]; // just take the real part
	}

	//crappy lowpass <-- it should kind of be good now because it's a library, and libs work right? right?
	
	//Apply filters to output signal
	printf("Resampling to %d samples/sec\n", samp_rate / 100);
	float *outputReal2 = calloc(samp_count / 100, sizeof(float));
	int samplecounter = 0;
	for(int i = 0; i < samp_count; i++)
	{
		samplecounter++;
		if(samplecounter == 100)
		{
			outputReal2[i/100] = outputReal[i];
			samplecounter = 0;
		}
	}
	

	
	printf("Writing to file\n");
    char *outFileName = "out.wav";
	SNDFILE *outFile;
	SF_INFO outFileInfo = inFileInfo;
	outFileInfo.samplerate = samp_rate / 100;
	outFile = sf_open(outFileName, SFM_WRITE, &outFileInfo);
	sf_writef_float(outFile, outputReal2, samp_count / 100);
	sf_close(outFile);
	
}
