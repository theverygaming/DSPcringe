#include <stdio.h>
#include <stdbool.h>
#include <sndfile.h>
#include <math.h>

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
	float samples[samp_count];
	sf_readf_float(inFile, &samples, samp_count); //??? fix this pointer thing pls why it produce warning error idk
	sf_close(inFile);
	printf("Sample Rate = %d Hz\n", samp_rate);
	printf("Sample Count = %d Hz\n", samp_count);
	sf_close(inFile);
	
	//Convert real array into complex, set Imaginary to zero
	float inputComplex[samp_count * 2];
	for (int i = 0; i < samp_count; i++)
	{
		inputComplex[(2*i)] = 0; //imaginary
		inputComplex[(2*i)+1] = samples[i]; //real
	}

	//Generate sine and cosine for complex mixing
	float pi = 3.14159265358979323846;
	float frequency = 8200;
	float ComplexSine[samp_count * 2];
	for(int i = 0; i < samp_count; i++)
	{
		ComplexSine[(2*i)] = cos(frequency * (2 * pi) * i / samp_rate); // imaginary
		ComplexSine[(2*i)+1] = sin(frequency * (2 * pi) * i / samp_rate); //real
	}

	//Mix complex signals together
	float outputComplex[samp_count * 2];
	for(int i = 0; i < samp_count; i++)
	{
		outputComplex[(2*i)] = inputComplex[(2*i)] * ComplexSine[(2*i)] - inputComplex[(2*i)+1] * ComplexSine[(2*i)+1]; //imaginary
		outputComplex[(2*i)+1] = inputComplex[(2*i)] * ComplexSine[(2*i)+1] + inputComplex[(2*i)+1] * ComplexSine[(2*i)]; //real
	}

	//Convert complex array back to real, will flip over imaginary!
	float outputReal[samp_count];
	for(int i = 0; i < samp_count; i++)
	{
		outputReal[i] = outputComplex[(2*i)+1]; // just take the real part
	}


    char *outFileName = "out.wav";
	SNDFILE *outFile;
	SF_INFO outFileInfo = inFileInfo;
	outFileInfo.samplerate = 48000;
	outFile = sf_open(outFileName, SFM_WRITE, &outFileInfo);
	sf_writef_float(outFile, &outputReal, samp_count);
	sf_close(outFile);
}



