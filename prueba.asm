	mov [0], 0
	ldl [0], 0xFFFB
	ldh [0], 0xFFFF
	mov edx, ds
	ldl ecx, 1
	ldh ecx, 4
	mov eax, 1
	sys 2
	stop