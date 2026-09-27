inicio: mov edx, ds         ; EDX apunta al inicio del Data Segment (DS+0)
    mov ecx, 0
    ldh ecx, 4          ; Tamaño del dato: 4 bytes
    ldl ecx, 1          ; Cantidad de elementos: 1
    mov eax, 1          ; Formato: Decimal
    sys 1
	mov ebx, [0]        ; Cargamos el número ingresado en EBX
    cmp ebx, 0          ; Lo comparamos contra 0
    jn fin
	mov ecx, 0
contar_bits: cmp ebx, 0          ; Verificamos si el número ya se vació (llegó a 0)
    jz imprimir         ; Si es 0 (Z=1), terminamos de contar y vamos a imprimir

    mov eax, ebx        ; Copiamos el valor actual a EAX para no perder el original
    and eax, 1          ; Aislamos el bit menos significativo (Lógica AND con 00...001)
    add ecx, eax        ; Sumamos el resultado (sumará 1 si el bit era 1, o 0 si era 0)

    shr ebx, 1          ; Desplazamiento lógico a la derecha. Mete un 0 por la izquierda.
    jmp contar_bits
imprimir: mov [4], ecx        ; Guardamos el contador en la dirección [DS+4]
    
    mov edx, ds         ; Apuntamos EDX al Data Segment
    add edx, 4          ; Le sumamos 4 para que apunte exactamente a [DS+4]
    mov ecx, 0
    ldh ecx, 4          ; Tamaño: 4 bytes
    ldl ecx, 1          ; Cantidad: 1
    mov eax, 1          ; Formato: Decimal
    sys 2               ; Llamada al sistema para IMPRIMIR

    jmp inicio
fin: stop