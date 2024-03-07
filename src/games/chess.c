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
        if (cursor_pos[0] > 0) {
            remove_cursor();
            cursor_pos[0]--;
            print_cursor();
        }
    } else if (c == 77) { // right
        if (cursor_pos[0] < 7) {
            remove_cursor();
            cursor_pos[0]++;
            print_cursor();
        }
    } else if (c == 72) { // up
        if (cursor_pos[1] > 0) {
            remove_cursor();
            cursor_pos[1]--;
            print_cursor();
        }
	} else if (c == 80) { // down
        if (cursor_pos[1] < 7) {
            remove_cursor();
            cursor_pos[1]++;
            print_cursor();
        }
    } else if (c == 28) { // enter

    } else if (c == 1) { // esc
        in_chess = false;
    }
}

void draw_piece(int x, int y, char piece) {
    // draw piece (coming soon)
    // remember to add 60 to x and multiply by 25 on both
}

void print_cursor() {
    draw_rectangle(60 + cursor_pos[0] * 25, cursor_pos[1] * 25, 25, 25, VGA_COLOR_RED);
    int color = (cursor_pos[0] + cursor_pos[1]) % 2 == 0 ? VGA_COLOR_WHITE : VGA_COLOR_DARK_GREY;
    draw_rectangle(63 + cursor_pos[0] * 25, 3 + cursor_pos[1] * 25, 19, 19, color);

    if (chess_board[cursor_pos[1]][cursor_pos[0]] != 0) {
        draw_piece(cursor_pos[0], cursor_pos[1], chess_board[cursor_pos[1]][cursor_pos[0]]);
    }
}

void remove_cursor() {
    int color = (cursor_pos[0] + cursor_pos[1]) % 2 == 0 ? VGA_COLOR_WHITE : VGA_COLOR_DARK_GREY;
    draw_rectangle(60 + cursor_pos[0] * 25, cursor_pos[1] * 25, 25, 25, color);
    if (chess_board[cursor_pos[1]][cursor_pos[0]] != 0) {
        draw_piece(cursor_pos[0], cursor_pos[1], chess_board[cursor_pos[1]][cursor_pos[0]]);
    }
}

void chess_print_board() {
    for (int i = 60; i < 260; i += 50) {
        for (int j = 0; j < 200; j += 50) {
            draw_rectangle(i, j, 25, 25, VGA_COLOR_WHITE);
            draw_rectangle(i + 25, j, 25, 25, VGA_COLOR_DARK_GREY);
            draw_rectangle(i + 25, j + 25, 25, 25, VGA_COLOR_WHITE);
            draw_rectangle(i, j + 25, 25, 25, VGA_COLOR_DARK_GREY);
        }
    }

    for (int i = 0; i < 8; i++) {
        for (int j = 0; j < 8; j++) {
            if (chess_board[i][j] != 0) {
                draw_piece(j, i, chess_board[i][j]);
            }
        }
    }

    print_cursor();
}

void chess_play() {
    while (in_chess) {
        sleep(100); // sleep for 1s

        // this is just so that other stuff wont be running
        // may be adding a clock or something later, that will go here.
    }
}  

void chess_start() {
    in_chess = true;

    vga_enter();
    vga_clear_screen();


    cursor_pos[0] = 0;
    cursor_pos[1] = 7;
    chess_print_board();
}



