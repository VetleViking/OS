#include <system.h>

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>


bool shift_pressed = false;
bool caps_lock = false;


void test_at_in_command() {
	char test[3];
	itoa(at_in_command, test, 10);
	for (int i = 0; i < 3; i++) {
		if (i > strlen(test)) {
			terminal_putentryat(' ', terminal_color, i, 24);
			continue;	
		}
		terminal_putentryat(test[i], terminal_color, i, 24);
	}
}



// Checks if the key is special (like enter, backspace, etc.)
void keyboard_handler(unsigned char c) { 
	if (c == 0) {
	} else if (c == 1){
		if (in_text_editor) {
			in_text_editor = false;	
			check_scroll = true;
			end_check_for_command();
			clear_screen();
			new_kernel_line();
		}
	} else if (c == 28) { // enter
		if (in_text_editor) { // continue here
			int len = strlen(text_editor_text[terminal_row - 3]);
			
			char new_text[22][77];

			for (int i = 0; i < terminal_row - 3; i++) {
				for (int j = 0; j < 77; j++) {
					new_text[i][j] = text_editor_text[i][j];
				}
			}

			new_text[terminal_row - 3][terminal_column - 3] = '\0';

			for (int i = terminal_column - 3; i < len; i++) {
				new_text[terminal_row - 3][i - terminal_row + 3] = text_editor_text[terminal_row - 3][i];
			}

			for (int i = terminal_row - 2; i < 22; i++) {
				for (int j = 0; j < 77; j++) {
					new_text[i][j] = text_editor_text[i - 1][j];
				}
			}

			for (int i = 0; i < 22; i++) {
				int len = strlen(new_text[i]);

				for (int j = 0; j < 77; j++) {
					text_editor_text[i][j] = new_text[i][j];
				}
			}

			int prev_row = terminal_row;

			in_text_editor = false;
			terminal_row = 3;
			terminal_column = 3;

			for (int i = 0; i < 22; i++) {
				int len = strlen(text_editor_text[i]);
				terminal_writestring(text_editor_text[i]);
				
				for (int j = len + 3; j < 80; j++) {
					terminal_putentryat(' ', terminal_color, j, terminal_row);
				}

				if (i < 21) {
					terminal_column = 3;
					terminal_row++;
				}
			}
			
			in_text_editor = true;
			terminal_column = 3;

			terminal_row = prev_row + 1 > 24 ? 24 : prev_row + 1;
			terminal_column = 3;
			
		} else if (!is_writing_command) {
			newline();
		} else {
			at_in_command = 0;
			test_at_in_command();
			
			check_for_command();
		}
	} else if (c == 15) { // tab
		if (in_text_editor) {
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
		} else if (is_writing_command) {
			int len = strlen(command);
			if (len + 4 < MAX_COMMAND_LENGTH) {

				char buffer[MAX_COMMAND_LENGTH];

				for (int i = at_in_command; i < len; i++) {
					buffer[i - at_in_command] = command[i];
				}

				for (int i = 0; i < len - at_in_command; i++) {
      				command[i + at_in_command + 4] = buffer[i];
    			}

				command[at_in_command] = ' ';
				command[at_in_command + 1] = ' ';
				command[at_in_command + 2] = ' ';
				command[at_in_command + 3] = ' ';
				command[len + 4] = '\0';

				int prev_col = terminal_column;
				int prev_row = terminal_row;

				terminal_column = 2; // fix
				terminal_row = command_start_row;

				rollover = false;

				is_writing_command = false;
				terminal_writestring(command);
				is_writing_command = true;

				terminal_column = prev_col + 4;
				at_in_command += 4;
				test_at_in_command();

				terminal_row = prev_row;

				if (rollover) {
					terminal_row--;
				}			
				
			}
		}
	} else if (c == 42) { // shift pressed
		if (is_writing_command || in_text_editor) {
			shift_pressed = true;
		}
	} else if (c == 170) { // shift released
		if (is_writing_command || in_text_editor) {
			shift_pressed = false; 
		}
	} else if (c == 58) { // caps lock
		if (is_writing_command || in_text_editor) {
			caps_lock = !caps_lock;
		}
	} else if (c == 14) { // backspace
		if (in_text_editor) {
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
			}
		} else if (is_writing_command) {
			size_t len = strlen(command);
			if (at_in_command > 0) { // if there is a command being written, delete last character
				for (int i = at_in_command - 1; i < len - 1; i++) {
					command[i] = command[i + 1];
				}

				int put_at_col = (len + 1) % 80; // fix later

				int put_at_row = command_start_row + (len + 1) / 80;
			
				terminal_putentryat(' ', terminal_color, put_at_col, put_at_row);
				command[len - 1] = '\0';

				int prev_col = terminal_column;
				int prev_row = terminal_row;

				rollover = false;

				terminal_column = 2;
				terminal_row = command_start_row;

				is_writing_command = false;
				terminal_writestring(command);
				is_writing_command = true;

				terminal_row = prev_row;

				if (rollover) {
					terminal_row--;
				}

				terminal_column = prev_col - 1;

				at_in_command--;
				test_at_in_command();
			}
		}
	} else if (c == 72) { // up arrow pressed
	 	if (in_text_editor) {
			if (terminal_row > 3) {
				terminal_row--;
				terminal_column = 3;
			}
		} else {	
			if (at_command > 0) { // if there is commands above
				at_command--;
				size_t len = strlen(command);
				for (size_t i = 0; i < len; i++) { // delete current command
					terminal_column--;
					terminal_putentryat(' ', terminal_color, terminal_column, terminal_row);
				}
				command[0] = '\0';
				is_writing_command = false;
				
				len = strlen(previous_commands[at_command]);
				for (size_t i = 0; i < len; i++) {
					command[i] = previous_commands[at_command][i];
				}
				command[len] = '\0';

				terminal_writestring(previous_commands[at_command]); // write command
				is_writing_command = true;

				at_in_command = strlen(command);
				test_at_in_command();
			}
		}
	} else if (c == 80) { // down arrow pressed
		if (in_text_editor) {
			if (terminal_row < 24) {
				terminal_row++;
				terminal_column = 3;
			}
		} else {
			if (at_command < num_commands) { // if there is commands below
				at_command++;
				size_t len = strlen(command);
				for (size_t i = 0; i < len; i++) { // delete current command
					terminal_column--;
					terminal_putentryat(' ', terminal_color, terminal_column, terminal_row);
				}
				command[0] = '\0';
					
				if (!(at_command == num_commands)) { 

					len = strlen(previous_commands[at_command]);
					for (size_t i = 0; i < len; i++) {
						command[i] = previous_commands[at_command][i];
					}
					command[len] = '\0';

					is_writing_command = false;
					terminal_writestring(previous_commands[at_command]); // if not last and empty command, write command
					is_writing_command = true;

					at_in_command = strlen(command);
					test_at_in_command();
				}
			}
		}
	} else if (c == 75) { // left arrow pressed
		if (in_text_editor) {
			if (terminal_column > 3) {
				terminal_column--;
			}
		} else if (is_writing_command) {
			if (at_in_command > 0) {
				terminal_column--;
				at_in_command--;
				test_at_in_command();
			}
		}
	} else if (c == 77) { // right arrow pressed
		if (in_text_editor) {
			if (terminal_column < 77 && terminal_column < strlen(text_editor_text[terminal_row - 3]) + 3) {
				terminal_column++;
			}
		} else if (is_writing_command) {
			if (at_in_command < strlen(command)) {
				terminal_column++;
				at_in_command++;
				test_at_in_command();
			}
		}
	}
	
	
	else { // any other key
		should_print = true;
		return;
	}
	should_print = false; // if any of the above is true, then it should not print
	return;
}
