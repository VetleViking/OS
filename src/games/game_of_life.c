#include <system.h>

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>



bool in_game_of_life = false;
bool game_of_life_running = false;

int gol_x = 150;
int gol_y = 100;

int gol_coords[320][200] = {0};

void plot_gol_square(int x, int y, unsigned short color) {
    draw_rectangle(x, y, 5, 5, color);
}

void plot_gol_square_border(int x, int y, unsigned short color) {
    for (int i = 0; i < 5; i++) {
        vga_plot_pixel(x + i, y, color);
        vga_plot_pixel(x + i, y + 5, color);
        vga_plot_pixel(x, y + i, color);
        vga_plot_pixel(x + 5, y + i, color);
    }
}

void check_under() {
    if (gol_coords[gol_x][gol_y] == 1) {
        plot_gol_square(gol_x, gol_y, VGA_COLOR_WHITE);
    } else {
        plot_gol_square(gol_x, gol_y, VGA_COLOR_BLACK);
    }
}

void gol_keyboard_handler(c) {
    if (c == 75) { // left
        check_under();        
        gol_x -= 5;
        plot_gol_square_border(gol_x, gol_y, VGA_COLOR_LIGHT_GREY);
	} else if (c == 77) { // right
        check_under();        
        gol_x += 5;
        plot_gol_square(gol_x, gol_y, VGA_COLOR_LIGHT_GREY);
	} else if (c == 72) { // up
        check_under();        
        gol_y -= 5;
        plot_gol_square(gol_x, gol_y, VGA_COLOR_LIGHT_GREY);
	} else if (c == 80) { // down
        check_under();        
        gol_y += 5;
        plot_gol_square(gol_x, gol_y, VGA_COLOR_LIGHT_GREY);
    } else if (c == 27) { // escape
        in_game_of_life = false;
        game_of_life_running = false;
        // exit vga mode coming soon
    } else if (c == 13) { // enter
        if (gol_coords[gol_x][gol_y] == 0) {
            gol_coords[gol_x][gol_y] = 1;
            plot_gol_square(gol_x, gol_y, VGA_COLOR_WHITE);
        } else {
            gol_coords[gol_x][gol_y] = 0;
            plot_gol_square(gol_x, gol_y, VGA_COLOR_BLACK);
        }
    }
}

void game_of_life_play() {
    plot_gol_square(gol_x, gol_y, VGA_COLOR_WHITE);

    while (in_game_of_life) {
        sleep(100);
        if (game_of_life_running) {
            // do the game of life stuff
        }

        // do the other game of life stuff
    }
}  

void game_of_life_start() {
    in_game_of_life = true;

    vga_enter();
    vga_clear_screen();
}



