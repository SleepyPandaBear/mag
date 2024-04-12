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

PLATFORM=xilinx_u250_gen3x16_xdma_4_1_202210_1
#PLATFORM=xilinx_u280_gen3x16_xdma_1_202211_1

FORMAT=csr
BUILD_DIR=../../build

# NOTE(miha): init xrt, xilinx stuff
. /ceph/grid/software/xilinx/tools/Vitis/2023.1/settings64.sh
. /opt/xilinx/xrt/setup.sh
export PLATFORM_REPO_PATHS=/opt/xilinx/platforms/${PLATFORM}/


if [ "$MODE" == "sw_emu" ] || [ "$MODE" == "hw_emu" ] || [ "$MODE" == "hw" ]; then
    echo "Using target: $TARGET"
    echo "Using mode: $MODE"
else
    if [ "$TARGET" == "help" ]; then
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
if [[ "$TARGET" == "00" ]]; then
    jumpto _00
fi
jumpto end

help:
	echo "Usage: ./run.sh target mode"
	echo "You can run next commands:"
	echo "  - help"
	echo "    Print this help message"
	echo "  - 00"
	echo "    Run 00 version."
jumpto end

_00:
# CARE(miha): Change this variables for different targets
TARGET_DIR=$PWD/00-basic

BUILD_DIR=$BUILD_DIR/$FORMAT/$TARGET/$MODE
pushd $BUILD_DIR > /dev/null

XCL_EMULATION_MODE=$MODE ./host

popd > /dev/null
jumpto end

end: 