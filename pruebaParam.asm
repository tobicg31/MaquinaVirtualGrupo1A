main:
        push bp
        mov bp, sp
        mov edx, 16  
        sys 4
        sys 15
        mov sp, bp
        pop bp
        ret