	mov eax, 0
    ldh eax, 0x4000
    ldl eax, 0x0000
    shl eax, 2
	mov [0], eax
	mov edx, ds
	ldl ecx, 1
	ldh ecx, 4
	mov eax, 1
	sys 2
	stop