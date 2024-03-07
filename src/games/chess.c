//
// This is the code for a chess game.
// 

#include <system.h> 

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>



bool in_chess = false; // bool to track if in the game

char chess_board[8][8] = {
    {'r', 'n', 'b', 'q', 'k', 'b', 'n', 'r'},
    {'p', 'p', 'p', 'p', 'p', 'p', 'p', 'p'},
    {0},
    {0},
    {0},
    {0},
    {'P', 'P', 'P', 'P', 'P', 'P', 'P', 'P'},
    {'R', 'N', 'B', 'Q', 'K', 'B', 'N', 'R'}
};

int cursor_pos[2] = {0, 0};


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

void print_cursor() {
    draw_rectangle(60 + cursor_pos[0] * 25, cursor_pos[1] * 25, 25, 25, VGA_COLOR_RED);
}

void chess_print_board() {
    for (int i = 60; i < 260; i += 50) {
        for (int j = 0; j < 200; j += 50) {
            draw_rectangle(i, j, 25, 25, VGA_COLOR_WHITE);
            draw_rectangle(i + 25, j + 25, 25, 25, VGA_COLOR_WHITE);
        }
    }

    for (int i = 0; i < 8; i++) {
        for (int j = 0; j < 8; j++) {
            if (chess_board[i][j] != 0) {
                // draw piece (coming soon)
            }
        }
    }
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

    chess_print_board();
}



