#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#define INT_DISABLE 0
#define INT_ENABLE  0x200
#define PIC1 0x20
#define PIC2 0xA0

#define ICW1 0x11
#define ICW4 0x01


// english keyboard layout TODO: add norwegian keyboard layout
char kbd_US [256] = {
    0,  0, '1', '2', '3', '4', '5', '6', '7', '8', '9', '0', '-', '=', '\b', /* <-- Backspace */   
 '\t', /* <-- Tab */
  'q', 'w', 'e', 'r', 't', 'y', 'u', 'i', 'o', 'p', '[', ']', '\n', /* <-- Enter key */     
   	0, /* <-- control key */
  'a', 's', 'd', 'f', 'g', 'h', 'j', 'k', 'l', ';', '\'', '`',  0, /* <-- left shift */ '\\', 'z', 'x', 'c', 'v', 'b', 'n', 'm', ',', '.', '/',   0, /* <-- right shift */
  '*',
    0,  /* Alt */
  ' ',  /* Space bar */
    0,  /* Caps lock */
    0,  /* 59 - F1 key ... > */
    0,   0,   0,   0,   0,   0,   0,   0,
    0,  /* < ... F10 */
    0,  /* 69 - Num lock*/
    0,  /* Scroll Lock */
    0,  /* Home key */
    0,  /* Up Arrow */
    0,  /* Page Up */
  '-',
    0,  /* Left Arrow */
    0,
    0,  /* Right Arrow */
  '+',
    0,  /* 79 - End key*/
    0,  /* Down Arrow */
    0,  /* Page Down */
    0,  /* Insert Key */
    0,  /* Delete Key */
    0,   0,   0,
    0,  /* F11 Key */
    0,  /* F12 Key */
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, /* 100 */
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, /* 110 */
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, /* 120 */
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, /* 130, key releases after number 128 */
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, /* 140 */
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, /* 150 */
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, /* 160 */
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, /* 170, 170 - left shift */
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, /* 180 */
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, /* 190, 182 - right shift */
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0 /* 200, rest are added if needed. */
};

char kbd_special_characters[256] = {
    0,  1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, /* <-- Backspace */
   15, /* <-- Tab */
   16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, /* <-- Enter key */
    0, /* <-- control key */
   30, 31, 32, 33, 34, 35, 36, 37, 38, 39, 40, 41, 42, /* <-- left shift */
   43, 44, 45, 46, 47, 48, 49, 50, 51, 52, 53, 54, /* <-- right shift */
   55,
    0,  /* Alt */
   57,  /* Space bar */
   58,  /* Caps lock */
    0,  /* 59 - F1 key ... > */
    0,   0,   0,   0,   0,   0,   0,   0,
    0,  /* < ... F10 */
    0,  /* 69 - Num lock*/
    0,  /* Scroll Lock */
    0,  /* Home key */
   72,  /* Up Arrow */
    0,  /* Page Up */
   74,
   75,  /* Left Arrow */
    0,
   77,  /* Right Arrow */
   78,
    0,  /* 79 - End key*/
   80,  /* Down Arrow */
    0,  /* Page Down */
    0,  /* Insert Key */
    0,  /* Delete Key */
    0,   0,   0,
    0,  /* F11 Key */
    0,  /* F12 Key */
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, /* 100 */
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, /* 110 */
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, /* 120 */
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, /* 130, key releases after number 128 */
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, /* 140 */
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, /* 150 */
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, /* 160 */
	0, 0, 0, 0, 0, 0, 0, 0, 0, 170, /* 170, 170 - left shift */
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, /* 180 */
	0, 182, 0, 0, 0, 0, 0, 0, 0, 0, /* 190, 182 - right shift */
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0 /* 200, rest are added if needed. */
};


// Colors used by the kernel, used so that colors are easier to use
enum vga_color {
	VGA_COLOR_BLACK = 0,
	VGA_COLOR_BLUE = 1,
	VGA_COLOR_GREEN = 2,
	VGA_COLOR_CYAN = 3,
	VGA_COLOR_RED = 4,
	VGA_COLOR_MAGENTA = 5,
	VGA_COLOR_BROWN = 6,
	VGA_COLOR_LIGHT_GREY = 7,
	VGA_COLOR_DARK_GREY = 8,
	VGA_COLOR_LIGHT_BLUE = 9,
	VGA_COLOR_LIGHT_GREEN = 10,
	VGA_COLOR_LIGHT_CYAN = 11,
	VGA_COLOR_LIGHT_RED = 12,
	VGA_COLOR_LIGHT_MAGENTA = 13,
	VGA_COLOR_LIGHT_BROWN = 14,
	VGA_COLOR_WHITE = 15,
};
 
static inline uint8_t vga_entry_color(enum vga_color fg, enum vga_color bg) {
	return fg | bg << 4;
}
 
static inline uint16_t vga_entry(unsigned char uc, uint8_t color) {
	return (uint16_t) uc | (uint16_t) color << 8;
}

size_t strlen(const char* str) {
	size_t len = 0;
	while (str[len])
		len++;
	return len;
}
 
static const size_t VGA_WIDTH = 80;
static const size_t VGA_HEIGHT = 25;
 
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
 

// Puts a character at a specific location in the kernel
void terminal_putentryat(unsigned char c, uint8_t color, size_t x, size_t y) {
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


// Checks if the kernel should scroll and scrolls if needed
void check_kernel_scroll() {
	if (terminal_row == VGA_HEIGHT - 1) {
		my_memmove(terminal_buffer, terminal_buffer + VGA_WIDTH, VGA_WIDTH * (VGA_HEIGHT - 1) * 2);
		for (size_t x = 0; x < VGA_WIDTH; x++) {
			const size_t index = (VGA_HEIGHT - 1) * VGA_WIDTH + x;
			terminal_buffer[index] = vga_entry(' ', terminal_color);
		}
		terminal_row--;
	}
}


// Creates a new line in the kernel
void newline() {
	terminal_row++;
	terminal_column = 0;

	check_kernel_scroll();
}

bool should_print = true;
bool is_writing_command = false;
static bool shift_pressed = false;


// calls newline and prints "> " to indicate that the user can write a command
void new_kernel_line() {
	newline();
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
			terminal_buffer[index] = vga_entry(' ', terminal_color);
		}
	}
	terminal_row = 0;
	terminal_column = 0;
}



#define MAX_COMMANDS 100
#define MAX_COMMAND_LENGTH 256

char previous_commands[MAX_COMMANDS][MAX_COMMAND_LENGTH];
int num_commands = 0;
int at_command = 0;
char command[MAX_COMMAND_LENGTH];
bool in_game = false;
char game[MAX_COMMAND_LENGTH];
int game_round = 1;

char tic_tac_toe_board[5][5] = {
	{' ', '|', ' ', '|', ' '},
	{'-', '+', '-', '+', '-'},
	{' ', '|', ' ', '|', ' '},
	{'-', '+', '-', '+', '-'},
	{' ', '|', ' ', '|', ' '}
};

int tic_tac_toe_occupied[5][5] = {
	{0, 1, 0, 1, 0},
	{1, 1, 1, 1, 1},
	{0, 1, 0, 1, 0},
	{1, 1, 1, 1, 1},
	{0, 1, 0, 1, 0}
};


void print_tic_tac_toe_board() {
	for (int i = 0; i < 5; i++) {
		for (int j = 0; j < 5; j++) {	
			char str[2] = {tic_tac_toe_board[i][j], '\0'};
			terminal_writestring(str);
		}
		if (i != 4) {
			newline();
		}
	}
}


void end_tic_tac_toe() {
	for (int i = 0; i < 5; i = i + 2) {
		tic_tac_toe_board[i][0] = ' ';
		tic_tac_toe_board[i][2] = ' ';
		tic_tac_toe_board[i][4] = ' ';
		tic_tac_toe_occupied[i][0] = 0;
		tic_tac_toe_occupied[i][2] = 0;
		tic_tac_toe_occupied[i][4] = 0;
	}

	game_round = 1;
	in_game = false;
}


int check_for_win_ttt() {
	for (size_t i = 0; i < 5; i = i + 2) {
		if (tic_tac_toe_board[i][0] == tic_tac_toe_board[i][2] && tic_tac_toe_board[i][2] == tic_tac_toe_board[i][4]) {
			if (tic_tac_toe_board[i][0] == 'X') {
				return 1;
			} else if (tic_tac_toe_board[i][0] == 'O') {
				return 2;
			}
		}

		if (tic_tac_toe_board[0][i] == tic_tac_toe_board[2][i] && tic_tac_toe_board[2][i] == tic_tac_toe_board[4][i]) {
			if (tic_tac_toe_board[0][i] == 'X') {
				return 1;
			} else if (tic_tac_toe_board[0][i] == 'O') {
				return 2;
			}
		}
	}
	if ((tic_tac_toe_board[0][0] == tic_tac_toe_board[2][2] && tic_tac_toe_board[2][2] == tic_tac_toe_board[4][4]) || (tic_tac_toe_board[0][4] == tic_tac_toe_board[2][2] && tic_tac_toe_board[2][2] == tic_tac_toe_board[4][0])) {
		if (tic_tac_toe_board[2][2] == 'X') {
			return 1;
		} else if (tic_tac_toe_board[2][2] == 'O') {
			return 2;
		}
	}
	
	return 0;
}


// Tic tac toe game TODO: make the pc better at the game
void tic_tac_toe() {
	if (game_round == 1) {
		terminal_writestring("Tic tac toe, you go first!");
		newline();
		terminal_writestring("Type the coordinates of the square you want to place your X in");
		newline();
		terminal_writestring("(for example 1,1 for upper left)");
		newline();
		terminal_writestring("Current board:");
		newline();
		print_tic_tac_toe_board();

		game_round++;
		return;

	} else {
		int x = command[0] - '0';
		int y = command[2] - '0';

		if (x == 1) {
			x = 0;
		} else if (x == 3) {
			x = 4;
		} else if (x != 2) {
			print_tic_tac_toe_board();
			newline();
			terminal_writestring("That is not a valid move, Try again.");
			return;
		}

		if (y == 1) {
			y = 0;
		} else if (y == 3) {
			y = 4;
		} else if (y != 2) {
			print_tic_tac_toe_board();
			newline();
			terminal_writestring("That is not a valid move, Try again.");
			return;
		}

		if (tic_tac_toe_occupied[x][y] == 1) {
			print_tic_tac_toe_board();
			newline();
			terminal_writestring("That place is occupied, Try again.");
			return;
		}
		tic_tac_toe_board[x][y] = 'X';
		tic_tac_toe_occupied[x][y] = 1;

		check_for_win_ttt();
		if (check_for_win_ttt() == 1) {
			print_tic_tac_toe_board();
			newline();
			terminal_writestring("You win!");
			end_tic_tac_toe();
			return;
		} else if (check_for_win_ttt() == 2) {
			print_tic_tac_toe_board();
			newline();
			terminal_writestring("You lose!");
			end_tic_tac_toe();
			return;
		}

		bool placed = false;
		for (size_t i = 0; i < 5; i = i + 2) {
			for (size_t j = 0; j < 5; j = j + 2) {
				if (tic_tac_toe_occupied[i][j] == 0) {
					tic_tac_toe_board[i][j] = 'O';
					tic_tac_toe_occupied[i][j] = 1;
					placed = true;					
				}
				if (placed) {
					break;
				}
			}
			if (placed) {
				break;
			}
		}
		if (!placed) {
			print_tic_tac_toe_board();
			newline();
			terminal_writestring("Its a tie!");
			end_tic_tac_toe();
			return;
		}

		check_for_win_ttt();
		if (check_for_win_ttt() == 1) {
			print_tic_tac_toe_board();
			newline();
			terminal_writestring("You win!");
			end_tic_tac_toe();
			return;
		} else if (check_for_win_ttt() == 2) {
			print_tic_tac_toe_board();
			newline();
			terminal_writestring("You lose!");
			end_tic_tac_toe();
			return;
		}

		print_tic_tac_toe_board();
		newline();
		terminal_writestring("Your turn.");
		game_round++;
		return;
	}
}


// Rock, paper, scissors game (the pc always wins)
void rock_paper_scissors() {
	if (game_round == 1) {
		terminal_writestring("Rock, paper, scissors, you go first!");
		newline();
		terminal_writestring("Type rock, paper or scissors");
		game_round++;
		return;

	} else if (game_round == 2) {
		if (strcmp(command, "rock") == 0) {
			terminal_writestring("I choose paper, You lose!");
		} else if (strcmp(command, "paper") == 0) {
			terminal_writestring("I choose scissors, You lose!");
		} else if (strcmp(command, "scissors") == 0) {
			terminal_writestring("I choose rock, You lose!");
		} else {
			terminal_writestring("That is not a valid move, You lose!");
		}
		game_round = 1;
		in_game = false;
		return;
	}
}


// Handles the game command, and calls the correct game
void game_handler() {
	in_game = true;
	
	if (game_round == 1) {
		if (strlen(command) > 5) {
			for (int i = 0; i < strlen(command) - 5; i++) {
				game[i] = command[i + 5];
			}
		} else {
			terminal_writestring("please specify a game");
			in_game = false;
			return;
		}
		game[strlen(command) - 5] = '\0';
	}
	
	if (strcmp(game, "rock paper scissors") == 0 || strcmp(game, "RPS") == 0) {
		rock_paper_scissors();
	} else if (strcmp(game, "tic tac toe") == 0 || strcmp(game, "TTT") == 0) {
		tic_tac_toe();
	}
	
	else {
		terminal_writestring("Unknown game: ");
		terminal_writestring(game);
		in_game = false;
	}
}


// Command for changing the colors of the kernel
int chosen_color = 13;
int chosen_bg_color = 0;

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

	int chosen_new_color = 0;
	size_t num_colors = sizeof(color_table) / sizeof(Color);

	for (size_t i = 0; i < num_colors; i++) { 
		if (strcmp(change_color, color_table[i].name) == 0) {
			chosen_new_color = color_table[i].vga_color;
			break;
		}
	}

	if (chosen_new_color == 0) {
		terminal_writestring("Unknown color: ");
		terminal_writestring(change_color);
		return;
	}

	if (bg) {
		chosen_bg_color = chosen_new_color;
	} else {
		chosen_color = chosen_new_color;
	}

	terminal_color = vga_entry_color(chosen_color, chosen_bg_color);

	if (all) {
		for (size_t y = 0; y < VGA_HEIGHT; y++) {
			for (size_t x = 0; x < VGA_WIDTH; x++) {
				const size_t index = y * VGA_WIDTH + x;
				terminal_buffer[index] = vga_entry(terminal_buffer[index], terminal_color);
			}
		}
	}

	terminal_writestring("Color changed :)");
}


// Things used by the text editor
bool check_scroll = true;
bool in_text_editor = false;
bool text_editor_exit_flag = false;
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
	}

	if (strcmplen(text, "print ", 6) == 0) {
		for (int i = 0; i < len - 6; i++) {
			execute_content[i] = text[i + 6];
		}
		execute_content[len - 6] = '\0';
		terminal_writestring(execute_content);
		newline();
	}

	if (strcmplen(text, "loop ", 5) == 0) {
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


// Opens a text editor
void text_editor() {
	check_scroll = false;
	in_text_editor = false;
	text_editor_exit_flag = false;

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

	// starts the text editor input loop
	in_text_editor = true;
	terminal_row = 3;
	terminal_column = 3;
	input_loop(&text_editor_exit_flag);
}


// Checks if the command is valid and executes it
void check_for_command() {
	newline();

	if (num_commands < MAX_COMMANDS) {
        strcpy(previous_commands[num_commands], command);
        num_commands++;
		at_command = num_commands;
    }

	is_writing_command = false;


	// checks if the command is a game command
	char is_game[6];
	for (int i = 0; i < 6; i++) {
		is_game[i] = command[i];
	}
	is_game[5] = '\0';

	if (strcmp(is_game, "game ") == 0 && !in_game) {
		game_handler(game_round);
		end_check_for_command();
		return;
	} else if (in_game) {
		game_handler(game_round);
		end_check_for_command();
		return;
	}
	

	// checks if the command is a normal command
	char color[7];
	for (int i = 0; i < 6; i++) {
		color[i] = command[i];
	}
	color[6] = '\0';

	if (strcmp(color, "color ") == 0) {
		color_command();
		end_check_for_command();
		return;
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
		terminal_writestring("color [bg or/and all] [color] - changes the color of the text");
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
	} else if (strcmp(command, "execute") == 0) {
		execute_text();
	}
	
	
	 else if (strcmp(command, "gamelist") == 0) {
		terminal_writestring("Games:");
		newline();
		terminal_writestring("rock paper scissors - RPS");
		newline();
		terminal_writestring("tic tac toe - TTT");
	} else if (strcmp(command, "color") == 0) {
		terminal_color = vga_entry_color(VGA_COLOR_BLUE, VGA_COLOR_BLACK);

		for (size_t y = 0; y < VGA_HEIGHT; y++) {
			for (size_t x = 0; x < VGA_WIDTH; x++) {
				const size_t index = y * VGA_WIDTH + x;
				terminal_buffer[index] = vga_entry(terminal_buffer[index], terminal_color);
			}
		}

		terminal_writestring("Color changed :)");
	}
	
	
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


// Checks if the key is special (like enter, backspace, etc.)
void keyboard_handler(unsigned char c) {

	if (c == 0) {
	} else if (c == 1){
		if (in_text_editor) {
			in_text_editor = false;	
			check_scroll = true;
			text_editor_exit_flag = true;

			clear_screen();
		}
	} else if (c == 28) { // enter and writing command
		if (in_text_editor) {
			// TODO: make lower lines move down and delete last line, or something like that
		} else if (!is_writing_command) {
			newline();
		} else {
			check_for_command();
		}
	} else if (c == 15) { // tab
		if (in_text_editor) {
			int len = strlen(text_editor_text[terminal_row - 3]);
			if (terminal_column + 4 < 80) {
				terminal_column += 4;
				text_editor_text[terminal_row - 3][len] = ' ';
				text_editor_text[terminal_row - 3][len + 1] = ' ';
				text_editor_text[terminal_row - 3][len + 2] = ' ';
				text_editor_text[terminal_row - 3][len + 3] = ' ';
				text_editor_text[terminal_row - 3][len + 4] = '\0';
			}
		} else if (is_writing_command) {
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
	} else if (c == 42) { // shift pressed TODO: inconsistently works (shift_pressed is true after release of shift)
		if (is_writing_command || in_text_editor) {
			shift_pressed = true;
		}
	} else if (c == 170) { // shift released
		if (is_writing_command || in_text_editor) {
			shift_pressed = false; 
		}
	} else if (c == 58) { // caps lock
		if (is_writing_command || in_text_editor) {
			shift_pressed = !shift_pressed; // TODO: add more elegant solution
		}
	} else if (c == 14) { // backspace
		if (in_text_editor) {
			size_t len = strlen(text_editor_text[terminal_row - 3]);
			if (len > 0 && terminal_column > 3) {
				if (terminal_column < len + 3) {
					text_editor_text[terminal_row - 3][terminal_column - 3] = ' ';
				} else {
					text_editor_text[terminal_row - 3][len - 1] = '\0';
				}
				terminal_column--;
				terminal_putentryat(' ', terminal_color, terminal_column, terminal_row);
			}
		} else if (is_writing_command) {
			size_t len = strlen(command);
			if (len > 0) { // if there is a command being written, delete last character
				if (terminal_column < len + 2) {
					command[terminal_column - 3] = ' ';
				} else {
					command[len - 1] = '\0';
				}
				terminal_column--;
				terminal_putentryat(' ', terminal_color, terminal_column, terminal_row);
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


// Puts a character in the kernel, used by terminal_write
void terminal_putchar(unsigned char c) {
	should_print = true;

	if (is_writing_command || in_text_editor) {
		keyboard_handler(c);
	}

	if (!should_print || c == 0) {
		return;
	}

	if (is_writing_command || in_text_editor) {
		c = kbd_US[c];
	}

	if (shift_pressed && (is_writing_command || in_text_editor)) {
		if (c >= 'a' && c <= 'z') {
			c -= 32;
		}
	}

	if (is_writing_command) { 
		int len = strlen(command);
		if (terminal_column == len + 2) {
			command[len] = c;
			command[len + 1] = '\0';
		} else if (terminal_column < len + 2) {
			command[terminal_column - 2] = c;
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
			text_editor_text[terminal_row - 3][terminal_column - 3] = c;
		}
	}

	terminal_putentryat(c, terminal_color, terminal_column, terminal_row);

	terminal_column++;

	if (terminal_column == VGA_WIDTH) {
		terminal_column = 0;
		terminal_row++;
		
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


// Initializes the PICs
void init_pics(int pic1, int pic2) {
   /* send ICW1 */
   outb(PIC1, ICW1);
   outb(PIC2, ICW1);

   /* send ICW2 */
   outb(PIC1 + 1, pic1);   
   outb(PIC2 + 1, pic2);   

   /* send ICW3 */
   outb(PIC1 + 1, 4);   
   outb(PIC2 + 1, 2);

   /* send ICW4 */
   outb(PIC1 + 1, ICW4);
   outb(PIC2 + 1, ICW4);

   /* disable all IRQs */
   outb(PIC1 + 1, 0xFF);
}


// Moves the cursor to a specific location, used to move cursor when writing commands
void move_cursor(int x, int y) {
    uint16_t pos = y * VGA_WIDTH + x;
    outb(0x3D4, 14);
    outb(0x3D5, pos >> 8);
    outb(0x3D4, 15);
    outb(0x3D5, pos);
}


// idk, does not do what i wanted it to do, but my fans are not going crazy anymore
void idk_what_this_really_does_but_the_loop_does_not_take_as_much_resources_now_it_does_not_pause_though(int ticks) {
	int i;
	for (i = 0; i < ticks; i++) {
		asm volatile("nop");
	}
}


// Starts an input loop, used to get input from the user
void input_loop(bool *exit_flag) {
	unsigned char c = 0;
	while (!(*exit_flag)) { // loop until exit_flag is true
		
		if (inb(0x60) != c) { // if key pressed

			c = inb(0x60);
			if (c > 0) {
				terminal_putchar(kbd_special_characters[c]);
				move_cursor(terminal_column, terminal_row);
			}
		}
		
		idk_what_this_really_does_but_the_loop_does_not_take_as_much_resources_now_it_does_not_pause_though(1000);
	};
}


bool main_exit_flag = false;

// The main function, called at the start of the kernel, calls all the other functions
void kernel_main(void) {
	terminal_initialize();

	// Prints the cat
	terminal_writestring("  _____");
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
	init_pics(0x20, 0x28);
	input_loop(&main_exit_flag);
}