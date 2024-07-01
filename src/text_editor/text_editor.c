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

void print_text_editor() {
	in_text_editor = false;
	
	int term_row_temp = terminal_row;
	int term_col_temp = terminal_column;
	
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

		if (i < 21) {
			terminal_row++;
			terminal_column = 0;
		}
	}
	for (size_t i = 0; i < 22; i++) {
		terminal_row = i + 3;
		terminal_column = 3;
		terminal_writestring(text_editor_text[i]);
	}

	terminal_row = term_row_temp;
	terminal_column = term_col_temp;

	move_cursor(terminal_row, terminal_column);

	in_text_editor = true;
}

void text_editor_keyboard_handler(unsigned char c) { 
	if (c == 0) {
	} else if (c == 1){
		in_text_editor = false;	
		check_scroll = true;
		end_check_for_command();
		clear_screen();
		new_kernel_line();
	} else if (c == 28) { // enter
		if (terminal_row == 24) {
			should_print = false;
			return;
		}

		int at = terminal_column - 3;
		int row = terminal_row - 3;

		for (int i = 22; i > row + 1; i--) {
			for (int j = 0; j < 77; j++) {
				text_editor_text[i][j] = text_editor_text[i - 1][j];
			}
		}
		
		for (int i = at; i < 77; i++) {
			text_editor_text[row + 1][i - at] = text_editor_text[row][i];
			text_editor_text[row][i] = '\0';
		}

		print_text_editor();

		terminal_row++;
		terminal_column = 3;
	} else if (c == 15) { // tab
		int len = strlen(text_editor_text[terminal_row - 3]);

		if (terminal_column + 4 < 80) { 
			char buffer[80];
			
			for (int i = terminal_column - 3; i < len; i++) {
				buffer[i - (terminal_column - 3)] = text_editor_text[terminal_row - 3][i];
			}

			for (int i = terminal_column - 3; i < len; i++) {
				text_editor_text[terminal_row - 3][i + 4] = buffer[i - (terminal_column - 3)];
			}

			text_editor_text[terminal_row - 3][terminal_column - 3] = ' ';
			text_editor_text[terminal_row - 3][(terminal_column - 3) + 1] = ' ';
			text_editor_text[terminal_row - 3][(terminal_column - 3) + 2] = ' ';
			text_editor_text[terminal_row - 3][(terminal_column - 3) + 3] = ' ';
			text_editor_text[terminal_row - 3][len + 4] = '\0';

			int prev_col = terminal_column;
			
			terminal_column = 3;

			in_text_editor = false;
			terminal_writestring(text_editor_text[terminal_row - 3]);
			in_text_editor = true;

			terminal_column = prev_col + 4;
		
		}
	} else if (c == 42) { // shift pressed
		shift_pressed = true;
	} else if (c == 170) { // shift released
		shift_pressed = false; 
	} else if (c == 58) { // caps lock
		caps_lock = !caps_lock;
	} else if (c == 14) { // backspace
		size_t len = strlen(text_editor_text[terminal_row - 3]);
		if (len > 0 && terminal_column > 3) {
			for (int i = terminal_column - 4; i < len - 1; i++) {
				text_editor_text[terminal_row - 3][i] = text_editor_text[terminal_row - 3][i + 1];
			}
		
			terminal_putentryat(' ', terminal_color, len + 2, terminal_row);
			text_editor_text[terminal_row - 3][len - 1] = '\0';

			int prev_col = terminal_column;
			terminal_column = 3;
			in_text_editor = false;
			terminal_writestring(text_editor_text[terminal_row - 3]);
			in_text_editor = true;

			terminal_column = prev_col - (prev_col > 3 ? 1 : 0);
		} else if (len == 0 && terminal_row > 3) {
			int len2 = strlen(text_editor_text[terminal_row - 4]);

			for (int i = terminal_row - 3; i > 21; i++) {
				for (int j = 0; j < 77; j++) {
					text_editor_text[i][j] = text_editor_text[i + 1][j];
				}
			}

			for (int i = 0; i < 77; i++) {
				text_editor_text[22][i] = '\0';
			}

			print_text_editor();

			terminal_row--;
			terminal_column = len2 + 3;
			move_cursor(terminal_row, terminal_column);
		}
	} else if (c == 72) { // up arrow pressed
		if (terminal_row > 3) {
			terminal_row--;
			terminal_column = 3;
		}
	} else if (c == 80) { // down arrow pressed
		if (terminal_row < 24) {
			terminal_row++;
			terminal_column = 3;
		}
	} else if (c == 75) { // left arrow pressed
		if (terminal_column > 3) {
			terminal_column--;
		}
	} else if (c == 77) { // right arrow pressed
		if (terminal_column < 77 && terminal_column < strlen(text_editor_text[terminal_row - 3]) + 3) {
			terminal_column++;
		}
	}
	
	
	else { // any other key
		should_print = true;
		return;
	}
	should_print = false; // if any of the above is true, then it should not print
}


// Opens a text editor
void text_editor() {
	check_scroll = false;
	in_text_editor = false;

	// draws the header of the text editor
	clear_screen();  

	print_text_editor();

	terminal_row = 3;
	terminal_column = 3;
	move_cursor(terminal_row, terminal_column);
	
	in_text_editor = true;
}