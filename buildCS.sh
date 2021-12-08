#!/bin/bash
rm FFTDataToImage.exe
#csc FFTDataToImage.cs
mcs /reference:System.Drawing.dll FFTDataToImage.cs
mono FFTDataToImage.exe