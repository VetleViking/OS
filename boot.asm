[org 0x7c00]
mov ah, 0x0e


mov bl, 40
mov al, 61
loop:
    int 0x10
    dec bl
    cmp bl, 0
    jne loop
mov al, 10
int 0x10
mov al, 13
int 0x10

mov al, 32
mov bl, 9
loop1:
    int 0x10
    dec bl
    cmp bl, 0
    jne loop1

mov bx, nameOs
printOSName:
    mov al, [bx]
    cmp al, 0
    je printOSNameStop
    int 0x10
    inc bx
    jmp printOSName
printOSNameStop:
    mov al, 10
    int 0x10
    mov al, 13
    int 0x10


mov al, 32
mov bl, 5
loop3:
    int 0x10
    dec bl
    cmp bl, 0
    jne loop3


mov bx, osDescription
printOsDesc:
    mov al, [bx]
    cmp al, 0
    je osDescStop
    int 0x10
    inc bx
    jmp printOsDesc
osDescStop:
    mov al, 10
    int 0x10
    mov al, 13
    int 0x10




mov bl, 40
mov al, 61
loop2:
    int 0x10
    dec bl
    cmp bl, 0
    jne loop2
mov al, 10
int 0x10
mov al, 13
int 0x10


nameOs:
    db 'OSU', 0

osDescription:
    db 'En OS laget av Kasper og Vetle', 0

jmp $
times 510-($-$$) db 0
db 0x55, 0xaa