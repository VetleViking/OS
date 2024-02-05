#include <system.h>

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>


typedef struct {
	char name[20];
	char content[77];
} Strs;

static Strs saved_strings[10] = {0};

typedef struct {
	char name[20];
	int start_line[10];
} Funcs;

static Funcs functions[10] = {0};

// Tries to execute the line of code given
void execute_checks(char text[77], int at_line) {

	int len = strlen(text);

	if (len <= 1) {
		return;
	}

	char execute_content [77];

	if (strcmplen(text, "command ", 8) == 0) {
		for (int i = 0; i < len - 8; i++) {
			command[i] = text[i + 8];
		}
		command[len - 8] = '\0';
		check_for_command();
		is_writing_command = false;
	} else if (strcmplen(text, "print ", 6) == 0) {
		if (text[6] == '"') {
			for (int i = 0; i < len - 7; i++) {
				execute_content[i] = text[i + 7];
				if (text[i + 7] == '"') {
					execute_content[i] = '\0';
					break;
				}

				if (i == len - 7) {
					terminal_writestring("Error: no end of string found");
					newline();
					return;
				}
			}
			terminal_writestring(execute_content);
			newline();
		} else {
			for (int i = 0; i < len - 6; i++) {
				execute_content[i] = text[i + 6];
			}
			execute_content[len - 6] = '\0';

			for (int i = 0; i < 10; i++) {
				if (saved_strings[i].name != NULL && strcmp(saved_strings[i].name, execute_content) == 0) {
					terminal_writestring(saved_strings[i].content);
					newline();
					return;
				}
			}	
			terminal_writestring("Error: no string with that name found");
			newline();
			return;
		}		
	} else if (strcmplen(text, "str ", 4) == 0) {
		for (int i = 0; i < len - 4; i++) {
			execute_content[i] = text[i + 4];
		}
		execute_content[len - 4] = '\0';
		len = strlen(execute_content);

		char name[10];
		char content[77];

		for (int i = 0; i < len; i++) {
			if (execute_content[i] == ' ' && execute_content[i + 1] == '=' && execute_content[i + 2] == ' ') {
				name[i] = '\0';
				break;
			}
			if (i == len - 1) {
				terminal_writestring("Error: no = found");
				newline();
				return;
			}
			name[i] = execute_content[i];
		}

		bool startStr = false;
		int j = 0;
		for (int i = strlen(name) + 3; i < len; i++) {
			if (execute_content[i] == '"' && !startStr) {
				startStr = true;
				continue;
			} else if (execute_content[i] == '"' && startStr) {
				content[j] = '\0';
				break;
			}
			if (i == len - 1) {
				terminal_writestring("Error: no end of string found");
				newline();
				return;
			}

			content[j] = execute_content[i];
			j++;
		}

		Strs new_str;

		strcpy(new_str.name, name);
		strcpy(new_str.content, content);

		for (int i = 0; i < 10; i++) {
			if (saved_strings[i].name[0] == '\0') {
				saved_strings[i] = new_str;
				break;
			}
		}
	} else if (strcmplen(text, "loop ", 5) == 0) {
		for (int i = 0; i < len - 5; i++) {
			execute_content[i] = text[i + 5];
		}
		execute_content[len - 5] = '\0';

		int loop_amount = atoi(execute_content);
		
		for (int i = 0; i < loop_amount; i++) {
			for (int i = at_line + 1; i < 22; i++) {
				if (strcmplen(text_editor_text[i], "    ", 4) == 0) {
					char text_fixed [73];
					int len2 = strlen(text_editor_text[i]);

					for (int j = 0; j < len2 - 4; j++) {
						text_fixed[j] = text_editor_text[i][j + 4];
					}
					text_fixed[len2 - 4] = '\0';

					execute_checks(text_fixed, i);
				} else {
					break;
				}
			}
		}	
	} else if (strcmplen(text, "func ", 5)) {
		char name[10];

		for (int i = 0; i < len; i++) {
			if (execute_content[i] == '\0') {
				name[i] = '\0';
				break;
			}
			name[i] = execute_content[i];
		}

		Funcs new_func;

		strcpy(new_func.name, name);
		new_func.start_line[0] = at_line + 1;
	} else if (strcmplen(text, "call ", 5)) {
		char name[10];

		for (int i = 0; i < len; i++) {
			if (execute_content[i] == '\0') {
				name[i] = '\0';
				break;
			}
			name[i] = execute_content[i];
		}

		for (int i = 0; i < 10; i++) {
			if (functions[i].name != NULL && strcmp(functions[i].name, name) == 0) {
				for (int i = functions[i].start_line; i < 22; i++) {
					if (strcmplen(text_editor_text[i], "    ", 4) == 0) {
						char text_fixed [73];
						int len2 = strlen(text_editor_text[i]);

						for (int j = 0; j < len2 - 4; j++) {
							text_fixed[j] = text_editor_text[i][j + 4];
						}
						text_fixed[len2 - 4] = '\0';

						execute_checks(text_fixed, i);
					} else {
						break;
					}
				}
				break;
			}
		}
	}


	else if (!strcmp(text, "\n") == 0 && !strcmplen(text, "#", 1) == 0) {
		terminal_writestring("Unknown command: ");
		terminal_writestring(text);
		newline();
	}
}


// Executes the code from the text editor
void execute_text() {
	for (size_t i = 0; i < 22; i++) {
		int len = strlen(text_editor_text[i]);

		if (strcmplen(text_editor_text[i], "    ", 4) == 0) {
			continue;
		}

		execute_checks(text_editor_text[i], i);

	}
}