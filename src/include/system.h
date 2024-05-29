#ifndef __SYSTEM_H
#define __SYSTEM_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

/* Kernel */
extern bool in_game;
extern bool is_writing_command;
extern bool check_scroll;
extern bool has_params;
extern bool should_print;
extern size_t terminal_column;
extern size_t terminal_row;
extern uint8_t terminal_color;
extern int timer_ticks;
extern char game_params[20];
extern int game_round;
extern int chosen_color;
extern int chosen_bg_color;
extern const size_t VGA_WIDTH;
extern const size_t VGA_HEIGHT;
extern uint16_t* terminal_buffer;
extern char command[257];
#define MAX_COMMANDS 100
#define MAX_COMMAND_LENGTH 256
extern int at_command;
extern int at_in_command;
extern int command_start_row;
extern char previous_commands[MAX_COMMANDS][MAX_COMMAND_LENGTH];
extern int num_commands;
extern bool rollover;

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

extern void terminal_writestring(const char* data);
extern void newline();
extern void terminal_putentryat(unsigned char c, uint8_t color, size_t x, size_t y);
extern int rand(int something);
extern void move_cursor(int x, int y);
extern void clear_screen();
extern uint8_t vga_entry_color(enum vga_color fg, enum vga_color bg);
extern uint16_t vga_entry(unsigned char uc, uint8_t color);
extern void new_kernel_line();
extern void print_ticks_to_time(int ticks);
extern void sleep(int ticks);
extern size_t strlen(const char* str);
extern void strcpy(char* dest, const char* src);
extern int strcmplen(const char *str1, const char *str2, size_t n);
extern int atoi(const char *str);
extern void itoa(int value, char* str, int base);
extern void terminal_putchar(unsigned char c);
extern void outportb(unsigned short port, unsigned char value);
extern __inline unsigned char inb (unsigned short int port);
extern void add_main_loop_call(void (*func)());
extern void remove_main_loop_call(void (*func)());
extern void* my_malloc(size_t size);
extern void my_free(void* ptr);


/* Minesweeper */
extern bool in_mine_sweeper;

extern void mine_sweeper_start();
extern void mine_sweeper_play(); 

/* Game of life */
extern bool in_game_of_life;

extern void game_of_life_start();
extern void game_of_life_play();
extern void gol_keyboard_handler(c);


/* Pong */
extern bool in_pong;

extern void pong_start();
extern void pong_play();
extern void pong_keyboard_handler(c);


/* Chess */
extern bool in_chess;

extern void chess_start();
extern void chess_play();
extern void chess_keyboard_handler(c);

/* Rock paper scissors */
extern void rock_paper_scissors();


/* Tic tac toe */
extern void tic_tac_toe();


/* Text editor */
extern bool in_text_editor;
extern char text_editor_text[22][77];

extern void text_editor();


/* Execute text */
extern void execute_text();


/* Keyboard handler */
extern bool shift_pressed;
extern bool caps_lock;

extern void keyboard_handler(unsigned char c);


/* Keyboard layouts */
extern char kbd_US[256];
extern char kbd_NO[256];
extern char kbd_special_characters[256];


/* VGA */
extern void vga_enter();
extern void vga_enter_640x480x16();
extern void vga_exit();
extern void draw_rectangle(int x, int y, int width, int height, unsigned short color, bool use_buffer);
extern void draw_circle(int x, int y, int radius, unsigned short color, bool use_buffer);
extern void draw_triangle(int x1, int y1, int x2, int y2, int x3, int y3, unsigned short color, bool use_buffer);
extern void draw_triangle_fill(int x1, int y1, int x2, int y2, int x3, int y3, unsigned short color, bool use_buffer);
extern void vga_clear_screen();
extern void vga_plot_pixel(int x, int y, unsigned short color, bool use_buffer);
extern unsigned short vga_get_pixel_color(int x, int y);
extern void vga_print_frame_buffer();
extern void draw_image(int x, int y, int width, int height, unsigned short *image, bool use_buffer);

/* BGA */
extern void bga_write_register(unsigned short IndexValue, unsigned short DataValue);
extern unsigned short bga_read_register(unsigned short IndexValue);
extern int bga_is_available(void);
extern void bga_set_video_mode(unsigned int Width, unsigned int Height, unsigned int BitDepth, int UseLinearFrameBuffer, int ClearVideoMemory);
extern void bga_set_bank(unsigned short BankNumber);
extern void bga_plot_pixel(int x, int y, unsigned int color, bool use_buffer);
extern void bga_clear_screen();
extern void bga_draw_rectangle(int x, int y, int width, int height, unsigned int color);
extern void bga_draw_circle(int x, int y, int radius, unsigned int color);
extern void bga_draw_triangle(int x1, int y1, int x2, int y2, int x3, int y3, unsigned int color, bool use_buffer);
extern void bga_draw_trapezoid(int x1, int y1, int x2, int y2, int x3, int y3, int x4, int y4, unsigned int color);
extern void bga_print_frame_buffer();

/* 3D test */
extern void enter_3d_test();

/* Calculator */
extern bool in_calculator;
extern void calculator_start();
extern void calculator_calculate();

/* GDT */
extern void gdt_install();

/* IDT */
extern void idt_install();
extern void idt_set_gate(unsigned char num, unsigned long base, unsigned short sel, unsigned char flags);

/* IRQ */
struct regs
{
	unsigned int gs, fs, es, ds;      /* pushed the segs last */
	unsigned int edi, esi, ebp, esp, ebx, edx, ecx, eax;  /* pushed by 'pusha' */
	unsigned int int_no, err_code;    /* our 'push byte #' and ecodes do this */
	unsigned int eip, cs, eflags, useresp, ss;   /* pushed by the processor automatically */
};

extern void irq_install();
extern void irq_install_handler(int irq, void (*handler)(struct regs *r));
extern void irq_uninstall_handler(int irq);
extern void irq_remap();

/* Mouse test */
extern void mouse_test_loop();
extern void mouse_test();

extern bool in_mouse_test;
extern int mouse_x;
extern int mouse_y;

/* Mouse */
extern void mouse_install();
extern void mouse_handler(struct regs *r);
extern void add_mouse_handler(void (*handler)(int8_t mouse_byte[3]));
extern void remove_mouse_handler(void (*handler)(int8_t mouse_byte[3]));
extern void print_mouse(int x, int y);
extern void remove_mouse(int x, int y);

/* Keyboard */
extern void keyboard_install();
extern void keyboard_interrupt_handler(struct regs *r);
extern void add_keyboard_handler(void (*func)(unsigned char c));
extern void remove_keyboard_handler(void (*func)(unsigned char c));	

#endif