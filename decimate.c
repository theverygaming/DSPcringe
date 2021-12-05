#include <stdio.h>
#include <stdbool.h>
#include <sndfile.h>

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
	//int *samples2 = samples;
	sf_readf_float(inFile, &samples, samp_count); //??? fix this pointer thing pls
	sf_close(inFile);
	printf("Sample Rate = %d Hz\n", samp_rate);
	printf("Sample Count = %d Hz\n", samp_count);

	float samplesDec[samp_count / 2];
	bool idk = true;
	int decCount = 0;
	for (int i = 0; i < samp_count; i++)
	{
		//printf("%f \n", samples[i]);
		if(idk)
		{
			samplesDec[decCount] = samples[i];
			decCount++;
			idk = false;
		}
		else
		{
			idk = true;
		}
	}
	sf_close(inFile);
	
    char *outFileName = "out.wav";
	SNDFILE *outFile;
	SF_INFO outFileInfo = inFileInfo;
	outFileInfo.samplerate = 24000;
	outFile = sf_open(outFileName, SFM_WRITE, &outFileInfo);
	sf_writef_float(outFile, &samplesDec, samp_count / 2);
	sf_close(outFile);
}