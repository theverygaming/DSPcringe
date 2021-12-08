using System;
using System.IO;

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
        Console.WriteLine("\nsamplerate: {0}, size: {1}, cf: {2}, ncf: {3}, fffc: {4}", samplerate, fftsize, centerfrequency, newcenterfrequency, filterfrequencyfromcenter);
        Console.WriteLine("{0} FFT runs", FFTruns);

        float[,] FFTData = new float[FFTruns, binAmount];
        float[,] FFTDataNormalized = new float[FFTruns, binAmount];

        for(int i = 0; i < FFTruns; i++)
        {
            string[] SplitFFTData = allLines[i + 1].Split(',');
            for(int j = 0; j < binAmount; j++)
            {
                FFTData[i, j] = float.Parse(SplitFFTData[j]);
            }
        }
    }

    static void AddLineFFT(int[] data, int xPos)
    {

    }
}