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
	
	/*
	 * The loader is appended to firmware and linked with -nostartfiles, so no
	 * C runtime copies .data into RAM or clears .bss before playbrew_loader_load.
	 * Keep writable state out of the loader until startup code explicitly
	 * supports it.
	 */
	. = ALIGN(4);
	.data : {
		__loader_data_start = .;
		*(.data*)
		__loader_data_end = .;
	}

	. = ALIGN(4);
	.bss (NOLOAD) : {
		__loader_bss_start = .;
		*(.bss*)
		*(COMMON)
		__loader_bss_end = .;
	}

	ASSERT(__loader_data_start == __loader_data_end, "PlayBrew loader cannot use .data without startup copy support")
	ASSERT(__loader_bss_start == __loader_bss_end, "PlayBrew loader cannot use .bss without startup clear support")
}
