startQemu: setupGrub
	qemu-system-i386 -cdrom src/myos.iso

setupGrub: combineFiles
	cd src && mkdir -p isodir/boot/grub
	cd src && cp myos.bin isodir/boot/myos.bin
	cd src && cp grub.cfg isodir/boot/grub/grub.cfg
	cd src && grub-mkrescue -o myos.iso isodir

combineFiles: compileKernel
	cd src && i386-elf-gcc -T linker.ld -o myos.bin -ffreestanding -O2 -nostdlib boot.o kernel.o test.o minesweeper.o tower_defense.o rock_paper_scissors.o tic_tac_toe.o -lgcc

compileKernel: compileBoot
	cd src && i386-elf-gcc -c kernel.c -o kernel.o -std=gnu99 -ffreestanding -O2 -Wall -Wextra -I include
	cd src && i386-elf-gcc -c games/minesweeper.c -o minesweeper.o -std=gnu99 -ffreestanding -O2 -Wall -Wextra -I include
	cd src && i386-elf-gcc -c games/tower_defense.c -o tower_defense.o -std=gnu99 -ffreestanding -O2 -Wall -Wextra -I include
	cd src && i386-elf-gcc -c games/rock_paper_scissors.c -o rock_paper_scissors.o -std=gnu99 -ffreestanding -O2 -Wall -Wextra -I include
	cd src && i386-elf-gcc -c games/tic_tac_toe.c -o tic_tac_toe.o -std=gnu99 -ffreestanding -O2 -Wall -Wextra -I include
	cd src && nasm -f elf32 test.asm -o test.o

compileBoot:
	cd src && i386-elf-as boot.asm -o boot.o