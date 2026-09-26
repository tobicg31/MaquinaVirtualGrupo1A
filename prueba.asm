	mov [0], 2000
	shr [0], 2
	mov edx, ds
	ldl ecx, 1
	ldh ecx, 4
	mov eax, 1
	sys 2
	stop