[org 0x7c00]
mov ah, 0x0e

cld
lea di, [data] 
jmp newLine
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
    jmp printName
endCheckName:
    lea si, [data] 
    mov ah, 0x0e
checkShutdown:
    mov al, [si]
    cmp al, 115
    jne endCheckShutdown
    inc si
    mov al, [si]
    cmp al, 104
    jne endCheckShutdown
    jmp shutdown
endCheckShutdown:
    lea si, [data] 
    mov ah, 0x0e
 
checkclearAll:
    mov al, [si]
    cmp al, 99
    jne endCheckClearAll
    inc si
    mov al, [si]
    cmp al, 108
    jne endCheckClearAll
    inc si
    mov al, [si]
    cmp al, 101
    jne endCheckClearAll
    inc si
    mov al, [si]
    cmp al, 97
    jne endCheckClearAll
    inc si
    mov al, [si]
    cmp al, 114
    jne endCheckClearAll
    jmp clearAll
endCheckClearAll:
    lea si, [data] 
    mov ah, 0x0e

checkPrint:
    mov al, [si]
    cmp al, 112
    jne endCheckPrint
    inc si
    mov al, [si]
    cmp al, 114
    jne endCheckPrint
    inc si
    mov al, [si]
    cmp al, 105
    jne endCheckPrint
    inc si
    mov al, [si]
    cmp al, 110
    jne endCheckPrint
    inc si
    mov al, [si]
    cmp al, 116
    jne endCheckPrint
    jmp print_input
endCheckPrint:
    lea si, [data] 
    mov ah, 0x0e

jmp error_message
shutdown:
    jmp $

clearAll:
    mov ah, 0x00
    mov al, 0x03
    int 0x10
    lea si, [data] 
    mov ah, 0x0e
    mov al, 10
    int 0x10 
    mov al, 13
    int 0x10
    mov al, 62
    int 0x10
    mov di, data
    jmp remove_all
printName:
    lea si, [name] 
    jmp print_loop
print_input:
    lea si, [data] 
    jmp print_find_start
print_find_start:
    lodsb 
    cmp al, 0
    je newLine
    cmp al, 32
    je print_loop
    jmp print_find_start

error_message:
    lea si, [error] 
    jmp print_error
print_error:
    lodsb 
    cmp al, 0
    je print_error_end
    int 0x10
    jmp print_error
print_error_end:
    lea si, [data]
    jmp print_loop


print_loop:
    lodsb 
    cmp al, 0
    je newLine
    cmp al, 47
    je print_loop_nl
    int 0x10
    jmp print_loop
    mov al, 10
    int 0x10
    mov al, 13
    int 0x10
print_loop_nl:
    mov al, 10
    int 0x10
    mov al, 13
    int 0x10
    jmp print_loop

newLine:
    mov al, 10
    int 0x10
    mov al, 13
    int 0x10
    mov al, 62
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

name:
    db 'OSU', 0

error:
    db 'Error: no ', 0

jmp $
times 510-($-$$) db 0
db 0x55, 0xaa