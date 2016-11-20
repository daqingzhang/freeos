OUTPUT_FORMAT("elf32-littlearm","elf32-littlearm","elf32-littlearm")
OUTPUT_ARCH(arm)
ENTRY(_start)

MEMORY
{
	flash (rx) : o = 0x08000000, l = 0x1f800 /* 126K FLASH */
	ram   (rw) : o = 0x20000000, l = 0x5000  /* 20KB RAM */
}

ASSERT(ORIGIN(flash) == ALIGN(ORIGIN(flash), 0x80), "ERROR: flash memory is not aligned !");

SECTIONS
{
	.text : {
		__isr_vectors_offs__ = . - 0x08000000;
		KEEP(lib/core/arm/startup_stm.o	(.text))
		KEEP(lib/core/arm/core_op.o	(.text))
		KEEP(lib/core/arm/syscall.o	(.text))
		os/device/stmf10x/*		(.text) /* */
		lib/board/stmf10x/*		(.text) /* */
		*(.text .text.*)
		. = ALIGN(4);
	} >flash
	__text_end__ = . ;

	.rodata : {
		*(SORT_BY_ALIGNMENT(SORT_BY_NAME(.rodata*)))
		. = ALIGN(4);
	} >flash

	.ARM.exidx : {
		*(.ARM.exidx*)
		*(.gnu.linkonce.armexidx.*)
	} >flash

	__rodata_end__ = . ;
	__flash_end__ = . ;

	.data : {
		__data_start__ = . ;
		. = ALIGN(4);

		*(.data .data.*)
		*(.ram)
		*(.ramfunc*)
		. = ALIGN(4);
		__data_end__ = . ;
	} >ram AT>flash

	.bss (NOLOAD) : {
		__bss_start__ = . ;
		*(.shbss)
		*(.bss .bss.*)
		*(COMMON)
		. = ALIGN(8);
		*(.ram.b)
		. = ALIGN(4);
		__bss_end__ = . ;
	} >ram
	__ram_end__ = . ;

	_end = . ;

	/DISCARD/ : { *(.note.GNU-stack) }
	/DISCARD/ : { *(.gnu_debuglink) }
/*
	/DISCARD/ : { *(.dynamic*) }
	/DISCARD/ : { *(.dynstr*) }
	/DISCARD/ : { *(.plt*) }
	/DISCARD/ : { *(.interp*) }
	/DISCARD/ : { *(.gnu*) }
*/
}

ASSERT(__flash_end__ < (ORIGIN(flash) + LENGTH(flash)), "ERROR: no enough space for store code in FLASH !");
ASSERT(__ram_end__ < (ORIGIN(ram) + LENGTH(ram)), "ERROR: no enough space for store data in RAM!");

