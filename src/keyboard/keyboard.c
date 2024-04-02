#include <system.h>

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

typedef struct {
    void (*func)(unsigned char c);
} keyboard_handler_t;

#define MAX_KEYBOARD_HANDLERS 10
keyboard_handler_t keyboard_handlers[MAX_KEYBOARD_HANDLERS];

void init_keyboard_handlers() {
    for (int i = 0; i < MAX_KEYBOARD_HANDLERS; i++) {
        keyboard_handlers[i].func = NULL;
    }
}

void add_keyboard_handler(void (*func)(unsigned char c)) {
	for (int i = 0; i < MAX_KEYBOARD_HANDLERS; i++) {
		if (keyboard_handlers[i].func == NULL) {
			keyboard_handlers[i].func = func;
			return;
		}
	}
}

void remove_keyboard_handler(void (*func)(unsigned char c)) {
	for (int i = 0; i < MAX_KEYBOARD_HANDLERS; i++) {
		if (keyboard_handlers[i].func == func) {
			keyboard_handlers[i].func = NULL;
			return;
		}
	}
}

void keyboard_interrupt_handler(struct regs *r) {
	asm volatile ("sti");
	
    unsigned char c;
	c = inb(0x60);

	for (int i = 0; i < MAX_KEYBOARD_HANDLERS; i++) {
		if (keyboard_handlers[i].func != NULL) {
			keyboard_handlers[i].func(c);
		}
	}

	if (is_writing_command || in_text_editor) {	
		terminal_putchar(kbd_special_characters[c]);
		move_cursor(terminal_column, terminal_row);
	}
	
}

void keyboard_install()
{
	irq_install_handler(1, keyboard_interrupt_handler);
	init_keyboard_handlers();
}