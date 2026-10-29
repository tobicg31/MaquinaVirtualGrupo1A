main:   push bp
        mov bp, sp

        mov eax, 5
        mov ebx, 7
        add eax, ebx      ; EAX ahora vale 12 (0x0000000C)
        sys 15

        push eax          ; Guardamos el 12 en la memoria (Stack Segment)
        mov edx, sp       ; Apuntamos EDX a esa direccion para el SYS 2 de mas abajo
        
        mov eax, 999      ; Sobreescribimos EAX con "basura" a proposito

        ; Si frenas aqui, al retomar el .vmi EAX debe valer 999.
        ; Ademas, el emulador debe recordar que EDX apunta a la pila, 
        ; donde todavia esta guardado el numero 12.
        sys 15

        ldl ecx, 1        ; Cantidad a imprimir: 1
        ldh ecx, 4        ; Tamaño: 4 bytes
        mov eax, 1        ; Formato: Decimal
        
        ; Como el emulador recordo EDX (que apunta al 12 en la pila), 
        ; esto DEBE imprimir "12" y no "999".
        sys 2             

        pop eax           ; Recuperamos el 12 de la pila. EAX vuelve a valer 12.

        ; Si frenas aqui, ya deberias ver el "12" impreso en tu consola.
        ; Y si inspeccionas EAX en el VMI, habra vuelto a ser 12.
        sys 15

        mov sp, bp
        pop bp
        ret