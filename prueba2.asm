main:   push bp
        mov bp, sp
        mov eax, 10
        mov edx, 20
        add eax, dx
        push eax
        mov  edx, sp                ; EDX apunta al valor en la pila
        ldl  ecx, 1                 ; cantidad = 1
        ldh  ecx, 4                 ; tamanio de celda = 4 bytes
        mov  eax, 1                 ; formato decimal
        sys  2                      ; deberia mostrar 30
        add  sp, 4

        ; 3) SYS 3: leer una frase (con espacios) en el Data Segment
        mov  edx, ks
        add  edx, PIDE
        sys  4
        mov  edx, ds                ; EDX = &frase (offset 0 del DS)
        mov  ecx, 80                ; maximo de caracteres
        sys  3
        mov  edx, ds
        sys  4                      ; la imprime de vuelta
        mov  edx, ks
        add  edx, NL
        sys  4

        ; 4) subrutina que recorre el string con b[...]
        push ds                     ; parametro: puntero a la frase
        call slen                   ; EAX = largo
        add  sp, 4
        mov  ebx, eax
        mov  edx, ks
        add  edx, LARGO
        sys  4
        push ebx
        mov  edx, sp
        ldl  ecx, 1
        ldh  ecx, 4
        mov  eax, 1
        sys  2                      ; largo de la frase
        add  sp, 4

        ; 5) breakpoint: probar Enter (paso a paso), g (seguir) y q (salir)
        sys  15
        mov  eax, 7
        add  eax, 1
        add  eax, 1

        ; 6) argc y argv[0] que deja la VM en la pila
        push [bp+8]                 ; argc
        mov  edx, sp
        ldl  ecx, 1
        ldh  ecx, 4
        mov  eax, 1
        sys  2
        add  sp, 4
        cmp  [bp+12], -1            ; -1 = no hay parametros
        jz   fin
        mov  ebx, [bp+12]           ; EBX = argv
        mov  edx, [ebx]             ; EDX = argv[0]
        sys  4
        mov  edx, ks
        add  edx, NL
        sys  4

fin:    mov  sp, bp
        pop  bp
        ret

suma:   push bp
        mov  bp, sp
        mov  eax, [bp+8]            ; 1er parametro
        add  eax, [bp+12]           ; 2do parametro
        mov  sp, bp
        pop  bp
        ret

slen:   push bp
        mov  bp, sp
        push edx
        mov  edx, [bp+8]            ; puntero al string
        mov  eax, 0
sl1:    cmp  b[edx], 0
        jz   sl2
        add  eax, 1
        add  edx, 1
        jmp  sl1
sl2:    pop  edx
        mov  sp, bp
        pop  bp
        ret