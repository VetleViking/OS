startQemu: compileEverything
	cd src && qemu-system-x86_64 everything.bin

combineEverything: compileBoot
	cd src && cat "boot.bin" "full_kernel.bin" > "everything.bin"

compileBoot: combineKernel
	cd src && nasm "boot2.asm" -f bin -o "boot.bin"

combineKernel: compileKernel
	cd src && i386-elf-ld -o "full_kernel.bin" -Ttext 0x1000 "kernel_entry.o" "kernel.o" "kernel2.o" --oformat binary

compileKernel: 
	cd src && i386-elf-gcc -ffreestanding -m32 -g -c "kernel.c" -o "kernel.o"
	cd src && nasm "kernel.asm" -f elf -o "kernel2.o"
	cd src && nasm "kernel_entry.asm" -f elf -o "kernel_entry.o"
