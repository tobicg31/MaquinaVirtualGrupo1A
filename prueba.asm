<<<<<<< HEAD
<<<<<<< Updated upstream
    mov [0], 1073741824
=======
	mov [0], 0
	ldh [0], 0x4000
>>>>>>> 1e38db2de3bb313cbc342ca8d6293a393e213597
	shl [0], 2
	mov edx, ds
	ldl ecx, 1
<<<<<<< HEAD
=======
	mov [0], 0
	ldl [0], 0xFFFB
	ldh [0], 0xFFFF
	mov edx, ds
	ldl ecx, 1
	ldh ecx, 4
>>>>>>> Stashed changes
=======
	ldh ecx, 4
>>>>>>> 1e38db2de3bb313cbc342ca8d6293a393e213597
	mov eax, 1
	sys 2
	stop