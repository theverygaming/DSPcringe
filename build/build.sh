#!/bin/bash
rm decimate
cmake ..
make -j 6
./decimate
