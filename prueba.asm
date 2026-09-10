otro:   mov edx, ds
	add edx, 4
	ldl ecx, 1
	ldh ecx, 4
	jmp otro
	sys 1
	stop