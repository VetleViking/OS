#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#define INT_DISABLE 0
#define INT_ENABLE  0x200
#define PIC1 0x20
#define PIC2 0xA0

#define ICW1 0x11
#define ICW4 0x01

static bool shift_pressed = false;

char kbd_US [128] =
{
    0,  27, '1', '2', '3', '4', '5', '6', '7', '8', '9', '0', '-', '=', '\b',   
  '\t', /* <-- Tab */
  'q', 'w', 'e', 'r', 't', 'y', 'u', 'i', 'o', 'p', '[', ']', '\n',     
   	0, /* <-- control key */
  'a', 's', 'd', 'f', 'g', 'h', 'j', 'k', 'l', ';', '\'', '`',  42, '\\', 'z', 'x', 'c', 'v', 'b', 'n', 'm', ',', '.', '/',   0,
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
    0,  /* All other keys are undefined */
};



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
 
static inline uint8_t vga_entry_color(enum vga_color fg, enum vga_color bg) 
{
	return fg | bg << 4;
}
 
static inline uint16_t vga_entry(unsigned char uc, uint8_t color) 
{
	return (uint16_t) uc | (uint16_t) color << 8;
}
 
size_t strlen(const char* str) 
{
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
 
void terminal_setcolor(uint8_t color) {
	terminal_color = color;
}
 
void terminal_putentryat(unsigned char c, uint8_t color, size_t x, size_t y) {
	const size_t index = y * VGA_WIDTH + x;
	terminal_buffer[index] = vga_entry(c, color);
}

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

void newline() {
	terminal_row++;
	terminal_column = 0;

	check_kernel_scroll();
}

bool should_print = true;
bool is_writing_command = false;

void new_kernel_line() {
	newline();
	is_writing_command = false;
	terminal_writestring("> ");
	is_writing_command = true;
}

char* command = "";

void check_for_command() {
	newline();
	terminal_writestring(command);

	command[0] = '\0';
	new_kernel_line();
}



void keyboard_handler(unsigned char c) {
	if (c == 0) {
	} else if (c == '\n' && is_writing_command) {
		check_for_command();
	} else if (c == '\n' && !is_writing_command) {
		newline();
	} else if (c == '\t') {
		terminal_column += 4;
	} else if (c == 42) {
		shift_pressed = true;
	} else if (c == 42 + 0x80) {
		shift_pressed = false;
	}
	
	else {
		should_print = true;
		return;
	}
	should_print = false;
	return;
}

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
		command[len] = c;
		command[len + 1] = '\0';		
	}

	terminal_putentryat(c, terminal_color, terminal_column, terminal_row);

	if (++terminal_column == VGA_WIDTH) {
		terminal_column = 0;
		if (terminal_row++ == VGA_HEIGHT)
			terminal_row = 0;
	}

	check_kernel_scroll();
}


void terminal_write(const char* data, size_t size) {
	for (size_t i = 0; i < size; i++)
		terminal_putchar(data[i]);
}
 
void terminal_writestring(const char* data) {
	terminal_write(data, strlen(data));
}


void outb( unsigned short port, unsigned char val ) {
   asm volatile("outb %0, %1" : : "a"(val), "Nd"(port) );
}

static __inline unsigned char inb (unsigned short int port) {
  unsigned char _v;

  __asm__ __volatile__ ("inb %w1,%0":"=a" (_v):"Nd" (port));
  return _v;
}

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

void move_cursor(int x, int y) {
    uint16_t pos = y * VGA_WIDTH + x;
    outb(0x3D4, 14);
    outb(0x3D5, pos >> 8);
    outb(0x3D4, 15);
    outb(0x3D5, pos);
}

void idk_what_this_really_does_but_the_loop_does_not_take_as_much_resources_now_it_does_not_pause_though(int ticks) {
	int i;
	for (i = 0; i < ticks; i++) {
		asm volatile("nop");
	}
}

void kernel_main(void) {

	terminal_initialize();
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


	is_writing_command = true;
	move_cursor(terminal_column, terminal_row);
	unsigned char c = 0;
	init_pics(0x20, 0x28);

	while (c != 1) { // ESC to exit writing loop (does not work yet)
		
		if (inb(0x60) != c) { // if key pressed

			if (c == 42 + 0x80) {
				shift_pressed = false;
			}

			c = inb(0x60);
			if (c > 0) {
				terminal_putchar(kbd_US[c]);
				move_cursor(terminal_column, terminal_row);
			}
		}
		
		idk_what_this_really_does_but_the_loop_does_not_take_as_much_resources_now_it_does_not_pause_though(1000);
	};
}