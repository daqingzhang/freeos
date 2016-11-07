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
		lib/core/arm/source/startup_stm.o	(.text)
		. = ALIGN(4);
		lib/board/stmf10x/source/*		(.text) /* objects */
		*(.text .text.*)
		. = ALIGN(4);
	} >flash
	__text_end__ = . ;
	.rodata : {
		*(SORT_BY_ALIGNMENT(SORT_BY_NAME(.rodata*)))
		. = ALIGN(4);
	} >flash
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
	} >ram AT>flash

	__ram_end__ = . ;
	_end = . ;

	/DISCARD/ : { *(.note.GNU-stack) *(.gnu_debuglink) }
}

ASSERT(__flash_end__ < (ORIGIN(flash) + LENGTH(flash)), "ERROR: no enough space for store code in FLASH !");
ASSERT(__ram_end__ < (ORIGIN(ram) + LENGTH(ram)), "ERROR: no enough space for store data in RAM!");

