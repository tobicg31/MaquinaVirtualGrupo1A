<<<<<<< Updated upstream
    mov [0], 1073741824
	shl [0], 2
	mov edx, ds
	ldh ecx, 4
	ldl ecx, 1
=======
	mov [0], 0
	ldl [0], 0xFFFB
	ldh [0], 0xFFFF
	mov edx, ds
	ldl ecx, 1
	ldh ecx, 4
>>>>>>> Stashed changes
	mov eax, 1
	sys 2
	stop