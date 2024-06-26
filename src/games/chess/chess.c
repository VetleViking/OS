//
// This is the code for a chess game.
// 

#include <system.h> 
#include <chess.h>
#include <chess_pieces.h>

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>



bool in_chess = false;
bool in_chess_game = false;

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

int possible_moves_pos[64][2] = {0};
int len_pm_pos = 0;

struct chess_piece_moves p = {
    .straight_or_diagonal = {false, false},
    .moves = {{0}},
    .is_pawn = true,
    .is_king = false,
    .value = 1 
};

struct chess_piece_moves r = {
    .straight_or_diagonal = {true, false},
    .moves = {{0}},
    .is_pawn = false,
    .is_king = false,
    .value = 5  
};

struct chess_piece_moves n = {
    .straight_or_diagonal = {false, false},
    .moves = {{1, 2}, {2, 1}, {2, -1}, {1, -2}, {-1, -2}, {-2, -1}, {-2, 1}, {-1, 2}},
    .is_pawn = false,
    .is_king = false,
    .value = 3  
};

struct chess_piece_moves b = {
    .straight_or_diagonal = {false, true},
    .moves = {{0}},
    .is_pawn = false,
    .is_king = false,
    .value = 3  
};

struct chess_piece_moves q = {
    .straight_or_diagonal = {true, true},
    .moves = {{0}},
    .is_pawn = false,
    .is_king = false,
    .value = 9  
};

struct chess_piece_moves k = {
    .straight_or_diagonal = {false, false},
    .moves = {{0, 1}, {1, 1}, {1, 0}, {1, -1}, {0, -1}, {-1, -1}, {-1, 0}, {-1, 1}},
    .is_pawn = false,
    .is_king = true,
    .value = 1000  
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
int chosen_piece_pos[2] = {0, 0};
bool pawn_moved_two = false; // used for en passant
int pm_two_pos[2] = {0, 0};
int since_piece_taken = 0;
bool castle_rights[2][3] = {{true, true, true}, {true, true, true}}; // castling rights for each king and the towers
bool have_castled[2] = {false, false};

bool in_threatened_by = false;
int winner = 0; // 1 = white, 2 = black, 3 = draw

int threatened_by_len = 0;
int threatened_by[64][2] = {0};

bool white_turn = true;

int test = 0;

int prev_moves_white[256][2][2] = {0}; // use or remove, currently not used
int prev_moves_white_len = 0;

int prev_moves_black[256][2][2] = {0};
int prev_moves_black_len = 0;



void piece_threatened_by(int x, int y, bool is_white, char board[8][8]) {
    threatened_by_len = 0;

    int pmp_temp[64][2] = {0};
    int pmplen_temp = len_pm_pos;

    for (int i = 0; i < len_pm_pos; i++) {
        pmp_temp[i][0] = possible_moves_pos[i][0];
        pmp_temp[i][1] = possible_moves_pos[i][1];
    }

    in_threatened_by = true; // so the function does not become infinitely recursive

    for (int i = 0; i < 8; i++) { // y
        for (int j = 0; j < 8; j++) { // x
            if (board[i][j] != 0 && ((board[i][j] >= 97 && is_white) || (board[i][j] < 97 && !is_white))){
                possible_moves(j, i, false, board, !is_white);

                for (int k = 0; k < len_pm_pos; k++) {
                    if (possible_moves_pos[k][0] == x && possible_moves_pos[k][1] == y) {
                        threatened_by[threatened_by_len][0] = j;
                        threatened_by[threatened_by_len][1] = i;
                        threatened_by_len++;
                    }
                }
            }
        }   
    }

    len_pm_pos = pmplen_temp;
    for (int i = 0; i < len_pm_pos; i++) {
        possible_moves_pos[i][0] = pmp_temp[i][0];
        possible_moves_pos[i][1] = pmp_temp[i][1];
    }

    in_threatened_by = false;
}

int protected_by_len = 0;
int protected_by[64][2] = {0};
bool in_protected_by = false;

void piece_protected_by(int x, int y, bool is_white, char board[8][8]) {
    in_protected_by = true;
    protected_by_len = 0;

    char board_temp[8][8] = {0};

    for (int i = 0; i < 8; i++) {
        for (int j = 0; j < 8; j++) {
            board_temp[i][j] = board[i][j];
        }
    }

    // invert the board so we can call possible_moves
    for (int i = 0; i < 8; i++) {
        for (int j = 0; j < 8; j++) {
            if (board_temp[i][j] != 0) {
                board_temp[i][j] = board_temp[i][j] < 97 ? board_temp[i][j] + 32 : board_temp[i][j] - 32;
            }
        }
    }

    // revert the piece at the position we are checking
    board_temp[y][x] = board[y][x];

    for (int i = 0; i < 8; i++) { // y
        for (int j = 0; j < 8; j++) { // x
            if (board_temp[i][j] != 0 && ((board_temp[i][j] >= 97 && is_white) || (board_temp[i][j] < 97 && !is_white))){
                possible_moves(j, i, false, board_temp, !is_white);

                for (int k = 0; k < len_pm_pos; k++) {
                    draw_rectangle(test * 5, 0, 5, 5, VGA_COLOR_GREEN, false);
                }

                for (int k = 0; k < len_pm_pos; k++) {
                    if (possible_moves_pos[k][0] == x && possible_moves_pos[k][1] == y) {
                        protected_by[protected_by_len][0] = j;
                        protected_by[protected_by_len][1] = i;
                        protected_by_len++;
                    }
                }
            }
        }   
    }

    in_protected_by = false;
}

bool check_king_threatened(int x, int y, bool is_white, char board[8][8]) {
    for (int i = 0; i < 8; i++) { // y
        for (int j = 0; j < 8; j++) { // x
            if ((board[i][j] == 'K' && is_white) || (board[i][j] == 'k' && !is_white)) {
                char chess_board_temp[8][8] = {0};

                for (int k = 0; k < 8; k++) {
                    for (int l = 0; l < 8; l++) {
                        chess_board_temp[k][l] = board[k][l];
                    }
                }

                chess_board_temp[y][x] = chess_board_temp[chosen_piece_pos[1]][chosen_piece_pos[0]];
                chess_board_temp[chosen_piece_pos[1]][chosen_piece_pos[0]] = 0;

                if (chosen_piece_pos[0] == j && chosen_piece_pos[1] == i) {
                    piece_threatened_by(x, y, is_white, chess_board_temp);
                } else {
                    piece_threatened_by(j, i, is_white, chess_board_temp);
                }

                if (threatened_by_len > 0) {
                    return true;
                }

                break;
            }
        }
    }
    
    return false;
}

bool is_possible_move(int x, int y, bool is_white, bool show_result, char board[8][8]) {
    if (board[y][x] != 0) {
        if ((is_white && board[y][x] >= 97) || (!is_white && board[y][x] < 97)) {    
            if (!in_threatened_by && !in_protected_by) {
                if (check_king_threatened(x, y, is_white, board)) {
                    return false;
                }
            }

            if (show_result) {
                draw_available_move(x, y);
            }

            possible_moves_pos[len_pm_pos][0] = x;
            possible_moves_pos[len_pm_pos][1] = y;
            len_pm_pos++;
        }

        // not quite true, but need it like this other places
        // does not seem to interfere with anything, so will keep it like it is for now
        return false; 
    } else {
        if (!in_threatened_by && !in_protected_by) {
            if (check_king_threatened(x, y, is_white, board)) {
                return true;
            }
        }

        if (show_result) {
            draw_available_move(x, y);
        }
    }
    possible_moves_pos[len_pm_pos][0] = x;
    possible_moves_pos[len_pm_pos][1] = y;
    len_pm_pos++;

    return true;
}

void possible_moves(int x, int y, bool show_result, char board[8][8], bool is_white) {
    len_pm_pos = 0;
    char piece = board[y][x];

    if (board[y][x] < 97) { // makes all pieces lowercase
        piece += ('a' - 'A');
    }

    struct chess_piece_moves* moves = find_piece(piece);

    if (moves == NULL) {
        draw_rectangle(0, test * 5, 5, 5, VGA_COLOR_GREEN, true);
        test++;
        return;
    }

    for (int i = 0; i < 8; i++) {
        if (moves->moves[i][0] == 0 && moves->moves[i][1] == 0) {
            break;
        }

        int new_x = x + moves->moves[i][0];
        int new_y = y + moves->moves[i][1];

        if ((new_x < 8 && new_x >= 0) && (new_y < 8 && new_y >= 0)) {
            is_possible_move(new_x, new_y, is_white, show_result, board);
        }
    }

    // bcause the pawn is a goofy goober, it needs some special treatment
    // will need to add changing at top of board
    if (moves->is_pawn) {
        if (is_white) { // white pawn
            if (board[y - 1][x] == 0 && y - 1 >= 0) { 
                is_possible_move(x, y - 1, is_white, show_result, board);

                if (y == 6 && board[y - 2][x] == 0) {
                    is_possible_move(x, y - 2, is_white, show_result, board);
                }
            }

            if (board[y - 1][x - 1] != 0 && (x - 1 >= 0 && y - 1 >= 0)) {
                is_possible_move(x - 1, y - 1, is_white, show_result, board);
            } if (board[y - 1][x + 1] != 0 && (x + 1 < 8 && y - 1 >= 0)) {
                is_possible_move(x + 1, y - 1, is_white, show_result, board);
            }

            // en passant
            if (pawn_moved_two) {
                if ((pm_two_pos[0] == x - 1 && pm_two_pos[1] == y) && board[y - 1][x - 1] == 0) {
                    is_possible_move(x - 1, y - 1, is_white, show_result, board);
                    if (show_result) {
                        draw_available_move(x - 1, y - 1);
                    }
                } else if ((pm_two_pos[0] == x + 1 && pm_two_pos[1] == y) && board[y - 1][x + 1] == 0) {
                    is_possible_move(x + 1, y - 1, is_white, show_result, board);
                    if (show_result) {
                        draw_available_move(x + 1, y - 1);
                    }
                }
            }
        } else { // black pawn
            if (board[y + 1][x] == 0 && y + 1 < 8) { 
                is_possible_move(x, y + 1, is_white, show_result, board); 

                if (y == 1 && board[y + 2][x] == 0) {
                    is_possible_move(x, y + 2, is_white, show_result, board);
                }
            }

            if (board[y + 1][x - 1] != 0 && (x - 1 >= 0 && y + 1 < 8)) {
                is_possible_move(x - 1, y + 1, is_white, show_result, board);
            } if (board[y + 1][x + 1] != 0 && (x + 1 < 8 && y + 1 < 8)) {
                is_possible_move(x + 1, y + 1, is_white, show_result, board);
            }

            // en passant
            if (pawn_moved_two) { 
                if ((pm_two_pos[0] == x - 1 && pm_two_pos[1] == y) && board[y + 1][x - 1] == 0) {
                    is_possible_move(x - 1, y + 1, is_white, show_result, board);
                    if (show_result) {
                        draw_available_move(x - 1, y + 1);
                    }
                } else if ((pm_two_pos[0] == x + 1 && pm_two_pos[1] == y) && board[y + 1][x + 1] == 0) {
                    is_possible_move(x + 1, y + 1, is_white, show_result, board);
                    if (show_result) {
                        draw_available_move(x + 1, y + 1);
                    }
                }
            }
        }
    }

    // for castling
    if (moves->is_king) {
        if (is_white && castle_rights[0][1]) {
            if (castle_rights[0][0] && board[7][1] == 0 && board[7][2] == 0 && board[7][3] == 0) {
                is_possible_move(2, 7, is_white, show_result, board);
            } if (castle_rights[0][2] && board[7][6] == 0 && board[7][5] == 0) {
                is_possible_move(6, 7, is_white, show_result, board);
            } 
        } else if (!is_white && castle_rights[1][1]) {
            if (castle_rights[1][0] && board[0][1] == 0 && board[0][2] == 0 && board[0][3] == 0) {
                is_possible_move(2, 0, is_white, show_result, board);
            } if (castle_rights[1][2] && board[0][6] == 0 && board[0][5] == 0) {
                is_possible_move(6, 0, is_white, show_result, board);
            }
        }
    }

    bool has_hit_dir[8] = {false};

    // handles the pieces that go to the edges of the board
    // for example rook and bishop
    for (int i = 1; i < 8; i++) { 
        if (moves->straight_or_diagonal[0]) {
            if (x + i < 8 && !has_hit_dir[0]) {
                has_hit_dir[0] = !is_possible_move(x + i, y, is_white, show_result, board);
            } if (y + i < 8 && !has_hit_dir[1]) {
                has_hit_dir[1] = !is_possible_move(x, y + i, is_white, show_result, board);
            } if (x - i >= 0 && !has_hit_dir[2]) {
                has_hit_dir[2] = !is_possible_move(x - i, y, is_white, show_result, board);
            } if (y - i >= 0 && !has_hit_dir[3]) {
                has_hit_dir[3] = !is_possible_move(x, y - i, is_white, show_result, board);
            }
        } 
        
        if (moves->straight_or_diagonal[1]) {
            if ((x + i < 8 && y + i < 8) && !has_hit_dir[4]) {
                has_hit_dir[4] = !is_possible_move(x + i, y + i, is_white, show_result, board);
            } if ((x + i < 8 && y - i >= 0) && !has_hit_dir[5]) {
                has_hit_dir[5] = !is_possible_move(x + i, y - i, is_white, show_result, board);
            } if ((x - i >= 0 && y - i >= 0) && !has_hit_dir[6]) {
                has_hit_dir[6] = !is_possible_move(x - i, y - i, is_white, show_result, board);
            } if ((x - i >= 0 && y + i < 8) && !has_hit_dir[7]) {
                has_hit_dir[7] = !is_possible_move(x - i, y + i, is_white, show_result, board);
            }
        }
    }    
}

void draw_piece(int x, int y, char piece) {
    bool is_white = false;


    if (piece < 97) {
        piece += 32;
        is_white = true;
    }

    int start_x = is_white ? (CHESS_PIECE_WIDTH * 3) : 0;
    int start_y = 0;
    
    if (piece == 'p') { // pawn
        start_x += CHESS_PIECE_WIDTH;
    } else if (piece == 'r') { // rook
    } else if (piece == 'n') { // knight
        start_x += CHESS_PIECE_WIDTH;
        start_y += CHESS_PIECE_HEIGHT;
    } else if (piece == 'b') { // bishop
        start_y += CHESS_PIECE_HEIGHT;
    } else if (piece == 'q') { // queen
        start_x += CHESS_PIECE_WIDTH * 2;
        start_y += CHESS_PIECE_HEIGHT;
    } else if (piece == 'k') { // king
        start_x += CHESS_PIECE_WIDTH * 2;
    }

    for (int i = 0; i < CHESS_PIECE_WIDTH; i++) {
        for (int j = 0; j < CHESS_PIECE_HEIGHT; j++) {
            unsigned int color = chess_pieces[(i + start_y) * CHESS_PIECES_WIDTH + (j + start_x)];
            
            if (color != 0xff) {
                vga_plot_pixel(63 + x * 25 + j,3 + y * 25 + i, color, true);
            }
        }
    }
}

void draw_available_move(int x, int y) {
    if (chess_board[y][x] != 0) {   
        draw_circle(60 + x * 25 + 12, y * 25 + 12, 5, VGA_COLOR_RED, true);
    } else {
        draw_circle(60 + x * 25 + 12, y * 25 + 12, 5, VGA_COLOR_GREEN, true);
    }
}

void print_cursor() {
    int color = (cursor_pos[0] + cursor_pos[1]) % 2 == 0 ? color_light : color_dark;
    char piece[2];
    piece[0] = chess_board[cursor_pos[1]][cursor_pos[0]];
    piece[1] = '\0';

    draw_rectangle(60 + cursor_pos[0] * 25, cursor_pos[1] * 25, 25, 25, VGA_COLOR_RED, true);
    draw_rectangle(63 + cursor_pos[0] * 25, 3 + cursor_pos[1] * 25, 19, 19, color, true);

    if (piece[0] != 0) {
        draw_piece(cursor_pos[0], cursor_pos[1], piece[0]);
    }
    
    if (chosen_piece) {
        for (int i = 0; i < len_pm_pos; i++) {
            if (possible_moves_pos[i][0] == cursor_pos[0] && possible_moves_pos[i][1] == cursor_pos[1]) {
                draw_available_move(cursor_pos[0], cursor_pos[1]);
            }
        }
    }
}

void remove_cursor() {
    char piece[2];
    piece[0] = chess_board[cursor_pos[1]][cursor_pos[0]];
    piece[1] = '\0';

    int color = (cursor_pos[0] + cursor_pos[1]) % 2 == 0 ? color_light : color_dark;
    draw_rectangle(60 + cursor_pos[0] * 25, cursor_pos[1] * 25, 25, 25, color, true);
    if (piece[0] != 0) {
        draw_piece(cursor_pos[0], cursor_pos[1], piece[0]);
    }
    if (chosen_piece) {
        for (int i = 0; i < len_pm_pos; i++) {
            if (possible_moves_pos[i][0] == cursor_pos[0] && possible_moves_pos[i][1] == cursor_pos[1]) {
                draw_available_move(cursor_pos[0], cursor_pos[1]);
            }
        }
    }
}

void chess_print_board(char board[8][8]) {
    for (int i = 60; i < 260; i += 50) {
        for (int j = 0; j < 200; j += 50) {
            draw_rectangle(i, j, 25, 25, color_light, true);
            draw_rectangle(i + 25, j, 25, 25, color_dark, true);
            draw_rectangle(i + 25, j + 25, 25, 25, color_light, true);
            draw_rectangle(i, j + 25, 25, 25, color_dark, true);
        }
    }

    for (int i = 0; i < 8; i++) {
        for (int j = 0; j < 8; j++) {
            if (board[i][j] != 0) {
                draw_piece(j, i, board[i][j]);
            }
        }
    }

    if (chosen_piece) {
        possible_moves(chosen_piece_pos[0], chosen_piece_pos[1], true, board, white_turn);
    }

    print_cursor();

    vga_print_frame_buffer();
}

void move_piece(int x, int y) {
    for (int i = 0; i < len_pm_pos; i++) {
        if (possible_moves_pos[i][0] == x && possible_moves_pos[i][1] == y) {
            if (white_turn) {
                prev_moves_white[prev_moves_white_len][0][0] = chosen_piece_pos[0];
                prev_moves_white[prev_moves_white_len][0][1] = chosen_piece_pos[1];
                prev_moves_white[prev_moves_white_len][1][0] = x;
                prev_moves_white[prev_moves_white_len][1][1] = y;
                prev_moves_white_len++;
            } else {
                prev_moves_black[prev_moves_black_len][0][0] = chosen_piece_pos[0];
                prev_moves_black[prev_moves_black_len][0][1] = chosen_piece_pos[1];
                prev_moves_black[prev_moves_black_len][1][0] = x;
                prev_moves_black[prev_moves_black_len][1][1] = y;
                prev_moves_black_len++;
            } 

            char piece[2];
            piece[0] = chess_board[chosen_piece_pos[1]][chosen_piece_pos[0]];
            piece[1] = '\0';

            char taken_piece[2];
            taken_piece[0] = chess_board[y][x];
            taken_piece[1] = '\0';

            if (taken_piece[0] != 0) {
                since_piece_taken = 0;

                if (taken_piece[0] == 'k' || taken_piece[0] == 'K') {
                    winner = piece < 97 ? 1 : 2;
                    in_chess_game = false;
                }
            } else {
                since_piece_taken++;
                if (since_piece_taken >= 50) {
                    winner = 3;
                    in_chess_game = false;
                }
            }

            if (piece[0] == 'p' || piece[0] == 'P') {
                if (pawn_moved_two && x == pm_two_pos[0]) {
                    if (y + 1 == pm_two_pos[1] && piece[0] == 'P') {
                        chess_board[pm_two_pos[1]][pm_two_pos[0]] = 0;
                    } else if (y - 1 == pm_two_pos[1] && piece[0] == 'p') {
                        chess_board[pm_two_pos[1]][pm_two_pos[0]] = 0;
                    } 
                } 
                
                if (chosen_piece_pos[1] - y == 2 || chosen_piece_pos[1] - y == -2) {
                    pawn_moved_two = true;
                    pm_two_pos[0] = x;
                    pm_two_pos[1] = y;
                } else {
                    pawn_moved_two = false;
                }

                if (y == 0 && piece[0] == 'P') {
                    piece[0] = 'Q';
                } else if (y == 7 && piece[0] == 'p') {
                    piece[0] = 'q';
                }
            } 

            if (piece[0] == 'R') {
                if (chosen_piece_pos[0] == 0 && chosen_piece_pos[1] == 7) {
                    castle_rights[0][0] = false;
                } else if (chosen_piece_pos[0] == 7 && chosen_piece_pos[1] == 7) {
                    castle_rights[0][2] = false;
                }
            } else if (piece[0] == 'r') {
                if (chosen_piece_pos[0] == 0 && chosen_piece_pos[1] == 0) {
                    castle_rights[1][0] = false;
                } else if (chosen_piece_pos[0] == 7 && chosen_piece_pos[1] == 0) {
                    castle_rights[1][2] = false;
                }
            } 

            if (piece[0] == 'K' && chosen_piece_pos[0] == 4) {
                if (x == 2) {
                    chess_board[7][0] = 0;
                    chess_board[7][3] = 'R'; 
                    have_castled[0] = true;
                } else if (x == 6) {
                    chess_board[7][7] = 0;
                    chess_board[7][5] = 'R'; 
                    have_castled[0] = true;
                }

                castle_rights[0][1] = false;
            } else if (piece[0] == 'k' && chosen_piece_pos[0] == 4) {
                if (x == 2 && y == 0) {
                    chess_board[0][0] = 0;
                    chess_board[0][3] = 'r';
                    have_castled[1] = true; 
                } else if (x == 6 && y == 0) {
                    chess_board[0][7] = 0;
                    chess_board[0][5] = 'r'; 
                    have_castled[1] = true;
                }
            
                castle_rights[1][1] = false;
            }

            chess_board[y][x] = piece[0];
            chess_board[chosen_piece_pos[1]][chosen_piece_pos[0]] = 0;

            int total_moves = 0;

            for (int i = 0; i < 8; i++) { // y
                for (int j = 0; j < 8; j++) { // x
                    if ((chess_board[i][j] > 97 && white_turn) || (chess_board[i][j] < 97 && chess_board[i][j] > 0 && !white_turn)) {
                        chosen_piece_pos[0] = j; // jank, i dont care
                        chosen_piece_pos[1] = i;
                        chosen_piece = true;

                        possible_moves(j, i, false, chess_board, !white_turn);

                        total_moves += len_pm_pos;
                    }
                }   
            }

            if (total_moves == 0) {
                if (check_mate(!white_turn, chess_board)) {
                    winner = white_turn ? 1 : 2;
                    in_chess_game = false;
                } else {
                    winner = 3;
                    in_chess_game = false;
                }
            }

            possible_moves(x, y, false, chess_board, white_turn);

            for (int i = 0; i < len_pm_pos; i++) {
                int chess_board_move = chess_board[possible_moves_pos[i][1]][possible_moves_pos[i][0]];

                if (chess_board_move == 'K') {
                    castle_rights[0][1] = false;
                } else if (chess_board_move == 'k') {
                    castle_rights[1][1] = false;
                }
            }

                           
            
            white_turn = !white_turn;
            break;
        }
    }

    chosen_piece = false;
}

int is_protected(int x, int y, bool is_white, char board[8][8]) {
    piece_threatened_by(x, y, !is_white, board);
    int protected_by = threatened_by_len;    
    piece_threatened_by(x, y, is_white, board);

    if (protected_by < threatened_by_len) {
        return threatened_by_len - protected_by;
    }

    return 0;
}

bool check_mate(bool king_white, char board[8][8]) {
    int king_pos[2] = {0};
    int total_moves = 0;

    for (int i = 0; i < 8; i++) { // y
        for (int j = 0; j < 8; j++) { // x
            if ((board[i][j] > 97 && !king_white) || (board[i][j] < 97 && board[i][j] > 0 && king_white)) {
                chosen_piece_pos[0] = j; // jank, i dont care
                chosen_piece_pos[1] = i;

                if (board[i][j] == 'k' || board[i][j] == 'K') {
                    king_pos[0] = j;
                    king_pos[1] = i;
                }

                possible_moves(j, i, false, board, king_white);

                total_moves += len_pm_pos;
            }
        }   
    }

    if (total_moves == 0) {
        piece_threatened_by(king_pos[0], king_pos[1], king_white, board);

        if (threatened_by_len > 0) {
            return true;
        }
    }
        
    return false; 
}

bool last_left_click = false;

void chess_mouse_handler(int8_t mouse_byte[3]) {
    remove_mouse(mouse_x, mouse_y);

    mouse_x += mouse_byte[1] * 1;
    mouse_y -= mouse_byte[2] * 1;

    if (mouse_x < 0) {
        mouse_x = 0;
    } else if (mouse_x > 320) {
        mouse_x = 320;
    }

    if (mouse_y < 0) {
        mouse_y = 0;
    } else if (mouse_y > 200) {
        mouse_y = 200;
    }

    int x = (mouse_x - 60) / 25;
    int y = mouse_y / 25;

    if (mouse_byte[0] & 0x01) { // left click
        if (in_chess_game) {
            if (!last_left_click) {
                if (x >= 0 && x < 8 && y >= 0 && y < 8) {
                    cursor_pos[0] = x;
                    cursor_pos[1] = y;

                    enter_or_left_click();

                    print_cursor();
                }
            }
        }

        last_left_click = true;
    } else {
        last_left_click = false;
    }
    if (mouse_byte[0] & 0x02) { } // right click
    if (mouse_byte[0] & 0x04) { } // middle click

    print_mouse(mouse_x, mouse_y);
}

void chess_keyboard_handler(int c) {
    if (!in_chess_game) {
        if (c == 75) { // left
            if (cursor_pos[0] > 4) {
                remove_cursor();
                draw_rectangle(60 + cursor_pos[0] * 25, cursor_pos[1] * 25, 25, 25, VGA_COLOR_LIGHT_RED, false);
                cursor_pos[0] -= 3;
                print_cursor();
                draw_rectangle(63 + cursor_pos[0] * 25, 3 + cursor_pos[1] * 25, 19, 19, VGA_COLOR_GREEN, false);
            }
        } else if (c == 77) { // right
            if (cursor_pos[0] < 3) {
                remove_cursor();
                draw_rectangle(60 + cursor_pos[0] * 25, cursor_pos[1] * 25, 25, 25, VGA_COLOR_GREEN, false);
                cursor_pos[0] += 3;
                print_cursor();
                draw_rectangle(63 + cursor_pos[0] * 25, 3 + cursor_pos[1] * 25, 19, 19, VGA_COLOR_LIGHT_RED, false);
            }
        } else if (c == 28) { // enter
        if (cursor_pos[0] == 2) {
                // reset all counters and stuff.
                vga_clear_screen();

                cursor_pos[0] = 0;
                cursor_pos[1] = 7;

                chosen_piece = false;
                pawn_moved_two = false; // used for en passant
                since_piece_taken = 0;

                for (int i = 0; i < 3; i++) {
                    castle_rights[0][i] = true;
                    castle_rights[1][i] = true;
                }


                winner = 0; // 1 = white, 2 = black, 3 = draw

                threatened_by_len = 0;

                prev_moves_white_len = 0;
                prev_moves_black_len = 0;

                white_turn = true;

                test = 0;

                char chess_board_orig[8][8] = {
                    {'r', 'n', 'b', 'q', 'k', 'b', 'n', 'r'},
                    {'p', 'p', 'p', 'p', 'p', 'p', 'p', 'p'},
                    {0, 0, 0, 0, 0, 0, 0, 0},
                    {0, 0, 0, 0, 0, 0, 0, 0},
                    {0, 0, 0, 0, 0, 0, 0, 0},
                    {0, 0, 0, 0, 0, 0, 0, 0},
                    {'P', 'P', 'P', 'P', 'P', 'P', 'P', 'P'},
                    {'R', 'N', 'B', 'Q', 'K', 'B', 'N', 'R'}
                };

                for (int i = 0; i < 8; i++) {
                    for (int j = 0; j < 8; j++) {
                        chess_board[i][j] =  chess_board_orig[i][j];
                    }
                }

                chess_print_board(chess_board);

                in_chess_game = true;
                in_chess = false;
            } else if (cursor_pos[0] == 5) {
                in_chess = false;
            }
        }
 
        return;
    }


    if (c == 75) { // left
        if (cursor_pos[0] > 0) {
            remove_cursor();
            cursor_pos[0]--;
            print_cursor();
            chess_print_board(chess_board);
        }
    } else if (c == 77) { // right
        if (cursor_pos[0] < 7) {
            cursor_pos[0]++;
            chess_print_board(chess_board);
        }
    } else if (c == 72) { // up
        if (cursor_pos[1] > 0) {
            cursor_pos[1]--;
            chess_print_board(chess_board);
        }
	} else if (c == 80) { // down
        if (cursor_pos[1] < 7) {
            cursor_pos[1]++;
            chess_print_board(chess_board);
        }
    } else if (c == 28) { // enter
        enter_or_left_click();
    } else if (c == 1) { // esc
        winner = 3;
        in_chess_game = false;
    } else if (c == 48) { // b
        if (white_turn) {
            chess_bot(white_turn);
        } else {
            chess_bot_experimental(white_turn, chess_board);
            //chess_bot_experimental_depth(white_turn, chess_board, 3);
        }
    }
}

void enter_or_left_click() {
    char piece[2];
    piece[0] = chess_board[cursor_pos[1]][cursor_pos[0]];
    piece[1] = '\0';

    if (piece[0] != 0 && !chosen_piece) {
        if ((piece[0] < 97 && piece[0] < 97 && white_turn) || (piece[0] >= 97 && !white_turn)) {
            chosen_piece_pos[0] = cursor_pos[0];
            chosen_piece_pos[1] = cursor_pos[1];
            chosen_piece = true;
        }
    } else if (chosen_piece) {
        if (cursor_pos[0] == chosen_piece_pos[0] && cursor_pos[1] == chosen_piece_pos[1]) {
            chosen_piece = false;
            chess_print_board(chess_board);
            return;
        }

        for (int i = 0; i < len_pm_pos; i++) {
            if (possible_moves_pos[i][0] == cursor_pos[0] && possible_moves_pos[i][1] == cursor_pos[1]) {
                move_piece(cursor_pos[0], cursor_pos[1]);

                // for playing with bot
                if (!white_turn) {
                    //chess_bot(white_turn);
                    chess_bot_experimental(white_turn, chess_board);  
                    
                    //chess_bot_experimental_depth(white_turn, chess_board, 3);
                }

                chess_print_board(chess_board);
                return;
            }
        }

        if ((piece[0] < 97 && piece[0] > 0 && white_turn) || (piece[0] >= 97 && !white_turn)) {
            chosen_piece_pos[0] = cursor_pos[0];
            chosen_piece_pos[1] = cursor_pos[1];
        } else {
            chosen_piece = false;
        }
    }
    
    chess_print_board(chess_board);
}

void chess_play() {

    while (in_chess_game) {
        sleep(10); // sleep for 0.1s

        // this is just so that other stuff wont be running
        // may be adding a clock or something later, that will go here.
    }


    vga_clear_screen();

    if (winner == 1) {
        draw_rectangle(0, 0, 25, 25, VGA_COLOR_WHITE, true);
    } else if (winner == 2) {
        draw_rectangle(0, 0, 25, 25, VGA_COLOR_WHITE, true);
        draw_rectangle(1, 1, 23, 23, VGA_COLOR_BLACK, true);
    } else if (winner == 3) {
        draw_rectangle(0, 0, 25, 25, VGA_COLOR_GREEN, true);
    }

    cursor_pos[0] = 2;
    cursor_pos[1] = 3;

    draw_rectangle(60 + 75 + cursor_pos[0] * 25, cursor_pos[1] * 25, 25, 25, VGA_COLOR_LIGHT_RED, true);

    print_cursor();
    draw_rectangle(63 + cursor_pos[0] * 25, 3 + cursor_pos[1] * 25, 19, 19, VGA_COLOR_GREEN, true);

    vga_print_frame_buffer();

    while (in_chess) {
        sleep(100); // sleep for 0.1s
    }

    if (in_chess_game) { // if it aint broke, dont fix
        in_chess = true;

        chess_play();
    }

    // exit stuff
    vga_exit(); // not working
    remove_keyboard_handler(chess_keyboard_handler);
    remove_main_loop_call(chess_play);
}  


void chess_start() {
    add_keyboard_handler(chess_keyboard_handler);
    add_main_loop_call(chess_play);

    in_chess = true;
    in_chess_game = true;

    vga_enter();
    vga_clear_screen();

    mouse_x = 160;
    mouse_y = 100;
    add_mouse_handler(chess_mouse_handler);

    cursor_pos[0] = 0;
    cursor_pos[1] = 7;
    chess_print_board(chess_board);
}


