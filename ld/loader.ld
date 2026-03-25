GROUP(libgcc.a libc.a libm.a)

SECTIONS {
	.firmware_hook : {
		*(.firmware_hook)
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