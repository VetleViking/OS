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

struct chess_piece_moves {
    bool straight_or_diagonal [2];
    int moves [8][2];
    bool is_pawn;
    bool is_knight;
};

struct chess_piece_moves p = {
    .straight_or_diagonal = {false, false},
    .moves = {{0, 1}, {0, 2}},
    .is_pawn = true,
    .is_knight = false
};

struct chess_piece_moves r = {
    .straight_or_diagonal = {true, false},
    .moves = {0},
    .is_pawn = false,
    .is_knight = false
};

struct chess_piece_moves n = {
    .straight_or_diagonal = {false, false},
    .moves = {{1, 2}, {2, 1}, {2, -1}, {1, -2}, {-1, -2}, {-2, -1}, {-2, 1}, {-1, 2}},
    .is_pawn = false,
    .is_knight = true
};

struct chess_piece_moves b = {
    .straight_or_diagonal = {false, true},
    .moves = {0},
    .is_pawn = false,
    .is_knight = false
};

struct chess_piece_moves q = {
    .straight_or_diagonal = {true, true},
    .moves = {0},
    .is_pawn = false,
    .is_knight = false
};

struct chess_piece_moves k = {
    .straight_or_diagonal = {true, true},
    .moves = {{0, 1}, {1, 1}, {1, 0}, {1, -1}, {0, -1}, {-1, -1}, {-1, 0}, {-1, 1}},
    .is_pawn = false,
    .is_knight = false
};

struct chess_piece_moves* find_piece(char piece) {
    switch (piece) {
        case 'p':
            return &p;
        case 'r':
            return &r;
        case 'n':
            return &n;
        case 'b':
            return &b;
        case 'q':
            return &q;
        case 'k':
            return &k;
        default:
            return NULL;
    }
}

int cursor_pos[2] = {0, 0};

int color_light = VGA_COLOR_LIGHT_GREY;
int color_dark = VGA_COLOR_DARK_GREY;

bool chosen_piece = false;

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
        if (chess_board[cursor_pos[1]][cursor_pos[0]] != 0 && !chosen_piece) {
            possible_moves(cursor_pos[0], cursor_pos[1]);
            chosen_piece = true;
        } else {
            // move piece 
            chosen_piece = false;
        }
    } else if (c == 1) { // esc
        in_chess = false;
    }
}

void possible_moves(int x, int y) {
    char piece = chess_board[y][x];
    bool is_white = false;

    if (piece < 97) { // TODO: exactly every other time, the piece is showing the wrong way if its white
        piece += 32;
        is_white = true;
    }

    struct chess_piece_moves* moves = find_piece(piece);

    if (moves == NULL) {
        draw_rectangle(0, 0, 25, 25, VGA_COLOR_GREEN);
        return;
    }

    if (is_white && moves->is_pawn) { // if the piece is white, we need to reverse the moves for pawn
        for (int i = 0; i < 8; i++) {
            for (int j = 0; j < 2; j++) {
                moves->moves[i][j] *= -1;
            }
        }
    }

    for (int i = 0; i < 8; i++) {
        if (moves->moves[i][0] == 0 && moves->moves[i][1] == 0) {
            break;
        }

        int new_x = x + moves->moves[i][0];
        int new_y = y + moves->moves[i][1];

        if (new_x < 0 || new_x > 7 || new_y < 0 || new_y > 7) {
            continue;
        }

        if (chess_board[new_y][new_x] == 0) {
            draw_rectangle(60 + new_x * 25, new_y * 25, 25, 25, VGA_COLOR_GREEN);
        } else {
            draw_rectangle(60 + new_x * 25, new_y * 25, 25, 25, VGA_COLOR_RED);
        }
    }
}

void draw_piece(int x, int y, char piece) {
    int color = VGA_COLOR_BLACK;

    if (piece < 97) {
        piece += 32;
        color = VGA_COLOR_WHITE;
    }

 
    if (piece == 'p') { // pawn
        draw_rectangle(63 + x * 25, 18 + y * 25, 19, 4, color == VGA_COLOR_BLACK ? VGA_COLOR_WHITE : VGA_COLOR_BLACK);
        draw_rectangle(64 + x * 25, 19 + y * 25, 17, 2, color);
    } else if (piece == 'r') { // rook
        draw_rectangle(63 + x * 25, 15 + y * 25, 19, 7, color == VGA_COLOR_BLACK ? VGA_COLOR_WHITE : VGA_COLOR_BLACK);
        draw_rectangle(64 + x * 25, 16 + y * 25, 17, 5, color);
    } else if (piece == 'n') { // knight
        draw_rectangle(63 + x * 25, 12 + y * 25, 19, 10, color == VGA_COLOR_BLACK ? VGA_COLOR_WHITE : VGA_COLOR_BLACK);
        draw_rectangle(64 + x * 25, 13 + y * 25, 17, 8, color);
    } else if (piece == 'b') { // bishop
        draw_rectangle(63 + x * 25, 9 + y * 25, 19, 13, color == VGA_COLOR_BLACK ? VGA_COLOR_WHITE : VGA_COLOR_BLACK);
        draw_rectangle(64 + x * 25, 10 + y * 25, 17, 11, color);
    } else if (piece == 'q') { // queen
        draw_rectangle(63 + x * 25, 6 + y * 25, 19, 16, color == VGA_COLOR_BLACK ? VGA_COLOR_WHITE : VGA_COLOR_BLACK);
        draw_rectangle(64 + x * 25, 7 + y * 25, 17, 14, color);
    } else if (piece == 'k') { // king
        draw_rectangle(63 + x * 25, 3 + y * 25, 19, 19, color == VGA_COLOR_BLACK ? VGA_COLOR_WHITE : VGA_COLOR_BLACK);
        draw_rectangle(64 + x * 25, 4 + y * 25, 17, 17, color);
    }
}

void print_cursor() {
    draw_rectangle(60 + cursor_pos[0] * 25, cursor_pos[1] * 25, 25, 25, VGA_COLOR_RED);
    int color = (cursor_pos[0] + cursor_pos[1]) % 2 == 0 ? color_light : color_dark;
    draw_rectangle(63 + cursor_pos[0] * 25, 3 + cursor_pos[1] * 25, 19, 19, color);

    if (chess_board[cursor_pos[1]][cursor_pos[0]] != 0) {
        draw_piece(cursor_pos[0], cursor_pos[1], chess_board[cursor_pos[1]][cursor_pos[0]]);
    }
}

void remove_cursor() {
    int color = (cursor_pos[0] + cursor_pos[1]) % 2 == 0 ? color_light : color_dark;
    draw_rectangle(60 + cursor_pos[0] * 25, cursor_pos[1] * 25, 25, 25, color);
    if (chess_board[cursor_pos[1]][cursor_pos[0]] != 0) {
        draw_piece(cursor_pos[0], cursor_pos[1], chess_board[cursor_pos[1]][cursor_pos[0]]);
    }
}

void chess_print_board() {
    for (int i = 60; i < 260; i += 50) {
        for (int j = 0; j < 200; j += 50) {
            draw_rectangle(i, j, 25, 25, color_light);
            draw_rectangle(i + 25, j, 25, 25, color_dark);
            draw_rectangle(i + 25, j + 25, 25, 25, color_light);
            draw_rectangle(i, j + 25, 25, 25, color_dark);
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



