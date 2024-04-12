#!/usr/bin/sh

function jumpto
{
    label=$1
    cmd=$(sed -n "/$label:/{:a;n;p;ba};" $0 | grep -v ':$')
    eval "$cmd"
    exit
}

TARGET=$1
MODE=$2
KERNEL=spmv
CU_NUM=1
PLATFORM=xilinx_u250_gen3x16_xdma_4_1_202210_1
#PLATFORM=xilinx_u280_gen3x16_xdma_1_202211_1

FORMAT=csr
BUILD_DIR=../../build

# NOTE(miha): init xrt, xilinx stuff
. /ceph/grid/software/xilinx/tools/Vitis/2023.1/settings64.sh
. /opt/xilinx/xrt/setup.sh
export PLATFORM_REPO_PATHS=/opt/xilinx/platforms/${PLATFORM}
export SYSROOT=/ceph/grid/software/xilinx/2023/
#module load foss/2022a

# . /opt/xilinx/xrt/setup.sh
# . /opt/xilinx/software/Vitis/2020.2/settings64.sh
# . /opt/xilinx/software/Vivado/2020.2/settings64.sh
# export PLATFORM_REPO_PATHS=/opt/xilinx/platforms/${PLATFORM}


if [ "$MODE" == "sw_emu" ] || [ "$MODE" == "hw_emu" ] || [ "$MODE" == "hw" ]; then
    echo "Using target: $TARGET"
    echo "Using mode: $MODE"
else
    if [ "$TARGET" == "help" ] || [ "$TARGET" == "clean" ]; then
        echo "Using target: $TARGET"
    else
        echo "Mode (third argument) should be {sw_emu | hw_emu | hw}."
        jumpto help
    fi
fi

start=${1:-"start"}

jumpto $start

start:

if [[ "$#" -eq 1 ]]; then
    jumpto help
fi
if [[ "$TARGET" == "help" ]]; then
    jumpto help
fi
if [[ "$TARGET" == "all" ]]; then
    jumpto all
fi
if [[ "$TARGET" == "clean" ]]; then
    jumpto clean
fi
if [[ "$TARGET" == "00" ]]; then
    jumpto _00
fi
jumpto end

help:
	echo "Usage: ./build.sh target mode"
	echo "You can run next commands:"
	echo "  - help"
	echo "    Print this help message"
	echo "  - all"
	echo "    Build all targets"
	echo "  - 00"
	echo "    Build 00 version."
jumpto end

all:
jumpto end

clean:
    echo "Cleaning: $BUILD_DIR/$FORMAT"
	rm -rf $BUILD_DIR/$FORMAT
jumpto end

_00:
# CARE(miha): Change this variables for different targets
TARGET_DIR=$PWD/00-basic

# NOTE(miha): Init dirs
BUILD_DIR=$BUILD_DIR/$FORMAT/$TARGET/$MODE
mkdir -p $BUILD_DIR
cp ../utils/xrt.ini $BUILD_DIR/xrt.ini
cp ../utils/fpga.cfg $BUILD_DIR/fpga.cfg

pushd $BUILD_DIR > /dev/null

g++ -g -std=c++17 -Wall -O0 $TARGET_DIR/host.cpp -I$XILINX_XRT/include -I$XILINX_VIVADO/include -lOpenCL -pthread -o ./host
emconfigutil --platform $PLATFORM
# v++ -c -t $MODE --connectivity.nk $KERNEL:$CU_NUM --platform $PLATFORM --config fpga.cfg -k $KERNEL $TARGET_DIR/$KERNEL.cpp -o ./$KERNEL.xo
# v++ -l -t $MODE --connectivity.nk $KERNEL:$CU_NUM --connectivity.sp spmv_1.a:DDR[0] --connectivity.sp spmv_1.b:DDR[1] --connectivity.sp spmv_1.c:DDR[0]  --platform $PLATFORM --config fpga.cfg ./$KERNEL.xo -o ./$KERNEL.xclbin
v++ -c -t $MODE --connectivity.nk $KERNEL:$CU_NUM --platform $PLATFORM --config fpga.cfg -k $KERNEL $TARGET_DIR/$KERNEL.cpp -o ./$KERNEL.xo
v++ -l -t $MODE --connectivity.nk $KERNEL:$CU_NUM --platform $PLATFORM --config fpga.cfg ./$KERNEL.xo -o ./$KERNEL.xclbin

mkdir -p cp ../../../../kernels/$FORMAT/$TARGET/$MODE
cp ./$KERNEL.xclbin ../../../../kernels/$FORMAT/$TARGET/$MODE

popd > /dev/null
jumpto end

end: 