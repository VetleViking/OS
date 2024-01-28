startQemu: setupGrub
	qemu-system-i386 -cdrom src/myos.iso

setupGrub: combineFiles
	cd src && mkdir -p isodir/boot/grub
	cd src && cp myos.bin isodir/boot/myos.bin
	cd src && cp grub.cfg isodir/boot/grub/grub.cfg
	cd src && grub-mkrescue -o myos.iso isodir

combineFiles: compileKernel
	cd src && i386-elf-gcc -T linker.ld -o myos.bin -ffreestanding -nostdlib boot.o kernel.o test.o -lgcc

compileKernel: compileBoot
	cd src && i386-elf-gcc -c kernel.c -o kernel.o -std=gnu99 -ffreestanding -Wall -Wextra
	cd src && nasm -f elf32 test.asm -o test.o

compileBoot:
	cd src && i386-elf-as boot.asm -o boot.o