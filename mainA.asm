\\include  "btree.asm"

root  equ    4
main:   sys 15
        push bp
        mov bp, sp
        sub sp, 4 ; root de ABB
        push eax
        push edx
        sys 15
        
        call    heap_init    ; inicializa memoria dinámica

        mov edx, bp
        sub edx, root        ; edx = &root
        mov [edx], null      ; inicializo root = null

        push 20
        call btn_new
        add sp, 4

        push eax
        push edx
        call bst_add
        add sp, 8

        push 20
        call btn_new
        add sp, 4

        push eax
        push edx
        call bst_add
        add sp, 8

        push 10
        call btn_new
        add sp, 4

        push eax
        push edx
        call bst_add
        add sp, 8

        push 15
        call btn_new
        add sp, 4

        push eax
        push edx
        call bst_add
        add sp, 8

        push 30
        call btn_new
        add sp, 4

        push eax
        push edx
        call bst_add
        add sp, 8

        push 25
        call btn_new
        add sp, 4

        push eax
        push edx
        call bst_add
        add sp, 8

        push [edx]
        call inorder
        add sp, 4

main_end:       sys 15
        sys 0
        pop edx
        pop eax
        mov sp,bp
        pop bp
        ret
