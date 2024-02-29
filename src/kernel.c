#include <system.h>

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#define INT_DISABLE 0
#define INT_ENABLE  0x200
#define PIC1 0x20
#define PIC2 0xA0

#define ICW1 0x11
#define ICW4 0x01


int timer_ticks = 0;


int chosen_color = 13;
int chosen_bg_color = 0;
 
inline uint8_t vga_entry_color(enum vga_color fg, enum vga_color bg) {
	return fg | bg << 4;
}
 
inline uint16_t vga_entry(unsigned char uc, uint8_t color) {
	return (uint16_t) uc | (uint16_t) color << 8;
}

size_t strlen(const char* str) {
	size_t len = 0;
	while (str[len])
		len++;
	return len;
}
 
const size_t VGA_WIDTH = 80;
const size_t VGA_HEIGHT = 25;
 
size_t terminal_row;
size_t terminal_column;
uint8_t terminal_color;
uint16_t* terminal_buffer;
 

// called at the start of the kernel
void terminal_initialize(void) {
	terminal_row = 0;
	terminal_column = 0;
	terminal_color = vga_entry_color(VGA_COLOR_LIGHT_MAGENTA, VGA_COLOR_BLACK);
	terminal_buffer = (uint16_t*) 0xB8000;
	for (size_t y = 0; y < VGA_HEIGHT; y++) {
		for (size_t x = 0; x < VGA_WIDTH; x++) {
			const size_t index = y * VGA_WIDTH + x;
			terminal_buffer[index] = vga_entry(' ', terminal_color);
		}
	}
}
 

// Sets the color of the kernel
void terminal_setcolor(uint8_t color) {
	terminal_color = color;
}
 

bool rainbow = false;
bool bg_rainbow = false;
int rainbow_additional_rows = 0;

// Puts a character at a specific location in the kernel
void terminal_putentryat(unsigned char c, uint8_t color, size_t x, size_t y) {
	if (rainbow || bg_rainbow) {
		int rainbow_color = rainbow ? (x + y + rainbow_additional_rows) % 14 + 1 : chosen_color;
		int bg_rainbow_color = bg_rainbow ? (x + 2 + y + rainbow_additional_rows) % 14 + 1 : chosen_bg_color;
		color = vga_entry_color(rainbow_color, bg_rainbow_color);
	} 

	const size_t index = y * VGA_WIDTH + x;
	terminal_buffer[index] = vga_entry(c, color);
}


// Copies memory from src to dst, used mainly for scrolling
void my_memmove(void* dst, const void* src, size_t count) {
    char* dst_byte = (char*)dst;
    const char* src_byte = (const char*)src;

    if (dst_byte > src_byte && dst_byte < src_byte + count) {
        dst_byte += count;
        src_byte += count;
        while (count--) {
            *--dst_byte = *--src_byte;
        }
    } else {
        while (count--) {
            *dst_byte++ = *src_byte++;
        }
    }
}

bool rollover = false; // make this false if you want to check for rollover, used in keyboard handlker for commands

// Checks if the kernel should scroll and scrolls if needed
void check_kernel_scroll() {
	if (terminal_row == VGA_HEIGHT - 1) {
		my_memmove(terminal_buffer, terminal_buffer + VGA_WIDTH, VGA_WIDTH * (VGA_HEIGHT - 1) * 2);
		for (size_t x = 0; x < VGA_WIDTH; x++) {
			const size_t index = (VGA_HEIGHT - 1) * VGA_WIDTH + x;
			terminal_buffer[index] = vga_entry(' ', terminal_color);
			
			if (rainbow || bg_rainbow) {
				int rainbow_color = rainbow ? (x + VGA_HEIGHT + rainbow_additional_rows) % 14 + 1 : chosen_color;
				int bg_rainbow_color = bg_rainbow ? (x + 2 + VGA_HEIGHT + rainbow_additional_rows) % 14 + 1 : chosen_bg_color;
				terminal_buffer[index] = vga_entry(terminal_buffer[index], vga_entry_color(rainbow_color, bg_rainbow_color));
			}
		}
		terminal_row--;
		rainbow_additional_rows++;

		command_start_row--;

		rollover = true;
		
	}
}


// Creates a new line in the kernel
void newline() {
	terminal_row++;
	terminal_column = 1;

	check_kernel_scroll();
}

bool should_print = true;
bool is_writing_command = false;
int command_start_row = 0;

// calls newline and prints "> " to indicate that the user can write a command
void new_kernel_line() {
	newline();
	terminal_column = 0;
	command_start_row = terminal_row;
	is_writing_command = false;
	terminal_writestring("> ");
	is_writing_command = true;
}


// Compares two strings, used to check if the command is valid
int strcmp(const char *str1, const char *str2) {
    while (*str1 && (*str1 == *str2)) {
        str1++;
        str2++;
    }
    return *(unsigned char *)str1 - *(unsigned char *)str2;
}


// Compares two strings, using n of the first string
int strcmplen(const char *str1, const char *str2, size_t n) {
    while (n-- && *str1 && (*str1 == *str2)) {
        str1++;
        str2++;
    }
    if (n == (size_t)-1) {
        return 0;
    } else {
        return *(unsigned char *)str1 - *(unsigned char *)str2;
    }
}


// Copies a string from src to dest
void strcpy(char* dest, const char* src) {
    int i = 0;
    while (src[i] != '\0') {
        dest[i] = src[i];
        i++;
    }
    dest[i] = '\0';
}


// Converts an int to a string
void itoa(int value, char* str, int base) {
    char* ptr = str, *ptr1 = str, tmp_char;
    int tmp_value;

    do {
        tmp_value = value;
        value /= base;
        *ptr++ = "0123456789abcdef"[tmp_value % base];
    } while (value);

    // Apply negative sign
    if (tmp_value < 0) *ptr++ = '-';
    *ptr-- = '\0';
    while(ptr1 < ptr) {
        tmp_char = *ptr;
        *ptr--= *ptr1;
        *ptr1++ = tmp_char;
    }
}


// Converts a string to an int
int atoi(const char *str) {
    int result = 0;
    int sign = 1;
  
    // Check for whitespace
    while (*str == ' ') {
        str++;
    }

    // Check for sign
    if (*str == '-' || *str == '+') {
        if (*str == '-') sign = -1;
        str++;
    }

    // Convert each digit to int and add it to the result
    while (*str >= '0' && *str <= '9') {
        result = result * 10 + (*str - '0');
        str++;
    }

    return sign * result;
}


void clear_screen() {
	for (size_t y = 0; y < VGA_HEIGHT; y++) {
		for (size_t x = 0; x < VGA_WIDTH; x++) {
			const size_t index = y * VGA_WIDTH + x;
			int rainbow_color = rainbow ? (x + y + rainbow_additional_rows) % 14 + 1 : chosen_color;
			int bg_rainbow_color = bg_rainbow ? (x + 2 + y + rainbow_additional_rows) % 14 + 1 : chosen_bg_color;
			terminal_buffer[index] = vga_entry(' ', vga_entry_color(rainbow_color, bg_rainbow_color));
		}
	}
	terminal_row = 0;
	terminal_column = 0;
}


// Command and game stuff
char previous_commands[MAX_COMMANDS][MAX_COMMAND_LENGTH];
int num_commands = 0;
int at_command = 0;
int at_in_command = 0;
char command[MAX_COMMAND_LENGTH];
bool in_game = false;
char game[MAX_COMMAND_LENGTH];
char game_params[20];
bool has_params = false;
int game_round = 1;


int times_rand_called = 0;
int rand(int something) {
	times_rand_called++;
	int seed = something + timer_ticks + num_commands + at_command + game_round + terminal_row + terminal_column + times_rand_called + 1;

	const int A = 1103515245;
    const int C = 12345;
    const int M = 2147483648; // 2^31

    // Calculate the next random number
    int rand = (A * seed + C) % M;

    // Normalize the random number to the range [0, 100]
    rand = rand % 1001;

	return rand;
}


bool check_scroll = true;


// Handles the game command, and calls the correct game
void game_handler() {
	in_game = true;
	
	has_params = false;

	if (game_round == 1) {
		if (strlen(command) > 5) {
			for (int i = 0; i < strlen(command) - 5; i++) {
				if (command[i + 5] == ' ' && command[i + 6] == '-') {
					game[i] = '\0';
					has_params = true;
					break;
				}

				game[i] = command[i + 5];	
			}
		} else {
			terminal_writestring("please specify a game");
			in_game = false;
			return;
		}
		game[strlen(command) - 5] = '\0';
	}

	if (has_params) {
		for (int i = 0; i < strlen(command) - strlen(game) - 5; i++) {
			game_params[i] = command[i + strlen(game) + 5];
		}
		game_params[strlen(command) - strlen(game) - 5] = '\0';
	}
	
	
	if (strcmp(game, "rock paper scissors") == 0 || strcmp(game, "RPS") == 0) {
		rock_paper_scissors();
	} else if (strcmp(game, "tic tac toe") == 0 || strcmp(game, "TTT") == 0) {
		tic_tac_toe();
	} else if (strcmp(game, "tower defense") == 0 || strcmp(game, "TD") == 0) {
		tower_defense_start();
	} else if (strcmp(game, "minesweeper") == 0 || strcmp(game, "MS") == 0) {
		mine_sweeper_start();
	} else if (strcmp(game, "game of life") == 0 || strcmp(game, "GOL") == 0) {
		game_of_life_start();
	} else if (strcmp(game, "pong") == 0) {
		pong_start();
	}


	else {
		terminal_writestring("Unknown game: ");
		terminal_writestring(game);
		newline();
		terminal_writestring("Type 'gamelist' to see all games");
		in_game = false;
	}
}

// Command for changing the colors of the kernel
void color_command() {
	int at_length = 6;
	int len = strlen(command);

	char extra_parameter[10];
	char extra_parameter2[10];

	for (int i = 0; i < 10; i++) { // gets the first extra parameter
		if (command[at_length] == ' ') {
			at_length++;
			extra_parameter[i] = '\0';
			
			for (int j = 0; j < 10; j++) { // gets the second extra parameter
				if (command[at_length] == ' ') {
					at_length++;
					extra_parameter2[j] = '\0';
					break;
				}
				extra_parameter2[j] = command[at_length];
				at_length++;
			}
			break;
		}
		extra_parameter[i] = command[at_length];
		at_length++;
	} 
	int extra_parameter_length = strlen(extra_parameter);

	bool all = false;
	bool bg = false;

	if (strcmp(extra_parameter, "reset") == 0) {
		chosen_color = 13;
		chosen_bg_color = 0;
		all = true;
		change_color_done(chosen_color, all, bg);
		terminal_writestring("Color reset");
		return;
	}

	if (strcmp(extra_parameter, "rainbow") == 0 || strcmp(extra_parameter2, "rainbow") == 0) {
		if (strcmp(extra_parameter, "bg") == 0 || strcmp(extra_parameter2, "bg") == 0) {
			bg_rainbow = true;
			terminal_writestring("Background rainbow mode enabled");		
		} else {
			rainbow = true;
			terminal_writestring("Rainbow mode enabled");
		}
		for (size_t y = 0; y < VGA_HEIGHT; y++) {
			for (size_t x = 0; x < VGA_WIDTH; x++) {
				int rainbow_color = rainbow ? (x + y + rainbow_additional_rows) % 14 + 1 : chosen_color;
				int bg_rainbow_color = bg_rainbow ? (x + 2 + y + rainbow_additional_rows) % 14 + 1 : chosen_bg_color;
				terminal_buffer[y * VGA_WIDTH + x] = vga_entry(terminal_buffer[y * VGA_WIDTH + x], vga_entry_color(rainbow_color, bg_rainbow_color));
			}
		}
		return;
	}

	if (strcmp(extra_parameter, "all") == 0 || strcmp(extra_parameter2, "all") == 0) {
		all = true;
	}

	if (strcmp(extra_parameter, "bg") == 0 || strcmp(extra_parameter2, "bg") == 0) {
		bg = true;
	}

	if ((all && !bg) || (!all && bg)) {
		at_length = 6 + extra_parameter_length + 1;
	} else if (!all && !bg) {
		at_length = 6;
	}

	char change_color[20];
	
	for (int i = 0; i < 20; i++) { // gets the color to change to
		if (command[at_length] == '\0') {
			change_color[i] = '\0';
			break;
		}
		change_color[i] = command[at_length];
		at_length++;
	}

	typedef struct {
		const char* name;
		int vga_color;
	} Color;

	static const Color color_table[] = {
		{"black", VGA_COLOR_BLACK},
		{"blue", VGA_COLOR_BLUE},
		{"green", VGA_COLOR_GREEN},
		{"cyan", VGA_COLOR_CYAN},
		{"red", VGA_COLOR_RED},
		{"magenta", VGA_COLOR_MAGENTA},
		{"brown", VGA_COLOR_BROWN},
		{"light grey", VGA_COLOR_LIGHT_GREY},
		{"dark grey", VGA_COLOR_DARK_GREY},
		{"light blue", VGA_COLOR_LIGHT_BLUE},
		{"light green", VGA_COLOR_LIGHT_GREEN},
		{"light cyan", VGA_COLOR_LIGHT_CYAN},
		{"light red", VGA_COLOR_LIGHT_RED},
		{"light magenta", VGA_COLOR_LIGHT_MAGENTA},
		{"light brown", VGA_COLOR_LIGHT_BROWN},
		{"white", VGA_COLOR_WHITE},
	};

	int chosen_new_color = 20;
	size_t num_colors = sizeof(color_table) / sizeof(Color);

	for (size_t i = 0; i < num_colors; i++) { 
		if (strcmp(change_color, color_table[i].name) == 0) {
			chosen_new_color = color_table[i].vga_color;
			break;
		}
	}

	if (chosen_new_color == 20) {
		terminal_writestring("Unknown color: ");
		terminal_writestring(change_color);
		return;
	}

	change_color_done(chosen_new_color, all, bg);

	terminal_writestring("Color changed :)");
}

void change_color_done(int color, bool all, bool bg) {
	if (bg) {
		bg_rainbow = false;
	} else {
		rainbow = false;
	}

	if (bg) {
		chosen_bg_color = color;
	} else {
		chosen_color = color;
	}

	terminal_color = vga_entry_color(chosen_color, chosen_bg_color);

	if (all) {
		for (size_t y = 0; y < VGA_HEIGHT; y++) {
			for (size_t x = 0; x < VGA_WIDTH; x++) {
				const size_t index = y * VGA_WIDTH + x;
				int rainbow_color = rainbow ? (x + y + rainbow_additional_rows) % 14 + 1 : chosen_color;
				int bg_rainbow_color = bg_rainbow ? (x + 2 + y + rainbow_additional_rows) % 14 + 1 : chosen_bg_color;
				terminal_buffer[index] = vga_entry(terminal_buffer[index], vga_entry_color(rainbow_color, bg_rainbow_color));
			}
		}
	}
}


void print_ticks_to_time(int ticks) {
	int seconds = ticks / 100;
	int minutes = seconds / 60;
	int hours = minutes / 60;
	int days = hours / 24;

	seconds = seconds % 60;
	minutes = minutes % 60;
	hours = hours % 24;

	char time[20];

	if (days > 0) {
		itoa(days, time, 10);
		terminal_writestring(time);
		terminal_writestring(" days, ");
	}
	
	if (hours > 0) {
		itoa(hours, time, 10);
		terminal_writestring(time);
		terminal_writestring(" hours, ");
	}

	if (minutes > 0) {
		itoa(minutes, time, 10);
		terminal_writestring(time);
		terminal_writestring(" minutes, ");
	}

	itoa(seconds, time, 10);
	terminal_writestring(time);
	terminal_writestring(" seconds.");
}


void uptime_command() {
	terminal_writestring("Uptime: ");

	print_ticks_to_time(timer_ticks);
}


void animation_test() {
	int framerate = 100/2;

	clear_screen();

	char animation_frames [9][25][80] = {
		{
			"     |\\__/,|   (`\\",
			"    |o o  |    ) )  ",
			"  _.\\ ^  /_  ( (   ",
			"-(((---(((--------  "
		},
		{
			"                    ",
			"    |\\__/,|   (`\\ ",
			"  _.|o o  |_   ) )  ",
			"-(((---(((--------  "
		},
		{
			"                    ",
			"                    ",
			"  _.|\\__/,|_  (`\\ ",
			"-(((---(((--------  "
		},
		{
			"                    ",
			"                    ",
			"              (`\\  ",
			"-(((---(((--------  "
		},
		{
			"                    ",
			"                    ",
			"              (`\\  ",
			"------------------  "
		},
		{
			"                    ",
			"                    ",
			"              (`\\  ",
			"-(((---(((--------  "
		},
		{
			"                    ",
			"                    ",
			"  _.|\\__/,|_  (`\\ ",
			"-(((---(((--------  "
		},
		{
			"                    ",
			"    |\\__/,|   (`\\ ",
			"  _.|o o  |_   ) )  ",
			"-(((---(((--------  "
		},
		{
			"     |\\__/,|   (`\\",
			"    |o o  |    ) )  ",
			"  _.\\ ^  /_  ( (   ",
			"-(((---(((--------  "
		}
	};

	for (int k = 0; k < 4; k++) {
		for (int i = 0; i < 9; i++) {
			terminal_column = 0;
			terminal_row = 0;
			for (int j = 0; j < 5; j++) {
				terminal_writestring(animation_frames[i][j]);
				newline();
			}

			sleep(framerate);
		}
	}
}


// Checks if the command is valid and executes it
void check_for_command() {
	newline();

	if (num_commands < MAX_COMMANDS && strlen(command) > 0 && strcmp(previous_commands[num_commands - 1], command) != 0) {
        strcpy(previous_commands[num_commands], command);
        num_commands++;
		at_command = num_commands;
    }

	is_writing_command = false;

	// checks if the command is a game command
	if (strcmplen(command, "game", 4) == 0 && !in_game) {
		game_handler(game_round);
		end_check_for_command();
		return;
	} else if (in_game) {
		game_handler(game_round);
		end_check_for_command();
		return;
	}

	if (in_calculator) {
		calculator_calculate();
		end_check_for_command();
		return;
	}

	// checks if the command is a normal command
	if (strcmplen(command, "color ", 6) == 0) {
		color_command();
	} else if (strcmp(command, "clear") == 0) {
		clear_screen();
	} else if (strcmp(command, "help") == 0) {
		terminal_writestring("Commands:");
		newline();
		terminal_writestring("clear - clears the screen");
		newline();
		terminal_writestring("help - shows this message");
		newline();
		terminal_writestring("cat - prints the cat");	
		newline();
		terminal_writestring("game [game name] - starts the specified game");
		newline();
		terminal_writestring("gamelist - lists all the games");
		newline();
		terminal_writestring("write - opens a text editor");
		newline();
		terminal_writestring("execute - executes the code from the text editor");
		newline();
		terminal_writestring("meow - prints meow :3");
		newline();
		terminal_writestring("color [bg or/and all] [(name of color), rainbow or reset] - changes the color of the text");
		newline();
		terminal_writestring("uptime - prints the time the kernel has been running");
	} else if (strcmp(command, "cat") == 0) {
		terminal_writestring("  _____");
		newline();
		terminal_writestring(" |     |___ ___ _ _ _                 |\\__/,|   (`\\");
		newline();
		terminal_writestring(" | | | | -_| . | | | |              _.|o o  |_   ) )");
		newline();
		terminal_writestring(" |_|_|_|___|___|_____|             -(((---(((--------");
	}  else if (strcmp(command, "meow") == 0) {
		terminal_writestring("meow! :3");
	} else if (strcmp(command, "write") == 0) {
		text_editor();
		return;		
	} else if (strcmp(command, "execute") == 0) {
		execute_text();
		terminal_writestring("Executed the code from the text editor");
	} else if (strcmp(command, "gamelist") == 0) {
		terminal_writestring("Games:");
		newline();
		terminal_writestring("rock paper scissors - RPS");
		newline();
		terminal_writestring("tic tac toe - TTT");
		newline();
		terminal_writestring("tower defense - TD");
		newline();
		terminal_writestring("minesweeper - MS params: -w[num] (width), -h[num] (height), -m[num] (mines)");
	} else if (strcmp(command, "uptime") == 0) {
		uptime_command();
	} else if (strcmp(command, "animation") == 0) {
		animation_test();
	} else if (strcmp(command, "kalkulator") == 0) {
		terminal_writestring("Ingen mellomrom ples :)");
		calculator_start();
	}  
	
	else if (strcmp(command, "kukbart") == 0) { // best code ever written. By banana toucher beats B)
		terminal_writestring("Kukbart finner ikke lommeboken sin! Kan du hjelpe han? PS: (Riktig svar er nei;))!");
	} 
	

	// dev commands 
	else if (strcmp(command, "vga test") == 0) { 
		vga_enter();
		draw_rectangle(0, 0, 80, 25, VGA_COLOR_BLUE);
	} else if (strcmp(command, "exception test") == 0) {
		int a = 1 / 0;
		char b[10];
		itoa(a, b, 10);
		terminal_writestring(b);
	}

	// if the command is not found in the
	else {
		terminal_writestring("Unknown command: ");
		terminal_writestring(command);
	}

	end_check_for_command();
}


// Ends the check for command, used to make the code more readable and shorter
void end_check_for_command() {
	is_writing_command = true;
	at_command = num_commands;
	command[0] = '\0';
	new_kernel_line();
}


// Puts a character in the kernel, used by terminal_write
void terminal_putchar(unsigned char c) {
	should_print = true;

	if (is_writing_command || in_text_editor) {
		keyboard_handler(c);
	}

	if (is_writing_command && strlen(command) >= MAX_COMMAND_LENGTH) {
		return;
	}

	if (!should_print || c == 0) {
		return;
	}

	if (is_writing_command || in_text_editor) {
		c = kbd_US[c];
	}

	if ((shift_pressed || caps_lock) && (is_writing_command || in_text_editor)) {
		if (c >= 'a' && c <= 'z') {
			c -= 32;
		}

		if (c >= '1' && c <= '9') {
			c -= 16;
		}

		if (c == '0') {
			c = '=';
		}
	}

	if (is_writing_command) { 
		int len = strlen(command);

		if (at_in_command == len) {
			command[len] = c;
			command[len + 1] = '\0';
		} else if (at_in_command < len) {

			char buffer[MAX_COMMAND_LENGTH];
			for (int i = at_in_command; i < len; i++) {
				buffer[i - at_in_command] = command[i];
			}

			for (int i = 0; i < len - at_in_command; i++) {
				command[i + at_in_command + 1] = buffer[i];
			}

			command[len + 1] = '\0';
		
			command[at_in_command] = c;

			int prev_col = terminal_column;
			int prev_row = terminal_row;

			terminal_column = 2;
			terminal_row = command_start_row;

			rollover = false;

			is_writing_command = false;
			terminal_writestring(command);
			is_writing_command = true;
			terminal_column = prev_col;
			terminal_row = prev_row;

			if (rollover) {
				terminal_row--;
			}

		}

		at_in_command++;		
		
		char test[3];
		itoa(at_in_command, test, 10);
		for (int i = 0; i < 3; i++) {
			if (test[i] == '\0') {
				break;
			}
			terminal_putentryat(test[i], terminal_color, i, 24);
		}
	}

	if (in_text_editor) {
		if (terminal_column == 79) {
			return;
		}

		int len = strlen(text_editor_text[terminal_row - 3]);
		if (terminal_column == len + 3) {
			text_editor_text[terminal_row - 3][len] = c;
			text_editor_text[terminal_row - 3][len + 1] = '\0';
		} else if (terminal_column < len + 3) {
			char buffer[80];
			for (int i = terminal_column - 3; i < len; i ++) {
				buffer[i - (terminal_column - 3)] = text_editor_text[terminal_row - 3][i];
			}

			for (int i = (terminal_column - 3); i < len; i++) {
				text_editor_text[terminal_row - 3][i + 1] = buffer[i - (terminal_column - 3)];
			}
		
			text_editor_text[terminal_row - 3][terminal_column - 3] = c;

			int prev_col = terminal_column;
			terminal_column = 3;
			in_text_editor = false;
			terminal_writestring(text_editor_text[terminal_row - 3]);
			in_text_editor = true;
			terminal_column = prev_col;
		}
	}



	terminal_putentryat(c, terminal_color, terminal_column, terminal_row);

	terminal_column++;

	if (terminal_column == VGA_WIDTH) {
		terminal_column = 0;
		terminal_row++;

		// this is to make the words not break on new lines
		// its not working, will fix later, not a priority

		// if (is_writing_command) {
		// 	is_writing_command = false;

		// 	terminal_putentryat('X', terminal_color, 0, 24);
		// 	int j = 0;
		// 	for (int i = strlen(command); i > 0; i--) {
		// 		if (command[i] == ' ') {
		// 			break;
		// 		}
		// 		terminal_putentryat(command[i], terminal_color, j, terminal_row);
		// 		terminal_putentryat(' ', terminal_color, i + 2, terminal_row - 1);
				
		// 		j++;
		// 	}

		// 	terminal_column = j;
		// 	is_writing_command = true;
		// }
		
		if (terminal_row == VGA_HEIGHT) {
			terminal_row++;
		}
	}

	
	if (check_scroll) {
		check_kernel_scroll();
	}
}

// Writes a string to the kernel, by looping through string and calling terminal_putchar
void terminal_write(const char* data, size_t size) {
	for (size_t i = 0; i < size; i++)
		terminal_putchar(data[i]);
}
 

// Writes a string to the kernel, most used write function
void terminal_writestring(const char* data) {
	terminal_write(data, strlen(data));
}


// Sends a byte to a port
void outb( unsigned short port, unsigned char val ) {
   asm volatile("outb %0, %1" : : "a"(val), "Nd"(port) );
}


// Reads a byte from a port
static __inline unsigned char inb (unsigned short int port) {
  unsigned char _v;

  __asm__ __volatile__ ("inb %w1,%0":"=a" (_v):"Nd" (port));
  return _v;
}

// Moves the cursor to a specific location, used to move cursor when writing commands
void move_cursor(int x, int y) {
    uint16_t pos = y * VGA_WIDTH + x;
    outb(0x3D4, 14);
    outb(0x3D5, pos >> 8);
    outb(0x3D4, 15);
    outb(0x3D5, pos);
}


void *memset(void *s, int c, size_t n) {
    unsigned char* p=s;
    while(n--)
        *p++ = (unsigned char)c;
    return s;
}

static inline void outportb(unsigned short port, unsigned char val)
{
    asm volatile ( "outb %0, %1" : : "a"(val), "Nd"(port) );
}

struct regs
{
    unsigned int gs, fs, es, ds;      /* pushed the segs last */
    unsigned int edi, esi, ebp, esp, ebx, edx, ecx, eax;  /* pushed by 'pusha' */
    unsigned int int_no, err_code;    /* our 'push byte #' and ecodes do this */
    unsigned int eip, cs, eflags, useresp, ss;   /* pushed by the processor automatically */ 
};


void sleep(int ticks)
{
    unsigned long eticks;

    eticks = timer_ticks + ticks;

    while(timer_ticks < eticks) {
		asm volatile ("pause");
	}
}


void timer_phase(int hz)
{
    int divisor = 1193180 / hz;       /* Calculate our divisor */
    outportb(0x43, 0x36);             /* Set our command byte 0x36 */
    outportb(0x40, divisor & 0xFF);   /* Set low byte of divisor */
    outportb(0x40, divisor >> 8);     /* Set high byte of divisor */
}

/* Handles the timer. By default, the timer fires 18.222 times per second. set it to 100 in kernel_main. */
void timer_handler(struct regs *r) {
	// bool was_writing_command = is_writing_command; // temporary, used for bug testing
	// is_writing_command = false;
	// char ticks_str[10];
	// itoa(timer_ticks, ticks_str, 10);
	// for (int i = 0; i < strlen(ticks_str); i++) {
	// 	terminal_putentryat(ticks_str[i], terminal_color, i, 23);
	// }
	// is_writing_command = was_writing_command; // end of temporary

    timer_ticks++;

}


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

	if (is_writing_command || in_text_editor) {	
		terminal_putchar(kbd_special_characters[c]);
		move_cursor(terminal_column, terminal_row);
	}
	
}


void timer_install()
{
    irq_install_handler(0, timer_handler);
}


void keyboard_install()
{
	irq_install_handler(1, keyboard_interrupt_handler);
}


extern void irq0();
extern void irq1();
extern void irq2();
extern void irq3();
extern void irq4();
extern void irq5();
extern void irq6();
extern void irq7();
extern void irq8();
extern void irq9();
extern void irq10();
extern void irq11();
extern void irq12();
extern void irq13();
extern void irq14();
extern void irq15();

/* This array is actually an array of function pointers. We use
*  this to handle custom IRQ handlers for a given IRQ */
void *irq_routines[16] =
{
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0
};

/* This installs a custom IRQ handler for the given IRQ */
void irq_install_handler(int irq, void (*handler)(struct regs *r))
{
    irq_routines[irq] = handler;
}

/* This clears the handler for a given IRQ */
void irq_uninstall_handler(int irq)
{
    irq_routines[irq] = 0;
}



/* Normally, IRQs 0 to 7 are mapped to entries 8 to 15. This
*  is a problem in protected mode, because IDT entry 8 is a
*  Double Fault! Without remapping, every time IRQ0 fires,
*  you get a Double Fault Exception, which is NOT actually
*  what's happening. We send commands to the Programmable
*  Interrupt Controller (PICs - also called the 8259's) in
*  order to make IRQ0 to 15 be remapped to IDT entries 32 to
*  47 */
void irq_remap(void)
{
    outportb(0x20, 0x11);
    outportb(0xA0, 0x11);
    outportb(0x21, 0x20);
    outportb(0xA1, 0x28);
    outportb(0x21, 0x04);
    outportb(0xA1, 0x02);
    outportb(0x21, 0x01);
    outportb(0xA1, 0x01);
    outportb(0x21, 0x0);
    outportb(0xA1, 0x0);
}

/* We first remap the interrupt controllers, and then we install
*  the appropriate ISRs to the correct entries in the IDT. This
*  is just like installing the exception handlers */
void irq_install()
{
    irq_remap();

    idt_set_gate(32, (unsigned)irq0, 0x08, 0x8E);
    idt_set_gate(33, (unsigned)irq1, 0x08, 0x8E);
	idt_set_gate(34, (unsigned)irq2, 0x08, 0x8E);
	idt_set_gate(35, (unsigned)irq3, 0x08, 0x8E);
	idt_set_gate(36, (unsigned)irq4, 0x08, 0x8E);
	idt_set_gate(37, (unsigned)irq5, 0x08, 0x8E);
	idt_set_gate(38, (unsigned)irq6, 0x08, 0x8E);
	idt_set_gate(39, (unsigned)irq7, 0x08, 0x8E);
	idt_set_gate(40, (unsigned)irq8, 0x08, 0x8E);
	idt_set_gate(41, (unsigned)irq9, 0x08, 0x8E);
	idt_set_gate(42, (unsigned)irq10, 0x08, 0x8E);
	idt_set_gate(43, (unsigned)irq11, 0x08, 0x8E);
	idt_set_gate(44, (unsigned)irq12, 0x08, 0x8E);
	idt_set_gate(45, (unsigned)irq13, 0x08, 0x8E);
	idt_set_gate(46, (unsigned)irq14, 0x08, 0x8E);
    idt_set_gate(47, (unsigned)irq15, 0x08, 0x8E);
}




/* Each of the IRQ ISRs point to this function, rather than
*  the 'fault_handler' in 'isrs.c'. The IRQ Controllers need
*  to be told when you are done servicing them, so you need
*  to send them an "End of Interrupt" command (0x20). There
*  are two 8259 chips: The first exists at 0x20, the second
*  exists at 0xA0. If the second controller (an IRQ from 8 to
*  15) gets an interrupt, you need to acknowledge the
*  interrupt at BOTH controllers, otherwise, you only send
*  an EOI command to the first controller. If you don't send
*  an EOI, you won't raise any more IRQs */
void irq_handler(struct regs *r)
{

    /* This is a blank function pointer */
    void (*handler)(struct regs *r);

    /* Find out if we have a custom handler to run for this
    *  IRQ, and then finally, run it */
    handler = irq_routines[r->int_no - 32];
    if (handler)
    {
        handler(r);
    }

    /* If the IDT entry that was invoked was greater than 40
    *  (meaning IRQ8 - 15), then we need to send an EOI to
    *  the slave controller */
    if (r->int_no >= 40)
    {
        outportb(0xA0, 0x20);
    }

    /* In either case, we need to send an EOI to the master
    *  interrupt controller too */
    outportb(0x20, 0x20);
}


extern void isr0();
extern void isr1();
extern void isr2();
extern void isr3();
extern void isr4();
extern void isr5();
extern void isr6();
extern void isr7();
extern void isr8();
extern void isr9();
extern void isr10();
extern void isr11();
extern void isr12();
extern void isr13();
extern void isr14();
extern void isr15();
extern void isr16();
extern void isr17();
extern void isr18();
extern void isr19();
extern void isr20();
extern void isr21();
extern void isr22();
extern void isr23();
extern void isr24();
extern void isr25();
extern void isr26();
extern void isr27();
extern void isr28();
extern void isr29();
extern void isr30();
extern void isr31();


void isrs_install()
{
    idt_set_gate(0, (unsigned)isr0, 0x08, 0x8E);
    idt_set_gate(1, (unsigned)isr1, 0x08, 0x8E);
    idt_set_gate(2, (unsigned)isr2, 0x08, 0x8E);
    idt_set_gate(3, (unsigned)isr3, 0x08, 0x8E);
	idt_set_gate(4, (unsigned)isr4, 0x08, 0x8E);
	idt_set_gate(5, (unsigned)isr5, 0x08, 0x8E);
	idt_set_gate(6, (unsigned)isr6, 0x08, 0x8E);
	idt_set_gate(7, (unsigned)isr7, 0x08, 0x8E);
	idt_set_gate(8, (unsigned)isr8, 0x08, 0x8E);
	idt_set_gate(9, (unsigned)isr9, 0x08, 0x8E);
	idt_set_gate(10, (unsigned)isr10, 0x08, 0x8E);
	idt_set_gate(11, (unsigned)isr11, 0x08, 0x8E);
	idt_set_gate(12, (unsigned)isr12, 0x08, 0x8E);
	idt_set_gate(13, (unsigned)isr13, 0x08, 0x8E);
	idt_set_gate(14, (unsigned)isr14, 0x08, 0x8E);
	idt_set_gate(15, (unsigned)isr15, 0x08, 0x8E);
	idt_set_gate(16, (unsigned)isr16, 0x08, 0x8E);
	idt_set_gate(17, (unsigned)isr17, 0x08, 0x8E);
	idt_set_gate(18, (unsigned)isr18, 0x08, 0x8E);
	idt_set_gate(19, (unsigned)isr19, 0x08, 0x8E);
	idt_set_gate(20, (unsigned)isr20, 0x08, 0x8E);
	idt_set_gate(21, (unsigned)isr21, 0x08, 0x8E);
	idt_set_gate(22, (unsigned)isr22, 0x08, 0x8E);
	idt_set_gate(23, (unsigned)isr23, 0x08, 0x8E);
	idt_set_gate(24, (unsigned)isr24, 0x08, 0x8E);
	idt_set_gate(25, (unsigned)isr25, 0x08, 0x8E);
	idt_set_gate(26, (unsigned)isr26, 0x08, 0x8E);
	idt_set_gate(27, (unsigned)isr27, 0x08, 0x8E);
	idt_set_gate(28, (unsigned)isr28, 0x08, 0x8E);
	idt_set_gate(29, (unsigned)isr29, 0x08, 0x8E);
    idt_set_gate(30, (unsigned)isr30, 0x08, 0x8E);
    idt_set_gate(31, (unsigned)isr31, 0x08, 0x8E);
}

unsigned char *exception_messages[] =
{
    "Division By Zero",
    "Debug",
    "Non Maskable Interrupt",
    "Breakpoint",
	"Into Detected Overflow",
	"Out of Bounds",
	"Invalid Opcode",
	"No Coprocessor",
	"Double Fault",
	"Coprocessor Segment Overrun",
	"Bad TSS",
	"Segment Not Present",
	"Stack Fault",
	"General Protection Fault",
	"Page Fault",
	"Unknown Interrupt",
	"Coprocessor Fault",
	"Alignment Check",
	"Machine Check",
	"Reserved",
	"Reserved",
	"Reserved",
	"Reserved",
	"Reserved",
	"Reserved",
	"Reserved",
	"Reserved",
    "Reserved",
	"Reserved",
	"Reserved",
	"Reserved",
    "Reserved"
};




void fault_handler(struct regs *r) {
    /* Is this a fault whose number is from 0 to 31? */
    if (r->int_no < 32)
    {
        /* Display the description for the Exception that occurred.
        *  In this tutorial, we will simply halt the system using an
        *  infinite loop */
        terminal_writestring(exception_messages[r->int_no]);
        terminal_writestring(" Exception. System Halted!");
        for (;;);
    }
}







bool main_exit_flag = false;


// The main function, called at the start of the kernel, calls all the other functions
void kernel_main(void) {
	gdt_install();
	idt_install();
	isrs_install();
	irq_install();
	
	__asm__ __volatile__ ("sti");

	terminal_initialize();
	irq_remap();
	timer_install();
	timer_phase(100);
	keyboard_install();

	// Prints the cat
	terminal_writestring("   _____");
	newline();
    terminal_writestring(" |     |___ ___ _ _ _                 |\\__/,|   (`\\");
	newline();
    terminal_writestring(" | | | | -_| . | | | |              _.|o o  |_   ) )");
	newline();
    terminal_writestring(" |_|_|_|___|___|_____|             -(((---(((--------");
	newline();

	// Cat (Meow)
	// 
	//         |\__/,|   (`\
  	//	     _.|o o  |_   ) )
	//	   -(((---(((--------
	//  _____               
	// |     |___ ___ _ _ _ 
    // | | | | -_| . | | | |
	// |_|_|_|___|___|_____|
	

	// start of kernel
	command[0] = '\0';
	new_kernel_line();
	is_writing_command = true;
	move_cursor(terminal_column, terminal_row);

	while (!main_exit_flag) {
		bool was_writing_command = is_writing_command; // temporary, used for bug testing
		is_writing_command = false;
		
		if (in_TD) {
			is_writing_command = true;
			tower_defense_play();
		}

		if (in_mine_sweeper) {
			is_writing_command = true;
			mine_sweeper_play();
		}

		if (in_game_of_life) {
			is_writing_command = true;
			game_of_life_play();
		}

		if (in_pong) {
			is_writing_command = true;
			pong_play();
		}
		
		is_writing_command = was_writing_command; // end of temporary
		sleep(1);
	}
}