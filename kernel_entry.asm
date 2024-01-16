section .text
    [bits 32]
    [extern main]
    call main
    [extern loop]
    call loop
    
    jmp $