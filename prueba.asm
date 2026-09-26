	mov [0], 0
	ldh [0], 0x7FFF
	ldl [0], 0xFFFF
	shl [0], 1
	mov edx, ds
	ldl ecx, 1
	ldh ecx, 4
	mov eax, 1
	sys 1
	stop