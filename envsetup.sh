FREEOS_CUR_DIR=$(pwd)
PATH_ARMGCC_BIN=$FREEOS_CUR_DIR/tools/gcc-arm-none-eabi/bin
#PATH_ARMGCC_LIB=~/

#export LD_LIBRARY_PATH=$PATH_ARMGCC_LIB
export PATH=$PATH:$PATH_ARMGCC_BIN
export FREEOS_TOP_DIR=$FREEOS_CUR_DIR
export FREEOS_BOARD_DIR=$FREEOS_TOP_DIR/lib/board/stm32f10x
export FREEOS_CORE_DIR=$FREEOS_TOP_DIR/lib/core/arm

freeosenv()
{
	echo
	echo "FREEOS_TOP_DIR=$FREEOS_TOP_DIR"
	echo "FREEOS_BOARD_DIR=$FREEOS_BOARD_DIR"
	echo "FREEOS_CORE_DIR=$FREEOS_CORE_DIR"
	echo "PATH=$PATH"
	echo
}

croot()
{
	cd $FREEOS_TOP_DIR
}

cdlib()
{
	cd $FREEOS_BOARD_DIR
}

cdcore()
{
	cd $FREEOS_CORE_DIR
}

lsout()
{
	echo "out/lib"
	ls -l out/lib

	echo "out/app"
	ls -l out/app

	echo "out/os"
	ls -l out/os
}

freeosenv
