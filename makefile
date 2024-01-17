step6: step5
	cd src && qemu-system-x86_64 everything.bin

step5: step4
	cd src && cat "boot.bin" "full_kernel.bin" > "everything.bin"

step4: step3
	cd src && nasm "boot2.asm" -f bin -o "boot.bin"

step3: step2a
	cd src && i386-elf-ld -o "full_kernel.bin" -Ttext 0x1000 "kernel_entry.o" "kernel.o" "kernel2.o" --oformat binary

step2a: step2
	cd src && nasm "kernel.asm" -f elf -o "kernel2.o"

step2: step1
	cd src && nasm "kernel_entry.asm" -f elf -o "kernel_entry.o"

step1:
	cd src && i386-elf-gcc -ffreestanding -m32 -g -c "kernel.c" -o "kernel.o"
