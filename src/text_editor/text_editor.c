#include <system.h>

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>


bool in_text_editor = false;
char text_editor_text[22][77] = {
	'\0',
	'\0',
	'\0',
	'\0',
	'\0',
	'\0',
	'\0',
	'\0',
	'\0',
	'\0',
	'\0',
	'\0',
	'\0',
	'\0',
	'\0',
	'\0',
	'\0',
	'\0',
	'\0',
	'\0',
	'\0',
	'\0',
	'\0',
};

// Opens a text editor
void text_editor() {
	check_scroll = false;
	in_text_editor = false;

	// draws the header of the text editor
	clear_screen();  
	terminal_putchar(201);
	for (size_t i = 0; i < 78; i++) {
		terminal_putchar(205);
	}
	terminal_putchar(187);
	terminal_putchar(186);
	terminal_writestring(" Text editor ------------------------- ESC to exit, arrow keys to move around ");
	terminal_putchar(186);
	terminal_putchar(200);
	terminal_putchar(205);
	terminal_putchar(203);
	for (size_t i = 0; i < 76; i++) {
		terminal_putchar(205);
	}
	terminal_putchar(188);

	// draws the text editor
	char line_number[10];

	for (size_t i = 0; i < 22; i++) {
		if (i < 9) {
			itoa(i + 1, line_number, 10);
			terminal_writestring(line_number);
			terminal_writestring(" ");
			terminal_putchar(186);
		} else {
			itoa(i + 1, line_number, 10);
			terminal_writestring(line_number);
			terminal_putchar(186);
		}

		terminal_writestring(text_editor_text[i]);

		if (i < 21) {
			terminal_row++;
			terminal_column = 0;
		}
	}
	terminal_row = 3;
	terminal_column = 3;
	move_cursor(terminal_row, terminal_column);
	
	in_text_editor = true;
}