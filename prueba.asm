otro:	mov [0], 0
	add [0], 1
	mov edx, ds 
	ldh ecx, 1
	ldl ecx, 1
	mov eax, 1
	sys 2
	cmp [0], 10
	jz otro
	stop