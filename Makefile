ABS_TOP_DIR:=$(shell pwd)
REL_APPDIR :=app
REL_LIBDIR :=lib
REL_OSDIR  :=os
REL_OUTDIR :=out

OUT_APPDIR :=$(REL_OUTDIR)/$(REL_APPDIR)
OUT_LIBDIR :=$(REL_OUTDIR)/$(REL_LIBDIR)
OUT_OSDIR  :=$(REL_OUTDIR)/$(REL_OSDIR)

all: clean boot

boot:
	mkdir -p $(REL_OUTDIR) $(OUT_APPDIR) $(OUT_LIBDIR) $(OUT_OSDIR)
	$(MAKE) -f Makefile.boot TOP_DIR=$(ABS_TOP_DIR)
	#$(MAKE) -f Makefile.boot_stm TOP_DIR=$(ABS_TOP_DIR)

clean:
	$(MAKE) -f Makefile.boot TOP_DIR=$(ABS_TOP_DIR) clean
	#$(MAKE) -f Makefile.boot_stm TOP_DIR=$(ABS_TOP_DIR) clean

.PHONY: boot clean
