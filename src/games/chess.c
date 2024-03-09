//
// This is the code for a chess game.
// 

#include <system.h> 

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>



bool in_chess = false;

char chess_board[8][8] = {
    {'r', 'n', 'b', 'q', 'k', 'b', 'n', 'r'},
    {'p', 'p', 'p', 'p', 'p', 'p', 'p', 'p'},
    {0},
    {0, 'p'},
    {'P', 0, 'Q'},
    {0},
    {'P', 'P', 'P', 'P', 'P', 'P', 'P', 'P'},
    {'R', 'N', 'B', 'Q', 'K', 'B', 'N', 'R'}
};

int possible_moves_pos[64][2] = {0};
int len_pm_pos = 0;

struct chess_piece_moves {
    bool straight_or_diagonal [2];
    int moves [8][2];
    bool is_pawn;

};

struct chess_piece_moves p = {
    .straight_or_diagonal = {false, false},
    .moves = {0},
    .is_pawn = true 
};

struct chess_piece_moves r = {
    .straight_or_diagonal = {true, false},
    .moves = {0},
    .is_pawn = false 
};

struct chess_piece_moves n = {
    .straight_or_diagonal = {false, false},
    .moves = {{1, 2}, {2, 1}, {2, -1}, {1, -2}, {-1, -2}, {-2, -1}, {-2, 1}, {-1, 2}},
    .is_pawn = false 
};

struct chess_piece_moves b = {
    .straight_or_diagonal = {false, true},
    .moves = {0},
    .is_pawn = false 
};

struct chess_piece_moves q = {
    .straight_or_diagonal = {true, true},
    .moves = {0},
    .is_pawn = false 
};

struct chess_piece_moves k = {
    .straight_or_diagonal = {false, false},
    .moves = {{0, 1}, {1, 1}, {1, 0}, {1, -1}, {0, -1}, {-1, -1}, {-1, 0}, {-1, 1}},
    .is_pawn = false 
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
int chosen_piece_pos [2] = {0};

bool white_turn = true;

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
            if ((chess_board[cursor_pos[1]][cursor_pos[0]] < 97 && white_turn) || (chess_board[cursor_pos[1]][cursor_pos[0]] >= 97) && !white_turn) {
                len_pm_pos = 0;
                
                possible_moves(cursor_pos[0], cursor_pos[1]);
                
                chosen_piece_pos[0] = cursor_pos[0];
                chosen_piece_pos[1] = cursor_pos[1];
                chosen_piece = true;
            }
        } else if (chosen_piece) {
            for (int i = 0; i < len_pm_pos; i++) {
                if (possible_moves_pos[i][0] == cursor_pos[0] && possible_moves_pos[i][1] == cursor_pos[1]) {
                    chess_board[cursor_pos[1]][cursor_pos[0]] = chess_board[chosen_piece_pos[1]][chosen_piece_pos[0]];
                    chess_board[chosen_piece_pos[1]][chosen_piece_pos[0]] = 0;
                    white_turn = !white_turn;
                    break;
                }
            }
            chosen_piece = false;
            chess_print_board();
        }
    } else if (c == 1) { // esc
        in_chess = false;
        vga_exit(); // not working
    }
}

bool is_possible_move(int x, int y, bool is_white) {
    if (chess_board[y][x] != 0) {
        if ((is_white && chess_board[y][x] >= 97) || (!is_white && chess_board[y][x] < 97)) {    
            draw_rectangle(60 + x * 25, (y) * 25, 25, 25, VGA_COLOR_RED);
            possible_moves_pos[len_pm_pos][0] = x;
            possible_moves_pos[len_pm_pos][1] = y;
            len_pm_pos++;
        }
        // not quite true, but need it like this other places
        // does not seem to interfere with anything, so will keep it like it is for now
        return false; 
    } else {
        draw_rectangle(60 + x * 25, (y) * 25, 25, 25, VGA_COLOR_GREEN);
    }
    possible_moves_pos[len_pm_pos][0] = x;
    possible_moves_pos[len_pm_pos][1] = y;
    len_pm_pos++;
    return true;
}


void possible_moves(int x, int y) {
    char piece = chess_board[y][x];
    bool is_white = !(piece >= 'a' && piece <= 'z');

    if (is_white) { // makes all pieces lowercase
        piece += ('a' - 'A');
    }

    struct chess_piece_moves* moves = find_piece(piece);

    if (moves == NULL) {
        draw_rectangle(0, 0, 25, 25, VGA_COLOR_GREEN);
        return;
    }

    for (int i = 0; i < 8; i++) {
        if (moves->moves[i][0] == 0 && moves->moves[i][1] == 0) {
            break;
        }

        int new_x = x + moves->moves[i][0];
        int new_y = y + moves->moves[i][1];

        if ((new_x < 8 && new_x >= 0) && (new_y < 8 && new_y >= 0)) {
            is_possible_move(new_x, new_y, is_white);
        }
    }

    // bcause the pawn is a goofy goober, it needs some special treatment
    // some errors with edges of screen will fix later
    // will need to add un passant and changing at top of board
    if (moves->is_pawn) {
        if (is_white) {
            if (chess_board[y - 1][x] == 0) { 
                is_possible_move(x, y - 1, is_white);

                if (y == 6) {
                    is_possible_move(x, y - 2, is_white);
                }
            }

            if (chess_board[y - 1][x - 1] != 0) {
                is_possible_move(x - 1, y - 1, is_white);
            } if (chess_board[y - 1][x + 1] != 0) {
                is_possible_move(x + 1, y - 1, is_white);
            }
        } else {
            if (chess_board[y + 1][x] == 0) { 
                is_possible_move(x, y + 1, is_white); 

                if (y == 1) {
                    is_possible_move(x, y + 2, is_white);
                }
            }

            if (chess_board[y + 1][x - 1] != 0) {
                is_possible_move(x - 1, y + 1, is_white);
            } if (chess_board[y + 1][x + 1] != 0) {
                is_possible_move(x + 1, y + 1, is_white);
            }
        }
        
    }

    bool has_hit_dir[8] = {false};

    // handles the pieces that go to the edges of the board
    // for example rook and bishop
    for (int i = 1; i < 8; i++) { 
        if (moves->straight_or_diagonal[0]) {
            if (x + i < 8 && !has_hit_dir[0]) {
                has_hit_dir[0] = !is_possible_move(x + i, y, is_white);
            } if (y + i < 8 && !has_hit_dir[1]) {
                has_hit_dir[1] = !is_possible_move(x, y + i, is_white);
            } if (x - i >= 0 && !has_hit_dir[2]) {
                has_hit_dir[2] = !is_possible_move(x - i, y, is_white);
            } if (y - i >= 0 && !has_hit_dir[3]) {
                has_hit_dir[3] = !is_possible_move(x, y - i, is_white);
            }
        } 
        
        if (moves->straight_or_diagonal[1]) {
            if ((x + i < 8 && y + i < 8) && !has_hit_dir[4]) {
                has_hit_dir[4] = !is_possible_move(x + i, y + i, is_white);
            } if ((x + i < 8 && y - i >= 0) && !has_hit_dir[5]) {
                has_hit_dir[5] = !is_possible_move(x + i, y - i, is_white);
            } if ((x - i >= 0 && y - i >= 0) && !has_hit_dir[6]) {
                has_hit_dir[6] = !is_possible_move(x - i, y - i, is_white);
            } if ((x - i >= 0 && y + i < 8) && !has_hit_dir[7]) {
                has_hit_dir[7] = !is_possible_move(x - i, y + i, is_white);
            }
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
    if (chosen_piece) {
        possible_moves(chosen_piece_pos[0], chosen_piece_pos[1]);
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



