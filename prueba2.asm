\\DATA 100
\\STACK 256

SALUDO EQU "Argumentos recibidos correctamente\n"

main:   push bp
        mov  bp, sp

        ; 1) Mostrar argc (está en [bp+8])
        push [bp+8]
        mov  edx, sp
        ldl  ecx, 1
        ldh  ecx, 4
        mov  eax, 1         ; Formato decimal
        sys  2              ; Imprime el valor de argc
        add  sp, 4

        ; 2) Verificar si argc == 0 para saltar el uso de argv
        cmp  [bp+8], 0
        jz   fin
        sys 15

        ; 3) Mostrar argv[0] (el puntero al primer string está en [bp+12])
        mov  ebx, [bp+12]   ; EBX apunta al inicio de los argumentos
        mov  edx, [ebx]     ; EDX obtiene la dirección lógica del string argv[0]
        sys  4              ; Imprime el string usando el Const/Param Segment

fin:    mov  sp, bp
        pop  bp
        ret