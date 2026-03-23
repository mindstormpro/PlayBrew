GROUP(libgcc.a libc.a libm.a)

SECTIONS {
	.text : {
		*(.text)
		
		KEEP(*(.init))
		KEEP(*(.fini))
		
		*(.rodata*)
	}
	
	.data : {
		. = ALIGN(4);
		*(.data*)
	}
	
	.bss : {
		. = ALIGN(4);
		*(.bss*)
		*(COMMON)
		. = ALIGN(4);
	}
}