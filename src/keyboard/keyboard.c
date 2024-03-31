#include <system.h>

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>



void keyboard_interrupt_handler(struct regs *r) {
	asm volatile ("sti");
	
    unsigned char c;
	c = inb(0x60);

	if (in_TD) { // no, this is not jank, this is unique and sofisticated
		td_keyboard_handler(kbd_special_characters[c]);
		return;
	} 
	
	if (in_mine_sweeper) {
		ms_keyboard_handler(kbd_special_characters[c]);
		return;
	}

	if (in_game_of_life) {
		gol_keyboard_handler(kbd_special_characters[c]);
		return;
	}

	if (in_pong) {
		pong_keyboard_handler(kbd_special_characters[c]);
		return;
	}

	if (in_chess) {
		chess_keyboard_handler(kbd_special_characters[c]);
		return;
	}

	if (is_writing_command || in_text_editor) {	
		terminal_putchar(kbd_special_characters[c]);
		move_cursor(terminal_column, terminal_row);
	}
	
}

void keyboard_install()
{
	irq_install_handler(1, keyboard_interrupt_handler);
}