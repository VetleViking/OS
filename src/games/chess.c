//
// This is the code for a chess game.
// 

#include <system.h> 

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>



bool in_chess = false; // bool to track if in the game

void chess_keyboard_handler(c) {
    if (c == 75) { // left

    } else if (c == 77) { // right

    } else if (c == 72) { // up

	} else if (c == 80) { // down

    } else if (c == 28) { // enter

    } else if (c == 1) { // esc
        in_chess = false;
    }
}

void chess_print_board() {
    draw_rectangle(60, 0, 25, 25, VGA_COLOR_WHITE); // draw a black rectangle
}

void chess_play() {
    while (in_chess) {
        sleep(100); // sleep for 1s

        // do game stuff
    }
}  

void chess_start() {
    in_chess = true;

    vga_enter();
    vga_clear_screen();

    // game specific setup
}



