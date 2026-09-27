    mov eax, 6
    div eax, 7          ; Resultado 0 -> Z=1
    jz prueba_jn
    stop
prueba_jn: mov eax, 0
    sub eax, 1          ; Resultado -1 -> N=1
    jn prueba_jc
    stop
prueba_jc: mov eax, 0
    ldh eax, 0xFFFF
    ldl eax, 0xFFFD     ; EAX = -3
    sub eax, 5          ; -> C=1
    jc prueba_jv
    stop
prueba_jv: mov eax, 0
    ldh eax, 0x7FFF
    ldl eax, 0xFFFF     ; EAX = 0x7FFFFFFF
    mov ebx, eax
    add eax, ebx        ; -> V=1
    jv prueba_jp
    stop
prueba_jp: mov eax, 5          ; Resultado 5 -> N=0, Z=0
    jp prueba_jnn
    stop
prueba_jnn: mov eax, 0          ; Resultado 0 -> N=0 (y Z=1)
    jnn prueba_jnz
    stop
prueba_jnz: mov eax, 0
    sub eax, 5          ; Resultado -5 -> Z=0 (y N=1)
    jnz prueba_jnp
    stop
prueba_jnp: mov eax, 0          ; Resultado 0 -> Z=1
    jnp exito
    stop
exito: mov [0], 111
    mov edx, ds
    mov ecx, 0
    ldh ecx, 4
    ldl ecx, 1
    mov eax, 1
    sys 2
    stop