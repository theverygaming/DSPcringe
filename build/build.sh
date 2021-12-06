#!/bin/bash
rm decimate
cmake ..
make -j ${nproc}
./decimate
