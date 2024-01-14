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
mov bl, 18
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

cld
lea di, [data] 
type:
    mov ah, 0
    int 0x16 
    mov [bx], al 
    cmp al, 13
    je prepare_print
    cmp al, 8
    je remove_byte
    mov ah, 0x0e
    int 0x10
    jmp store_byte 
store_byte:
    lea si, [bx]
    movsb
    jmp type

prepare_print:
    lea si, [data] 
    mov ah, 0x0e
    mov al, 10
    int 0x10 
    mov al, 13
    int 0x10

checkName:
    mov al, [si]
    cmp al, 110
    jne endCheckName
    inc si
    mov al, [si]
    cmp al, 97
    jne endCheckName
    inc si
    mov al, [si]
    cmp al, 109
    jne endCheckName
    inc si
    mov al, [si]
    cmp al, 101
    jne endCheckName
    jmp printOSU
endCheckName:
    lea si, [data] 
    mov ah, 0x0e
    
checkHelp:
    mov al, [si]
    cmp al, 104
    jne endCheckHelp
    inc si
    mov al, [si]
    cmp al, 101
    jne endCheckHelp
    inc si
    mov al, [si]
    cmp al, 108
    jne endCheckHelp
    inc si
    mov al, [si]
    cmp al, 112
    jne endCheckHelp
    jmp printHelp
endCheckHelp:
    lea si, [data] 
    mov ah, 0x0e


jmp print_loop

printHelp:
    lea si, [osDescription] 
    jmp print_loop

printOSU:
    lea si, [nameOs] 
    jmp print_loop



print_loop:
    lodsb 
    cmp al, 0
    je newLine
    int 0x10
    jmp print_loop
    mov al, 10
    int 0x10
    mov al, 13
    int 0x10

newLine:
    mov al, 10
    int 0x10
    mov al, 13
    int 0x10
    jmp remove_all

remove_byte:
    cmp di, data
    je type
    dec di 
    mov byte [di], 0
    mov ah, 0x0e
    mov al, 0x08
    int 0x10
    mov al, ' '
    int 0x10
    mov al, 0x08
    int 0x10
    jmp type

remove_all:
    cmp di, data
    je type
    dec di 
    mov byte [di], 0
    jmp remove_all

data:
    times 64 db 0

nameOs:
    db 'OSU', 0

osDescription:
    db 'En OS laget av Kasper og Vetle', 0

jmp $
times 510-($-$$) db 0
db 0x55, 0xaa