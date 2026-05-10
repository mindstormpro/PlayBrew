ENTRY(playbrew_payload_entry)

SECTIONS
{
	. = 0;

	.text :
	{
		KEEP(*(.playbrew.entry))
		*(.text*)
		*(.rodata*)
	}

	.data :
	{
		*(.data*)
	}

	.bss (NOLOAD) :
	{
		*(.bss*)
		*(COMMON)
	}

	/DISCARD/ :
	{
		*(.ARM.exidx*)
		*(.comment)
		*(.note*)
	}
}
