ABS_TOP_DIR := /home/zhangdaqing/workspace/github/freeos
REL_LIBDIR  := lib
REL_APPDIR  := app
REL_OSDIR   := os
REL_OUTDIR  := out
REL_OUTAPPDIR :=$(REL_OUTDIR)/app
REL_OUTLIBDIR :=$(REL_OUTDIR)/lib
REL_OUTOSDIR  :=$(REL_OUTDIR)/os

all: clean stm

stm:
	mkdir -p $(REL_OUTAPPDIR) $(REL_OUTLIBDIR) $(REL_OUTOSDIR)
	$(MAKE) -f Makefile.boot_stm TOP_DIR=$(ABS_TOP_DIR)

clean:
	$(MAKE) -f Makefile.stm TOP_DIR=$(ABS_TOP_DIR) clean

.PHONY: stm clean
