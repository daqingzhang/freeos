OUTPUT_FORMAT("elf32-littearm","elf32-littearm","elf32-littearm")
OUTPUT_ARCH(arm)
ENTRY(_start)

MEMORY
{
	ram (rw)   : o = 0x20000000, l = 0x5000  /* 20KB RAM */
	flash (rx) : o = 0x08000000, l = 0x1f800 /* 126K FLASH */
}

ASSERT(ORIGIN(flash) == ALIGN(ORIGIN(FLASH), 0x80), "ERROR: flash memory is not aligned !");

SECTIONS
{
	.text : {
		__isr_vectors_offs__ = . - 0x08000000;
		lib/core/startup/startup.o	(.text)
		. = ALIGN(4);
		lib/board/stm/source/*		(.text) /* objects */
		*(.text .text.*)
		. = ALIGN(4);
	} >flash

	.rodata : {
		*(SORT_BY_ALIGNMENT(SORT_BY_NAME(.rodata*)))
		. = ALIGN(4);
	} >flash
	__flash_end__ = . ;

	.data : {
		__start_data__ = . ;
		. = ALIGN(4);

		*(.data .data.*)
		*(.ram)
		*(.ramfunc*)
		. = ALIGN(4);
		__end_data__ . ;
	} >ram AT>flash

	.bss (NOLOAD) : {
		__start_bss__ = . ;
		*(.shbss)
		*(.bss .bss.*)
		*(COMMON)
		. = ALIGN(8);
		*(.ram.b)
		. = ALIGN(4);
		__end_bss__ = . ;
	} >ram AT>flash

	__ram_end__ = . ;
	_end = . ;

	/DISCARD/ : { *(.note.GNU-stack) *(.gnu_debuglink) }
}

ASSERT(__flash_end__ < (ORIGIN(flash) + LENGTH(flash)), "ERROR: no enough space for store code in FLASH !");
ASSERT(__ram_end__ < (ORIGIN(ram) + LENGTH(ram)), "ERROR: no enough space for store data in RAM!");

