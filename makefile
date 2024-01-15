step6: step5
	qemu-system-x86_64 everything.bin

step5: step4
	cat "boot.bin" "full_kernel.bin" > "everything.bin"

step4: step3
	nasm "boot2.asm" -f bin -o "boot.bin"

step3: step2a
	i386-elf-ld -o "full_kernel.bin" -Ttext 0x1000 "kernel_entry.o" "kernel.o" "kernel2.o" --oformat binary

step2a: step2
	nasm "kernel.asm" -f elf -o "kernel2.o"

step2: step1
	nasm "kernel_entry.asm" -f elf -o "kernel_entry.o"

step1:
	i386-elf-gcc -ffreestanding -m32 -g -c "kernel.c" -o "kernel.o"