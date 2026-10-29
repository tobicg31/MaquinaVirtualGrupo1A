main:   push bp
        mov bp, sp
        mov eax, 10
        mov edx, 20
        add eax, dx
        push eax
        mov edx, sp

        ldl ecx, 1
        ldh ecx, 4
        mov eax, 1
        sys 2
        sys 15
        pop eax
        mov sp, bp
        pop bp  
        ret