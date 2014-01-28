[BITS 32]
[GLOBAL flush_tss]


flush_tss:
	cli
	mov ax, 0x2b
	ltr ax	
	sti
	ret
