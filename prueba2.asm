\\DATA 100
\\STACK 256

SALUDO EQU "Argumentos recibidos correctamente\n"

main:   push bp
        mov  bp, sp
        mov  w[edx], 10
        ldl  ecx, 1
        ldh  ecx, 4
        mov  eax, 1         
        sys  2    
        mov  ebx, [bp+12]
        mov  edx, [ebx]     
        sys  4      
fin:    mov  sp, bp
        pop  bp
        ret