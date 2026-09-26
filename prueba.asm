    mov [0], 1073741824
	shl [0], 2
	mov edx, ds
	ldh ecx, 4
	ldl ecx, 1
	mov eax, 1
	sys 2
	stop