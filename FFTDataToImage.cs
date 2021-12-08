using System;
using System.IO;
using System.Drawing;

public class FFTDataToImage
{
    static void Main(string[] args)
    {
        Console.Write("Input file?");
        //string filepath = Console.ReadLine();
        string[] allLines = File.ReadAllLines("build/FFTout.txt"); //hardcoded for easy debugging
        string[] SplitInfoData = allLines[0].Split(' ');
        int samplerate = int.Parse(SplitInfoData[0].Replace("samplerate:", ""));
        int fftsize = int.Parse(SplitInfoData[1].Replace("fftsize:", ""));
        int binAmount = fftsize / 2 + 1;
        int centerfrequency = int.Parse(SplitInfoData[2].Replace("centerfrequency:", ""));
        int newcenterfrequency = int.Parse(SplitInfoData[3].Replace("newcenterfrequency:", ""));
        int filterfrequencyfromcenter = int.Parse(SplitInfoData[4].Replace("filterfrequencyfromcenter:", ""));
        int FFTruns = allLines.Length - 1;
        int maxFrequency = newcenterfrequency + filterfrequencyfromcenter;
        int FrequencyOffset = centerfrequency - newcenterfrequency;
        Console.WriteLine("\nsamplerate: {0}, size: {1}, cf: {2}, ncf: {3}, fffc: {4}", samplerate, fftsize, centerfrequency, newcenterfrequency, filterfrequencyfromcenter);
        Console.WriteLine("{0} FFT runs", FFTruns);


        //get usable bin amount
        binAmount = (int)Math.Ceiling((double)(maxFrequency * fftsize) / samplerate);


        float[,] FFTData = new float[FFTruns, binAmount];
        float max = 0; //for normalization
        for(int i = 0; i < FFTruns; i++)
        {
            string[] SplitFFTData = allLines[i + 1].Split(',');
            int j2 = binAmount - 1;
            for(int j = 0; j < binAmount; j++)
            {
                FFTData[i, j] = float.Parse(SplitFFTData[j2]);
                if(float.Parse(SplitFFTData[j]) > max)
                {
                    max = float.Parse(SplitFFTData[j]);
                    Console.WriteLine("Peak frequency: {0}", ((float)j * samplerate / fftsize) + FrequencyOffset);
                }
                j2--;
            }
        }

        //Normalize
        float[,] FFTDataNormalized = new float[FFTruns, binAmount];
        for(int i = 0; i < FFTruns; i++)
        {
            for(int j = 0; j < binAmount; j++)
            {
                FFTDataNormalized[i, j] = FFTData[i, j] / max;
            }
        }


        Bitmap bmp = new Bitmap(FFTruns, binAmount);
        for(int i = 0; i < FFTruns; i++)
        {
            for(int j = 0; j < binAmount; j++)
            {
                bmp.SetPixel(i, j, Color.FromArgb(0,0, (int)(FFTDataNormalized[i, j] * 255)));
            }
        }
        bmp.Save("build/FFTout.png");
    }

    static void AddLineFFT(int[] data, int xPos)
    {

    }
}