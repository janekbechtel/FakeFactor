#!/bin/bash

echo "Compiling the framework"
sh BuildStructure.sh
cd ../
make -B
#./Preselection
./SRHisto
