#include <system.h>

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>



bool shift_pressed = false;
bool caps_lock = false;

// Checks if the key is special (like enter, backspace, etc.)
void keyboard_handler(unsigned char c) { 
	if (c == 0) {
	} else if (c == 1){ // esc
	} else if (c == 28) { // enter
		if (!is_writing_command) {
			newline();
		} else {
			at_in_command = 0;
			
			check_for_command();
		}
	} else if (c == 15) { // tab
		if (is_writing_command) {
			int len = strlen(command);
			if (len + 4 <= MAX_COMMAND_LENGTH) {

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

				terminal_column = 2;
				terminal_row = command_start_row;

				rollover = false;

				is_writing_command = false;
				terminal_writestring(command);
				is_writing_command = true;

				terminal_column = prev_col + 4;
				at_in_command += 4;

				terminal_row = prev_row;

				if (rollover) {
					terminal_row--;
				}			
				
			}
		}
	} else if (c == 42) { // shift pressed
		if (is_writing_command) {
			shift_pressed = true;
		}
	} else if (c == 170) { // shift released
		if (is_writing_command) {
			shift_pressed = false; 
		}
	} else if (c == 58) { // caps lock
		if (is_writing_command) {
			caps_lock = !caps_lock;
		}
	} else if (c == 14) { // backspace
		if (is_writing_command) {
			int len = strlen(command);
			if (at_in_command > 0) { // if there is a command being written, delete character before cursor
				for (int i = at_in_command - 1; i < len - 1; i++) {
					command[i] = command[i + 1];
				}

				command[len - 1] = '\0';

				int prev_col = terminal_column;
				int prev_row = terminal_row;

				rollover = false;

				terminal_column = 2;
				terminal_row = command_start_row;

				is_writing_command = false;
				terminal_writestring(command);
				terminal_writestring(" ");
				is_writing_command = true;

				terminal_row = prev_row;

				if (rollover) {
					terminal_row--;
				}

				terminal_column = prev_col - 1;

				at_in_command--;
			}
		}
	} else if (c == 72) { // up arrow pressed	
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
		}	
	} else if (c == 80) { // down arrow pressed
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
			}
		}
	} else if (c == 75) { // left arrow pressed
		if (is_writing_command) {
			if (at_in_command > 0) {
				terminal_column--;
				at_in_command--;
			}
		}
	} else if (c == 77) { // right arrow pressed
		if (is_writing_command) {
			if (at_in_command < strlen(command)) {
				terminal_column++;
				at_in_command++;
			}
		}
	} else { // any other key
		return;
	}
	should_print = false; // if any of the above is true, then it should not print
}
