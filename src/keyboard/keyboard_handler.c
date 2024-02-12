#include <system.h>

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>


bool shift_pressed = false;
bool caps_lock = false;

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
		if (in_text_editor) {
			// TODO: make lower lines move down and delete last line, or something like that
		} else if (!is_writing_command) {
			newline();
		} else {
			check_for_command();
		}
	} else if (c == 15) { // tab
		if (in_text_editor) { // TODO: make it so that it moves the text after the tab, and make it not on end of text if in middle
			int len = strlen(text_editor_text[terminal_row - 3]);
			if (terminal_column + 4 < 80) { 
				terminal_column += 4;
				text_editor_text[terminal_row - 3][len] = ' ';
				text_editor_text[terminal_row - 3][len + 1] = ' ';
				text_editor_text[terminal_row - 3][len + 2] = ' ';
				text_editor_text[terminal_row - 3][len + 3] = ' ';
				text_editor_text[terminal_row - 3][len + 4] = '\0';
			}
		} else if (is_writing_command) { // TODO: the same as above
			int len = strlen(command);
			if (len + 4 < MAX_COMMAND_LENGTH) {
				terminal_column += 4;
				command[len] = ' ';
				command[len + 1] = ' ';
				command[len + 2] = ' ';
				command[len + 3] = ' ';
				command[len + 4] = '\0';
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
			if (len > 0 && terminal_column > 2) { // if there is a command being written, delete last character
				for (int i = terminal_column - 3; i < len - 1; i++) {
					command[i] = command[i + 1];
				}
			
				terminal_putentryat(' ', terminal_color, len + 1, terminal_row);
				command[len - 1] = '\0';

				int prev_col = terminal_column;
				terminal_column = 2;
				is_writing_command = false;
				terminal_writestring(command);
				is_writing_command = true;

				terminal_column = prev_col - (prev_col > 2 ? 1 : 0);
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
				}
			}
		}
	} else if (c == 75) { // left arrow pressed
		if (in_text_editor) {
			if (terminal_column > 3) {
				terminal_column--;
			}
		} else if (is_writing_command) {
			if (terminal_column > 2) {
				terminal_column--;
			}
		}
	} else if (c == 77) { // right arrow pressed
		if (in_text_editor) {
			if (terminal_column < 77 && terminal_column < strlen(text_editor_text[terminal_row - 3]) + 3) {
				terminal_column++;
			}
		} else if (is_writing_command) {
			if (terminal_column < strlen(command) + 2) {
				terminal_column++;
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
