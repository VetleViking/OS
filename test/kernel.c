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
char kbd_US [256] =
{
    0,  27, '1', '2', '3', '4', '5', '6', '7', '8', '9', '0', '-', '=', '\b', /* <-- Backspace */   
  '\t', /* <-- Tab */
  'q', 'w', 'e', 'r', 't', 'y', 'u', 'i', 'o', 'p', '[', ']', '\n', /* <-- Enter key */     
   	0, /* <-- control key */
  'a', 's', 'd', 'f', 'g', 'h', 'j', 'k', 'l', ';', '\'', '`',  42, /* <-- left shift */ '\\', 'z', 'x', 'c', 'v', 'b', 'n', 'm', ',', '.', '/',   54, /* <-- right shift */
  '*',
    0,  /* Alt */
  ' ',  /* Space bar */
    58,  /* Caps lock */
    0,  /* 59 - F1 key ... > */
    0,   0,   0,   0,   0,   0,   0,   0,
    0,  /* < ... F10 */
    0,  /* 69 - Num lock*/
    0,  /* Scroll Lock */
    0,  /* Home key */
   72,  /* Up Arrow */
    0,  /* Page Up */
  '-',
   75,  /* Left Arrow */
    0,
   77,  /* Right Arrow */
  '+',
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


// Copies a string from src to dest
void strcpy(char* dest, const char* src) {
    int i = 0;
    while (src[i] != '\0') {
        dest[i] = src[i];
        i++;
    }
    dest[i] = '\0';
}


#define MAX_COMMANDS 100
#define MAX_COMMAND_LENGTH 256

char previous_commands[MAX_COMMANDS][MAX_COMMAND_LENGTH];
int num_commands = 0;
int at_command = 0;
char command[MAX_COMMAND_LENGTH];


// Rock, paper, scissors game (the pc always wins)
void rock_paper_scissors() {
	if (strcmp(command, "game") == 0) {
		terminal_writestring("Lets play a game!\n");
		terminal_writestring("Rock, paper, scissors, you go first!\n");
		terminal_writestring("Type rock, paper or scissors");
		return;
	} else if (strcmp(command, "rock") == 0) {
		terminal_writestring("I choose paper, You lose!");
		return;
	} else if (strcmp(command, "paper") == 0) {
		terminal_writestring("I choose scissors, You lose!");
		return;
	} else if (strcmp(command, "scissors") == 0) {
		terminal_writestring("I choose rock, You lose!");
		return;
	} else {
		terminal_writestring("That is not a valid move, You lose!");
		return;
	}
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
	 if (strcmp(command, "game") == 0 || strcmp(previous_commands[num_commands - 2], "game") == 0) {
		rock_paper_scissors();
	} else if (strcmp(command, "clear") == 0) {
		for (size_t y = 0; y < VGA_HEIGHT; y++) {
			for (size_t x = 0; x < VGA_WIDTH; x++) {
				const size_t index = y * VGA_WIDTH + x;
				terminal_buffer[index] = vga_entry(' ', terminal_color);
			}
		}
		terminal_row = 0;
		terminal_column = 0;
	} else if (strcmp(command, "help") == 0) {
		terminal_writestring("Commands:\n");
		terminal_writestring("clear - clears the screen\n");
		terminal_writestring("help - shows this message\n");
		terminal_writestring("cat - prints the cat\n");	
		terminal_writestring("game - plays a game of rock, paper, scissors \n");
		terminal_writestring("meow - prints meow :3");
	} else if (strcmp(command, "cat") == 0) {
		terminal_writestring("  _____\n");
		terminal_writestring(" |     |___ ___ _ _ _                 |\\__/,|   (`\\\n");
		terminal_writestring(" | | | | -_| . | | | |              _.|o o  |_   ) )\n");
		terminal_writestring(" |_|_|_|___|___|_____|             -(((---(((--------");
	}  else if (strcmp(command, "meow") == 0) {
		terminal_writestring("meow! :3");
	}
	
	else {
		terminal_writestring("Unknown command: ");
		terminal_writestring(command);
	}
	is_writing_command = true;

	at_command = num_commands;
	command[0] = '\0';
	new_kernel_line();
}


// Checks if the key is special (like enter, backspace, etc.)
void keyboard_handler(unsigned char c) {
	if (c == 0) {} else if (c == '\n' && is_writing_command) { // enter and writing command
		check_for_command();
	} else if (c == '\n' && !is_writing_command) { // enter and not writing command
		newline();
	} else if (c == '\t') { // tab
		terminal_column += 4; 
	} else if ((c == 42 || c == 54) && is_writing_command) { // shift pressed TODO: inconsistently works (shift_pressed is true after release of shift)
		shift_pressed = true; 
	} else if ((c == 170 || c == 182) && is_writing_command) { // shift released
		shift_pressed = false; 
	} else if (c == 58 && is_writing_command) { // caps lock
		shift_pressed = !shift_pressed; // TODO: add more elegant solution later
	} else if (c == '\b') { // backspace
		size_t len = strlen(command);
		if (len > 0) { // if there is a command being written, delete last character
			command[len - 1] = '\0';
			terminal_column--;
			terminal_putentryat(' ', terminal_color, terminal_column, terminal_row);
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
            terminal_writestring(previous_commands[at_command]); // write command
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
		         
			if (!(at_command == num_commands - 1)) { 
				terminal_writestring(previous_commands[at_command]); // if not last and empty command, write command
			}
        }
	} else if (c == 75) { // left arrow pressed
		if (terminal_column > 2 && is_writing_command) {
			terminal_column--;
		}
	} else if (c == 77) { // right arrow pressed
		if (terminal_column < strlen(command) && is_writing_command) {
			terminal_column++;
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
	keyboard_handler(c);

	if (!should_print) {
		return;
	}
	
	if (shift_pressed) {
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

	terminal_putentryat(c, terminal_color, terminal_column, terminal_row);

	if (++terminal_column == VGA_WIDTH) {
		terminal_column = 0;
		if (terminal_row++ == VGA_HEIGHT)
			terminal_row = 0;
	}

	check_kernel_scroll();
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
void input_loop() {
	unsigned char c = 0;
	while (c != 1) { // ESC to exit writing loop (does not work yet, at least i think so)
		
		if (inb(0x60) != c) { // if key pressed

			c = inb(0x60);
			if (c > 0) {
				terminal_putchar(kbd_US[c]);
				move_cursor(terminal_column, terminal_row);
			}
		}
		
		idk_what_this_really_does_but_the_loop_does_not_take_as_much_resources_now_it_does_not_pause_though(1000);
	};
}



// The main function, called at the start of the kernel, calls all the other functions
void kernel_main(void) {

	terminal_initialize();

	// Prints the cat
	terminal_writestring("  _____\n");
    terminal_writestring(" |     |___ ___ _ _ _                 |\\__/,|   (`\\\n");
    terminal_writestring(" | | | | -_| . | | | |              _.|o o  |_   ) )\n");
    terminal_writestring(" |_|_|_|___|___|_____|             -(((---(((--------\n");


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
	input_loop();
}