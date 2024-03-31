# This makefile compiles and runs the most recent version of the OS.
# It is run by using the command "make" in the terminal, in the root directory of the project.

# Starts the OS in QEMU.
startQemu: clean
	qemu-system-i386 -cdrom src/myos.iso

# Removes all the object and binary files the makefile created.
clean: setupGrub 
	cd src && rm -f *.o
	cd src && rm -f *.bin

# Creates the iso file that can be used to boot the OS, using GRUB.
setupGrub: combineFiles
	cd src && mkdir -p isodir/boot/grub
	cd src && cp myos.bin isodir/boot/myos.bin
	cd src && cp grub.cfg isodir/boot/grub/grub.cfg
	cd src && grub-mkrescue -o myos.iso isodir

# Combines the all the different object files into a single binary file.
# If you add something to compileKernel, you need to add the object file it creates here.
combineFiles: compileKernel
	cd src && i386-elf-gcc -T linker.ld -o myos.bin -ffreestanding -O2 -nostdlib boot.o mouse.o pong.o bga.o keyboard.o mouse_test.o chess_bot.o chess.o vga.o calculator.o kernel.o kernel_asm.o gdt.o idt.o irq.o execute_text.o text_editor.o minesweeper.o tower_defense.o game_of_life.o rock_paper_scissors.o keyboard_layouts.o tic_tac_toe.o keyboard_handler.o -lgcc

# Compiles the c and asm files to object files.
# If you add a new c file, you need to add it to the compileKernel command.
compileKernel: compileBoot
	cd src && i386-elf-gcc -c kernel.c -o kernel.o -std=gnu99 -ffreestanding -O2 -w -I include
	cd src && i386-elf-gcc -c games/minesweeper.c -o minesweeper.o -std=gnu99 -ffreestanding -O2 -w -I include
	cd src && i386-elf-gcc -c games/tower_defense.c -o tower_defense.o -std=gnu99 -ffreestanding -O2 -w -I include
	cd src && i386-elf-gcc -c games/rock_paper_scissors.c -o rock_paper_scissors.o -std=gnu99 -ffreestanding -O2 -w -I include
	cd src && i386-elf-gcc -c games/tic_tac_toe.c -o tic_tac_toe.o -std=gnu99 -ffreestanding -O2 -w -I include
	cd src && i386-elf-gcc -c games/game_of_life.c -o game_of_life.o -std=gnu99 -ffreestanding -O2 -w -I include	
	cd src && i386-elf-gcc -c games/pong.c -o pong.o -std=gnu99 -ffreestanding -O2 -w -I include	
	cd src && i386-elf-gcc -c text_editor/execute_text.c -o execute_text.o -std=gnu99 -ffreestanding -O2 -w -I include
	cd src && i386-elf-gcc -c text_editor/text_editor.c -o text_editor.o -std=gnu99 -ffreestanding -O2 -w -I include
	cd src && i386-elf-gcc -c keyboard/keyboard_handler.c -o keyboard_handler.o -std=gnu99 -ffreestanding -O2 -w -I include
	cd src && i386-elf-gcc -c keyboard/keyboard_layouts.c -o keyboard_layouts.o -std=gnu99 -ffreestanding -O2 -w -I include
	cd src && i386-elf-gcc -c keyboard.o/keyboard.o.c -o keyboard.o -std=gnu99 -ffreestanding -O2 -w -I include	
	cd src && i386-elf-gcc -c graphics/vga.c -o vga.o -std=gnu99 -ffreestanding -O2 -w -I include
	cd src && i386-elf-gcc -c graphics/bga.c -o bga.o -std=gnu99 -ffreestanding -O2 -w -I include
	cd src && i386-elf-gcc -c other/calculator.c -o calculator.o -std=gnu99 -ffreestanding -O2 -w -I include
	cd src && i386-elf-gcc -c abbreviation_stuff/gdt.c -o gdt.o -std=gnu99 -ffreestanding -O2 -w -I include
	cd src && i386-elf-gcc -c abbreviation_stuff/idt.c -o idt.o -std=gnu99 -ffreestanding -O2 -w -I include
	cd src && i386-elf-gcc -c abbreviation_stuff/irq.c -o irq.o -std=gnu99 -ffreestanding -O2 -w -I include
	cd src && i386-elf-gcc -c games/chess/chess.c -o chess.o -std=gnu99 -ffreestanding -O2 -w -I include	
	cd src && i386-elf-gcc -c games/chess/chess_bot.c -o chess_bot.o -std=gnu99 -ffreestanding -O2 -w -I include	
	cd src && i386-elf-gcc -c mouse/mouse_test.c -o mouse_test.o -std=gnu99 -ffreestanding -O2 -w -I include	
	cd src && i386-elf-gcc -c mouse/mouse.c -o mouse.o -std=gnu99 -ffreestanding -O2 -w -I include	



	cd src && nasm -f elf32 kernel_asm.asm -o kernel_asm.o

# Compiles the boot Assembly file to an object file.
compileBoot:
	cd src && i386-elf-as boot.asm -o boot.o