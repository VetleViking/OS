startQemu: setupGrub
	qemu-system-i386 -cdrom test/myos.iso

setupGrub: combineFiles
	cd test && mkdir -p isodir/boot/grub
	cd test && cp myos.bin isodir/boot/myos.bin
	cd test && cp grub.cfg isodir/boot/grub/grub.cfg
	cd test && grub-mkrescue -o myos.iso isodir

combineFiles: compileKernel
	cd test && i386-elf-gcc -T linker.ld -o myos.bin -ffreestanding -O2 -nostdlib boot.o kernel.o -lgcc

compileKernel: compileBoot
	cd test && i386-elf-gcc -c kernel.c -o kernel.o -std=gnu99 -ffreestanding -O2 -Wall -Wextra

compileBoot:
	cd test && i386-elf-as boot.asm -o boot.o