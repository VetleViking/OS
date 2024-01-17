global loop

loop:
    mov edi, 0xb8000
    mov al, 'O'
    mov ah, 0x0f
    mov [edi], ax
    add edi, 2
    mov al, 'S'
    mov [edi], ax
    add edi, 2
    mov al, 'U'
    mov [edi], ax
    add edi, 2
    ret