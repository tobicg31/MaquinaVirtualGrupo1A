	mov [0], 0
	ldh [0], 0x4000
	shl [0], 2
	mov edx, ds
	ldl ecx, 1
	ldh ecx, 4
	mov eax, 1
	sys 2
	stop