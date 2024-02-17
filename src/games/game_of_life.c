#include <system.h>

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>



bool in_game_of_life = false;
bool game_of_life_running = false;

int gol_x = 150;
int gol_y = 100;

int gol_coords[320][200] = {0};
int new_gol_coords[320][200] = {0};

void plot_gol_square(int x, int y, unsigned short color) {
    draw_rectangle(x, y, 5, 5, color);
}

void plot_gol_square_border(int x, int y, unsigned short color) {
    for (int i = 0; i < 5; i++) {
        vga_plot_pixel(x + i, y, color);
        vga_plot_pixel(x + i, y + 4, color);
        vga_plot_pixel(x, y + i, color);
        vga_plot_pixel(x + 4, y + i, color);
    }
}

void check_under() {
    if (gol_coords[gol_x][gol_y] == 1) {
        plot_gol_square(gol_x, gol_y, VGA_COLOR_WHITE);
    } else {
        plot_gol_square(gol_x, gol_y, VGA_COLOR_BLACK);
    }
}

int check_nearby(int x, int y) {
    int nearby = 0;
    
    int dx, dy;

    for (dx = -5; dx <= 5; dx += 5) {
        for (dy = -5; dy <= 5; dy += 5) {
            if (dx == 0 && dy == 0) continue;
            if (gol_coords[x+dx][y+dy] == 1) {
                nearby++;
            }
        }
    }

    return nearby;
}

void gol_keyboard_handler(c) {
    if (c == 75) { // left
        check_under();        
        gol_x -= 5;
        if (gol_x < 0) gol_x = 315;
        plot_gol_square_border(gol_x, gol_y, VGA_COLOR_LIGHT_BLUE);
	} else if (c == 77) { // right
        check_under();        
        gol_x += 5;
        if (gol_x > 315) gol_x = 0;
        plot_gol_square_border(gol_x, gol_y, VGA_COLOR_LIGHT_BLUE);
	} else if (c == 72) { // up
        check_under();        
        gol_y -= 5;
        if (gol_y < 0) gol_y = 195;
        plot_gol_square_border(gol_x, gol_y, VGA_COLOR_LIGHT_BLUE);
	} else if (c == 80) { // down
        check_under();        
        gol_y += 5;
        if (gol_y > 195) gol_y = 0;
        plot_gol_square_border(gol_x, gol_y, VGA_COLOR_LIGHT_BLUE);
    } else if (c == 1) { // escape
        in_game_of_life = false;
        game_of_life_running = false;
        in_game = false;
        vga_exit();
    } else if (c == 28) { // enter
        if (gol_coords[gol_x][gol_y] == 0) {
            gol_coords[gol_x][gol_y] = 1;
            plot_gol_square(gol_x, gol_y, VGA_COLOR_WHITE);
            plot_gol_square_border(gol_x, gol_y, VGA_COLOR_LIGHT_BLUE);
        } else {
            gol_coords[gol_x][gol_y] = 0;
            plot_gol_square(gol_x, gol_y, VGA_COLOR_BLACK);
            plot_gol_square_border(gol_x, gol_y, VGA_COLOR_LIGHT_BLUE);
        }
    } else if (c == 57) { // space
        game_of_life_running = !game_of_life_running;
    } else if (c == 46) { // c
        vga_clear_screen();
        for (int i = 0; i < 320; i = i + 5) {
            for (int j = 0; j < 200; j = j + 5) {
                gol_coords[i][j] = 0;
                new_gol_coords[i][j] = 0;
            }
        }
        plot_gol_square_border(gol_x, gol_y, VGA_COLOR_LIGHT_BLUE);
    }
}

void game_of_life_play() {
    plot_gol_square_border(gol_x, gol_y, VGA_COLOR_LIGHT_BLUE);

    while (in_game_of_life) {
        sleep(20);
        if (game_of_life_running) {
            for (int i = 0; i < 320; i = i + 5) {
                for (int j = 0; j < 200; j = j + 5) {
                    int nearby = 0;
                    nearby = check_nearby(i, j);

                    if (nearby < 2 && gol_coords[i][j] == 1) {
                        new_gol_coords[i][j] = 0;
                    } else if ((nearby == 2 || nearby == 3) && gol_coords[i][j] == 1) {
                        new_gol_coords[i][j] = 1;
                    } else if (nearby > 3 && gol_coords[i][j] == 1) {
                        new_gol_coords[i][j] = 0;
                    } else if (nearby == 3 && gol_coords[i][j] == 0) {
                        new_gol_coords[i][j] = 1;
                    }
                }
            }

            for (int i = 0; i < 320; i = i + 5) {
                for (int j = 0; j < 200; j = j + 5) {
                    gol_coords[i][j] = new_gol_coords[i][j];
                    
                    if (gol_coords[i][j] == 1) {
                        plot_gol_square(i, j, VGA_COLOR_WHITE);
                    } else {
                        plot_gol_square(i, j, VGA_COLOR_BLACK);
                    }

                    if (i == gol_x && j == gol_y) {
                        plot_gol_square_border(i, j, VGA_COLOR_LIGHT_BLUE);
                    }
                }
            }            
        }

        // do the other game of life stuff
    }
}  

void game_of_life_start() {
    in_game_of_life = true;

    vga_enter();
    vga_clear_screen();
}



