#include <system.h>

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>




bool in_game_of_life = false;

int gol_x = 150;
int gol_y = 100;


void game_of_life_play() {
    vga_plot_pixel(150, 100, VGA_COLOR_RED);

    while (in_game_of_life) {
        sleep(100);
        
        // do the game of life stuff
    }
}  

void game_of_life_start() {
    in_game_of_life = true;

    vga_enter();
    vga_clear_screen();

    
	// draw rectangle
	draw_rectangle(150, 10, 100, 50, VGA_COLOR_GREEN);
}

void gol_keyboard_handler(c) {
    if (c == 75) { // left
		vga_plot_pixel(gol_x, gol_y, VGA_COLOR_BLACK);
        gol_x -= 10;
        vga_plot_pixel(gol_x, gol_y, VGA_COLOR_RED);
	} else if (c == 77) { // right
		vga_plot_pixel(gol_x, gol_y, VGA_COLOR_BLACK);
        gol_x += 10;
        vga_plot_pixel(gol_x, gol_y, VGA_COLOR_RED);
	} else if (c == 72) { // up
		vga_plot_pixel(gol_x, gol_y, VGA_COLOR_BLACK);
        gol_y -= 10;
        vga_plot_pixel(gol_x, gol_y, VGA_COLOR_RED);
	} else if (c == 80) { // down
		vga_plot_pixel(gol_x, gol_y, VGA_COLOR_BLACK);
        gol_y += 10;
        vga_plot_pixel(gol_x, gol_y, VGA_COLOR_RED);
	}
}