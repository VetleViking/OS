#include <system.h>

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>



bool in_pong = false;


// this function will draw the number at the given x and y coordinates
// the number is 40 pixels high and 25 pixels wide
void print_number(int x, int y, int number) {
    switch (number) {
        case 0:
            draw_rectangle(x, y, 25, 40, VGA_COLOR_WHITE);
            draw_rectangle(x + 3, y + 3, 19, 34, VGA_COLOR_BLACK);            
            break;

        case 1:
            draw_rectangle(x + 37, y, 3, 40, VGA_COLOR_WHITE);            
            break;

        case 2:
            draw_rectangle(x, y, 25, 3, VGA_COLOR_WHITE);
            draw_rectangle(x + 22, y, 3, 20, VGA_COLOR_WHITE);
            draw_rectangle(x, y + 19, 25, 3, VGA_COLOR_WHITE);
            draw_rectangle(x, y + 20, 3, 20, VGA_COLOR_WHITE);
            draw_rectangle(x, y + 37, 25, 3, VGA_COLOR_WHITE);
            break;

        case 3:
            draw_rectangle(x, y, 25, 3, VGA_COLOR_WHITE);
            draw_rectangle(x, y + 19, 25, 3, VGA_COLOR_WHITE);
            draw_rectangle(x, y + 37, 25, 3, VGA_COLOR_WHITE);
            draw_rectangle(x + 22, y, 3, 40, VGA_COLOR_WHITE);
            break;

        case 4: 
            draw_rectangle(x, y, 3, 20, VGA_COLOR_WHITE);
            draw_rectangle(x + 22, y, 3, 40, VGA_COLOR_WHITE);
            draw_rectangle(x, y + 19, 25, 3, VGA_COLOR_WHITE);
            break;

        case 5:
            draw_rectangle(x, y, 25, 3, VGA_COLOR_WHITE);
            draw_rectangle(x, y, 3, 20, VGA_COLOR_WHITE);
            draw_rectangle(x, y + 19, 25, 3, VGA_COLOR_WHITE);
            draw_rectangle(x + 22, y + 20, 3, 20, VGA_COLOR_WHITE);
            draw_rectangle(x, y + 37, 25, 3, VGA_COLOR_WHITE);
            break;

        case 6:
            draw_rectangle(x, y, 25, 3, VGA_COLOR_WHITE);
            draw_rectangle(x, y, 3, 40, VGA_COLOR_WHITE);
            draw_rectangle(x, y + 19, 25, 3, VGA_COLOR_WHITE);
            draw_rectangle(x + 22, y + 20, 3, 20, VGA_COLOR_WHITE);
            draw_rectangle(x, y + 37, 25, 3, VGA_COLOR_WHITE);
            break;

        case 7:
            draw_rectangle(x, y, 25, 3, VGA_COLOR_WHITE);
            draw_rectangle(x + 22, y, 3, 40, VGA_COLOR_WHITE);
            break;

        case 8:
            draw_rectangle(x, y, 25, 40, VGA_COLOR_WHITE);
            draw_rectangle(x + 3, y + 3, 19, 34, VGA_COLOR_BLACK); 
            draw_rectangle(x, y + 19, 25, 3, VGA_COLOR_WHITE);
            break;

        case 9:
            draw_rectangle(x, y, 25, 3, VGA_COLOR_WHITE);
            draw_rectangle(x + 22, y, 3, 40, VGA_COLOR_WHITE);
            draw_rectangle(x, y + 19, 25, 3, VGA_COLOR_WHITE);
            draw_rectangle(x, y, 3, 20, VGA_COLOR_WHITE);
            draw_rectangle(x, y + 37, 25, 3, VGA_COLOR_WHITE);
            break;
    };
}


void pong_make_map() {
    for (int i = 5; i < 200; i += 15) {
        draw_rectangle(159, i, 3, 10, VGA_COLOR_WHITE);
    }

    draw_rectangle(10, 80, 3, 40, VGA_COLOR_WHITE);

    draw_rectangle(310, 80, 3, 40, VGA_COLOR_WHITE);

    print_number(125, 5, 9);
}

void pong_keyboard_handler(c) {
    if (c == 72) { // up

	} else if (c == 80) { // down

    }
}

void pong_play() {
    while (in_pong) {
        sleep(100); // sleep for 1s
        // do
    }
}  

void pong_start() {
    in_pong = true;

    vga_enter();
    vga_clear_screen();
    pong_make_map();
}



