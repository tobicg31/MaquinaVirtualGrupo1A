	mov ac, 4
	shl ac, 1
	mov edx, ac
	ldl ecx, 1
	ldh ecx, 4
	mov eax, 1
	sys 2
	stop