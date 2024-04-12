#!/bin/bash

CODE=$PWD

pushd .. >/dev/null # go to the parent dir
if [ ! -d "build" ]; then
	mkdir build
fi
pushd build >/dev/null

cp $CODE/xrt.ini .
cp $CODE/build.tcl .

. /tools/xilinx/Vitis/2022.1/settings64.sh
vitis_hls -f build.tcl
cp $CODE/../build/spmv_hls/solution/syn/report/csynth.rpt $CODE/report.rpt
