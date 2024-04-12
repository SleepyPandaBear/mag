SHELL := /bin/bash

FORMAT:= csr
KERNEL:=$(FORMAT)
BUILD_DIR:= ../../build
MODE = $(error Please set flag MODE, i.e. MODE={sw_emu, hw_emu, hw})
PLATFORM:=xilinx_u250_gen3x16_xdma_4_1_202210_1
PWD:=$(shell pwd)

check_args:
	echo "Using mode $(MODE)"
	# check if building or running

all: check_args 00
	echo "all"

00:
	source /ceph/grid/software/xilinx/tools/Vitis/2023.1/settings64.sh; /
    source /opt/xilinx/xrt/setup.sh; /
    export PLATFORM_REPO_PATHS=/opt/xilinx/platforms/${PLATFORM}/; /
	mkdir -p $(BUILD_DIR)/$(FORMAT)/$@; /
	cp ../utils/xrt.ini $(BUILD_DIR)/$(FORMAT)/$@/xrt.ini; /
	cp ../utils/fpga.cfg $(BUILD_DIR)/$(FORMAT)/$@/fpga.cfg; /
	
	cd $(BUILD_DIR)/$(FORMAT)/$@; /
	g++ -g -std=c++17 -Wall -O0 $PWD/00-basic/host.cpp -I$XILINX_XRT/include -I$XILINX_VIVADO/include -lOpenCL -pthread -o ./host; /
	emconfigutil --platform $PLATFORM; /
	v++ -c -t $MODE --platform $PLATFORM --config fpga.cfg -k $KERNEL $DIR/$KERNEL.cpp -o ./$KERNEL.xo; /
	v++ -l -t $MODE --platform $PLATFORM  --config fpga.cfg ./$KERNEL.xo -o ./$KERNEL.xclbin; /
run_00:
	echo "run sw 00"

clean:
	rm -rf $(BUILD_DIR)/$(FORMAT)