[org 0x7c00]                        
      
mov [BOOT_DISK], dl                 

CODE_SEG equ GDT_code - GDT_start
DATA_SEG equ GDT_data - GDT_start

cli
lgdt [GDT_descriptor]
mov eax, cr0
or eax, 1
mov cr0, eax
jmp CODE_SEG:start_protected_mode

jmp $
                                    
                                 
GDT_start:                         
    GDT_null:
        dd 0x0
        dd 0x0

    GDT_code:
        dw 0xffff
        dw 0x0
        db 0x0
        db 0b10011010
        db 0b11001111
        db 0x0

    GDT_data:
        dw 0xffff
        dw 0x0
        db 0x0
        db 0b10010010
        db 0b11001111
        db 0x0

GDT_end:

GDT_descriptor:
    dw GDT_end - GDT_start - 1
    dd GDT_start


[bits 32]
start_protected_mode:


    mov edi, 0xb8000
    mov dx, 0
    loop:
        mov al, ' '
        mov ah, 0xdf
        mov [edi], ax
        add edi, 2
        inc dx
        cmp dx, 998
        jne loop

    mov al, 'O'
    mov ah, 0xdf
    mov [edi], ax
    add edi, 2
    mov al, 'S'
    mov ah, 0xdf
    mov [edi], ax
    add edi, 2
    mov al, 'U'
    mov ah, 0xdf
    mov [edi], ax
    add edi, 2

    mov dx, 0
    loop2:
        mov al, ' '
        mov ah, 0xdf
        mov [edi], ax
        add edi, 2
        inc dx
        cmp dx, 999
        jne loop2

    jmp $



    

BOOT_DISK: db 0                                     
 
times 510-($-$$) db 0              
dw 0xaa55