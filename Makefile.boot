CHIP		:= stm32f10x
CORE		:= arm
OS		:= FreeRTOS
ARCH		:= ARM_CM3

#==============================================================================
# DIRECTORY
#==============================================================================
LIBDIR		:= lib
APPDIR		:= app
OSDIR		:= os
OUTDIR		:= out
OUTLIB_DIR	:= $(TOP_DIR)/$(OUTDIR)/$(LIBDIR)
OUTAPP_DIR	:= $(TOP_DIR)/$(OUTDIR)/$(APPDIR)
OUTOS_DIR	:= $(TOP_DIR)/$(OUTDIR)/$(OSDIR)

APP_INC		:= $(APPDIR)/include
LIB_INC		:= $(LIBDIR)/include
LIB_BOARD_INC	:= $(LIBDIR)/board/$(CHIP)/include
LIB_BOARD_SRC	:= $(LIBDIR)/board/$(CHIP)
LIB_CORE_INC	:= $(LIBDIR)/core/$(CORE)/include
LIB_CORE_SRC	:= $(LIBDIR)/core/$(CORE)
OS_DEVICE_INC	:= $(OSDIR)/device/include
OS_DEVICE_SRC	:= $(OSDIR)/device/$(CHIP)
OS_FREEOS_CFG	:= $(OSDIR)/$(OS)/config
OS_FREEOS_INC	:= $(OSDIR)/$(OS)/Source/include
OS_FREEOS_SRC	:= $(OSDIR)/$(OS)/Source
OS_FREEOS_PORT	:= $(OSDIR)/$(OS)/Source/portable/GCC/$(ARCH)
OS_FREEOS_MEM	:= $(OSDIR)/$(OS)/Source/portable/MemMang

INCS		:= -I$(LIB_INC)
INCS		+= -I$(LIB_BOARD_INC)
INCS		+= -I$(LIB_CORE_INC)
INCS		+= -I$(OS_DEVICE_INC)
INCS		+= -I$(APP_INC)
INCS		+= -I$(OS_FREEOS_CFG)
INCS		+= -I$(OS_FREEOS_INC)
INCS		+= -I$(OS_FREEOS_PORT)

#==============================================================================
# TARGET FILES
#==============================================================================
TARGET		:= boot
TARGET_ELF	:= $(TARGET).elf
TARGET_BIN	:= $(TARGET).bin
TARGET_HEX	:= $(TARGET).hex
TARGET_MAP	:= $(TARGET).map
TARGET_SYM	:= $(TARGET).symbols
TARGET_ELF_DUMP := $(TARGET_ELF).dump
TARGET_BIN_DUMP := $(TARGET_BIN).dump
TARGET_HEX_DUMP := $(TARGET_HEX).dump

#==============================================================================
# COMPILER
#==============================================================================
TC	:= arm-none-eabi
CC	:= $(TC)-gcc
LD	:= $(TC)-ld
AS	:= $(TC)-as
AR	:= $(TC)-ar
GDB	:= $(TC)-gdb
OBJCOPY := $(TC)-objcopy
OBJDUMP := $(TC)-objdump

CC_OPTS		:= -g -mthumb -mcpu=cortex-m3 -march=armv7-m
CC_OPTS		+= -O2 -Wall -static -fno-common -fno-builtin-printf

LD_OPTS		:= -T boot_stm.ld $(INCS)
LD_OPTS		+= -Wl,-nostdlib,--relax,-Map=$(TARGET_MAP),--gc-sections
LD_OPTS		+= -nostartfiles -ffast-math -lgcc

OBJDUMP_OPTS	:= --disassemble-all
OBJDUMP_OPTS	+= --section=.text --section=.test.startup --section=.data

#==============================================================================
# MACRO DEFINITIONS
#==============================================================================
TARGET_DEFS	:= -DHOST_DEBUG=1
TARGET_DEFS	+= -DSTM32F10X_MD
TARGET_DEFS	+= -DCONFIG_USE_FREERTOS
TARGET_DEFS	+= -DGCC_ARMCM3
#==============================================================================
# OBJECTS & SOURCE FILES
#==============================================================================
A_OBJS := \
	./$(LIB_CORE_SRC)/startup_stm.o \
	./$(LIB_CORE_SRC)/core_op.o

C_OBJS_LIB := \
	./$(LIB_CORE_SRC)/syscall.o		\
	./$(LIB_BOARD_SRC)/stm32f10x_system.o	\
	./$(LIB_BOARD_SRC)/stm32f10x_rcc.o	\
	./$(LIB_BOARD_SRC)/stm32f10x_gpio.o	\
	./$(LIB_BOARD_SRC)/stm32f10x_usart.o

C_OBJS_DEV := \
	./$(OS_DEVICE_SRC)/string.o	\
	./$(OS_DEVICE_SRC)/serial.o	\
	./$(OS_DEVICE_SRC)/timer.o	\
	./$(OS_DEVICE_SRC)/key.o	\
	./$(OS_DEVICE_SRC)/motor.o	\
	./$(OS_DEVICE_SRC)/led.o

C_OBJS_APP := \
	./$(APPDIR)/main.o \
	./$(APPDIR)/led_fsm.o \
	./$(APPDIR)/led_task.o \
	./$(APPDIR)/key_task.o

C_OBJS_OS := \
	./$(OS_FREEOS_MEM)/heap_2.o	\
	./$(OS_FREEOS_PORT)/port.o	\
	./$(OS_FREEOS_SRC)/croutine.o	\
	./$(OS_FREEOS_SRC)/event_groups.o	\
	./$(OS_FREEOS_SRC)/list.o	\
	./$(OS_FREEOS_SRC)/queue.o	\
	./$(OS_FREEOS_SRC)/tasks.o	\
	./$(OS_FREEOS_SRC)/timers.o

C_OBJS	:= $(C_OBJS_LIB) \
	$(C_OBJS_DEV) 	\
	$(C_OBJS_APP)	\
	$(C_OBJS_OS)	\

#==============================================================================
# SECTIONS
#==============================================================================
all: $(TARGET_BIN_DUMP) $(TARGET_HEX_DUMP) $(TARGET_ELF_DUMP)
	mv -f $(LIB_CORE_SRC)/*.o $(OUTLIB_DIR)
	mv -f $(LIB_BOARD_SRC)/*.o $(OUTLIB_DIR)
	mv -f $(APPDIR)/*.o $(OUTAPP_DIR)
	mv -f $(OS_DEVICE_SRC)/*.o $(OUTOS_DIR)
	mv -f $(OS_FREEOS_MEM)/*.o $(OUTOS_DIR)
	mv -f $(OS_FREEOS_PORT)/*.o $(OUTOS_DIR)
	mv -f $(OS_FREEOS_SRC)/*.o $(OUTOS_DIR)
	@echo "Build done !"

$(TARGET_BIN_DUMP): $(TARGET_BIN)
	xxd $< > $@

$(TARGET_HEX_DUMP): $(TARGET_HEX)
	hexdump $< > $@

$(TARGET_ELF_DUMP): $(TARGET_ELF)
	$(OBJDUMP) $(OBJDUMP_OPTS) $< > $@
	$(OBJDUMP) -t $< > $(TARGET_SYM)
	$(OBJDUMP) -h $<

$(TARGET_BIN): $(TARGET_ELF)
	$(OBJCOPY) -O binary $< $@

$(TARGET_HEX): $(TARGET_ELF)
	$(OBJCOPY) -O ihex $< $@

$(TARGET_ELF): $(C_OBJS) $(A_OBJS)
	$(CC) $^ -o $@ $(LD_OPTS)

%.o: %.c
	$(CC) $(CC_OPTS) $(TARGET_DEFS)	\
		-c $(INCS) $< -o $@

%.o: %.S
	$(CC) $(CC_OPTS) $(TARGET_DEFS)	\
		-c $(INCS) $< -o $@

JUNK	:= $(TARGET_ELF) $(TARGET_BIN) $(TARGET_HEX) $(TARGET_MAP) \
	$(TARGET_SYM) $(TARGET_ELF_DUMP) $(TARGET_BIN_DUMP) $(TARGET_HEX_DUMP)

clean:
	rm -rf $(JUNK)
	rm -f  $(LIB_BOARD_SRC)/*.o
	rm -f  $(LIB_CORE_SRC)/*.o
	rm -f  $(OS_DEVICE_SRC)/*.o
	rm -f  $(OUTLIB_DIR)/*.o
	rm -f  $(OUTAPP_DIR)/*.o
	rm -f  $(OUTOS_DIR)/*.o

