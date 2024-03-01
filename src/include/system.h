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


/* Minesweeper */
extern bool in_mine_sweeper;

extern void mine_sweeper_start();
extern void mine_sweeper_play(); 


/* Tower defense */
extern bool in_TD;

extern void tower_defense_start();
extern void tower_defense_play();


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
extern void vga_exit();
extern void draw_rectangle(int x, int y, int width, int height, unsigned short color);
extern void vga_clear_screen();
extern void vga_plot_pixel(int x, int y, unsigned short color);

/* Calculator */
extern bool in_calculator;
extern void calculator_start();
extern void calculator_calculate();

/* GDT */
extern void gdt_install();

/* IDT */
extern void idt_install();
extern void idt_set_gate(unsigned char num, unsigned long base, unsigned short sel, unsigned char flags);

#endif