	mov [0], 0
	ldl [0], 0xFFFD
	ldh [0], 0xFFFF
	mov ebx, 5
	sub [0], ebx
	mov edx, ds
	ldh ecx, 4
	ldl ecx, 1
	mov eax, 1
	sys 2
	stop