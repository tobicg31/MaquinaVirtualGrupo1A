    mov edx, ds
	add [edx], 4
	cmp eax, edx
	sys 1
	stop