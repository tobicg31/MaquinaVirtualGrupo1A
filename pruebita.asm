main:   push bp     
        mov bp, sp
        mov eax, ds
        mov w[eax], 1
        push 10
        call suma
        add sp, 4
        mov edx, ds
        ldl ecx, 1
        ldh ecx, 2
        mov eax, 1
        sys 2
        pop bp 
        mov sp, bp  
        ret


suma:   push bp 
        mov bp, sp      
        add w[ds], 10
        mov sp, bp
        pop bp
        ret