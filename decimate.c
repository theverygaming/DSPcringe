#include <stdio.h>
#include <stdbool.h>
#include <sndfile.h>
#include <math.h>
#include <fftw3.h>
#include <complex.h>
#include "filter.h"
#include <volk/volk.h>

void main()
{
	char *inFileName;
	SNDFILE *inFile;
	SF_INFO inFileInfo;
	int fs;

	inFileName = "in.wav";
	inFile = sf_open(inFileName, SFM_READ, &inFileInfo);
	long int samp_count = inFileInfo.frames;
	int samp_rate = inFileInfo.samplerate;
	float *samples = calloc(samp_count, sizeof(float));
	sf_readf_float(inFile, samples, samp_count); //??? fix this pointer thing pls why it produce warning error idk
	sf_close(inFile);
	printf("Be careful! This code will break with WAV files at 48000 Hz that are longer than about 12h. Actually it might be 6h but idk\n");
	printf("Sample Rate = %d Hz\n", samp_rate);
	printf("Sample Count = %d\n", samp_count);
	sf_close(inFile);
	

	const int MixCenterFrequency = 8274; //user
	const int MixBandwidth = 10; //user
	int FilterAndMixFrequency = MixBandwidth / 2;
	//int samplerateDivider = 100;
	int samplerateDivider = samp_rate / (MixBandwidth * 2);
	const int FFTsize = 4096; //user
	printf("FFT Resolution will be %f Hz\n", ((float)samp_rate / samplerateDivider) / FFTsize);
	  

	//Convert real array into complex, set Imaginary to zero
	printf("Converting to complex\n");
	complex float *inputComplex = (complex float*) calloc(samp_count, sizeof(complex float));
	for (unsigned int i = 0; i < samp_count; i++)
	{
		inputComplex[i] = samples[i]+0*I;
	}
	free(samples); 

	
	
    printf("Mixing signals\n");
    unsigned int alignment = volk_get_alignment();
    lv_32fc_t* outputComplex = (lv_32fc_t*)volk_malloc(sizeof(lv_32fc_t)*samp_count, alignment);
    float sinAngle = 2.0 * 3.14159265359 * MixCenterFrequency / samp_rate;
    lv_32fc_t phase_increment = lv_cmake(cos(sinAngle), sin(sinAngle));
    lv_32fc_t phase= lv_cmake(1.f, 0.0f);
    volk_32fc_s32fc_x2_rotator_32fc(outputComplex, inputComplex, phase_increment, &phase, samp_count);
	free(inputComplex); 
	


	//Confusion, Invert I and Q
	printf("Inverting I and Q\n");
	for (unsigned int i = 0; i < samp_count; i++)
	{
		outputComplex[i] = conjf(outputComplex[i]);
	}
	

	
	

	
	//lowpass struggle
	//Apply filters to I and Q
	printf("Applying filters\n");
	BWLowPass* filterR = create_bw_low_pass_filter(50, samp_rate, FilterAndMixFrequency);
	BWLowPass* filterI = create_bw_low_pass_filter(50, samp_rate, FilterAndMixFrequency);
	for(int i = 0; i < samp_count; i++)
	{
		float imag = bw_low_pass(filterI, cimag(outputComplex[i]));
		float real = bw_low_pass(filterR, creal(outputComplex[i]));
		outputComplex[i] = real+imag*I;
		
	}
	free_bw_low_pass(filterR);
	free_bw_low_pass(filterI);



	
    printf("Mixing signals\n");
    unsigned int alignment2 = volk_get_alignment();
    lv_32fc_t* outputComplex2 = (lv_32fc_t*)volk_malloc(sizeof(lv_32fc_t)*samp_count, alignment2);
    float sinAngle2 = 2.0 * 3.14159265359 * FilterAndMixFrequency / samp_rate;
    lv_32fc_t phase_increment2 = lv_cmake(cos(sinAngle2), sin(sinAngle2));
    lv_32fc_t phase2= lv_cmake(1.f, 0.0f);
    volk_32fc_s32fc_x2_rotator_32fc(outputComplex2, outputComplex, phase_increment2, &phase2, samp_count);
	volk_free(outputComplex);





	//Convert complex array back to real, will flip over imaginary!
	printf("Converting back to real\n");
	float *outputReal = calloc(samp_count, sizeof(float));
	for(int i = 0; i < samp_count; i++)
	{
		outputReal[i] = creal(outputComplex2[i]); // just take the real part
	}
	volk_free(outputComplex2);


	
	//Resample
	int newSampleCount = samp_count / samplerateDivider;
	int newSampleRate = samp_rate / samplerateDivider;
	printf("Resampling to %d samples/sec\n", newSampleRate);
	float *outputReal2 = calloc(newSampleCount, sizeof(float));
	int samplecounter = 0;
	for(int i = 0; i < samp_count; i++)
	{
		samplecounter++;
		if(samplecounter == samplerateDivider)
		{
			outputReal2[i/samplerateDivider] = outputReal[i];
			samplecounter = 0;
		}
	}
	free(outputReal);
	

	//Write the result to a file
	printf("Writing to file\n");
    char *outFileName = "out.wav";
	SNDFILE *outFile;
	SF_INFO outFileInfo = inFileInfo;
	outFileInfo.samplerate = newSampleRate;
	outFile = sf_open(outFileName, SFM_WRITE, &outFileInfo);
	sf_writef_float(outFile, outputReal2, newSampleCount);
	sf_close(outFile);




	int FFTsampleCount = floor((float) newSampleCount / (float) FFTsize) * FFTsize;
	FILE *fftoutfile = fopen("FFTout.txt", "w"); //One line for each FFT comma sepearted
	if(fftoutfile == NULL)
	{
		printf("Error opening file!\n");
		return;
	}
	fprintf(fftoutfile, "samplerate:%d fftsize:%d centerfrequency:%d newcenterfrequency:%d filterfrequencyfromcenter:%d\n", newSampleRate, FFTsize, MixCenterFrequency, FilterAndMixFrequency, FilterAndMixFrequency); //Still some hardcoded values
	for(int i = 0; i < FFTsampleCount;)
	{
		float *FFTin = calloc(FFTsize, sizeof(float));
		for(int j = 0; j < FFTsize; j++)
		{
			FFTin[j] = outputReal2[i+j];
		}
		float *FFTout = calloc((FFTsize / 2) + 1, sizeof(float));
		processFFT(FFTsize, FFTin, FFTout);
		for(int e = 0; e < ((FFTsize / 2) + 1) - 1; e++) //-1 because the last one will be written to file with line break
		{
			fprintf(fftoutfile, "%0.6f,", FFTout[e]);
		}
		if(i + FFTsize < FFTsampleCount)
		{
			fprintf(fftoutfile, "%0.6f\n", FFTout[(FFTsize / 2) + 1]);
		}
		else
		{
			fprintf(fftoutfile, "%0.6f", FFTout[(FFTsize / 2) + 1]);
		}
		i += FFTsize;
	}
	fclose(fftoutfile);
	
}



void processFFT(int n, float *input, float *output) //input size must be equal to n, output has to be n/2 + 1
{
	//Resize array for FFT
	double *FFTarray = calloc(n, sizeof(double));
	fftw_complex *outFFTarray = (fftw_complex*) fftw_malloc(sizeof(fftw_complex) * (n / 2 + 1));
	fftw_plan p = fftw_plan_dft_r2c_1d(n, FFTarray, outFFTarray, FFTW_MEASURE);
	for(int i = 0; i < n; i++)
	{
		double multiplier = 0.5 * (1 - cos(2*3.14159265*i/(n - 1))); // hann window thanks to https://stackoverflow.com/a/3555393 lmfao
		FFTarray[i] = multiplier * (double) input[i];
	}
	fftw_execute(p);
	fftw_destroy_plan(p);
	for(int i = 0; i < (n/2 + 1); i++)
	{
		double magnitude = sqrt(outFFTarray[i][0] * outFFTarray[i][0] + outFFTarray[i][1] * outFFTarray[i][1]);
		output[i] = (float) magnitude;
		//printf("%f\n", magnitude);
	}
	//free memory
	free(FFTarray);
	fftw_free(outFFTarray);
	

}



