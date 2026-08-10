GROUP(libgcc.a libc.a libm.a)

SECTIONS {
	.up : {
		*(.up)
	}
	
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
}