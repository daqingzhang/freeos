PATH_ARMGCC_BIN=~/gcc-arm-none-eabi-5_3-2016q1/bin
PATH_ARMGCC_LIB=~/

export LD_LIBRARY_PATH=$PATH_ARMGCC_LIB
export PATH=$PATH:$PATH_ARMGCC_BIN

armgccenv()
{
	echo
	echo "LD_LIBRARY_PATH=$LD_LIBRARY_PATH"
	echo "PATH=$PATH"
	echo
}

armgccenv
