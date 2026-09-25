    mov edx, ds
	add [edx], 4
	cmp eax, edx
	sub eax, 10
	mul eax, 2
	div eax, 1
	sys 1
	stop