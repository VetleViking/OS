section .text
    [bits 32]
    [extern main]
    call main
    [extern test]
    call test
    [extern loop]
    call loop
    
    jmp $