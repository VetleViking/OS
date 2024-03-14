//
// This is the code for a chess game.
// 

#include <system.h> 

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

struct chess_piece_moves {
    bool straight_or_diagonal [2];
    int moves [8][2];
    bool is_pawn;
    bool is_king;
    int value;
};

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
bool in_threatened_by = false;
int winner = 0; // 1 = white, 2 = black, 3 = draw

int threatened_by_len = 0;
int threatened_by[64][2] = {0};

bool white_turn = true;

int test = 0;



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
            if (board[i][j] != 0) {
                possible_moves(j, i, false, board);

                for (int k = 0; k < len_pm_pos; k++) {
                    if (possible_moves_pos[k][0] == x && possible_moves_pos[k][1] == y && (is_white == (board[i][j] >= 97))) {
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
            if (!in_threatened_by) {
                if (check_king_threatened(x, y, is_white, board)) {
                    return false;
                }
            }

            if (show_result) {
                draw_rectangle(60 + x * 25, y * 25, 25, 25, VGA_COLOR_RED);
            }

            possible_moves_pos[len_pm_pos][0] = x;
            possible_moves_pos[len_pm_pos][1] = y;
            len_pm_pos++;
        }

        // not quite true, but need it like this other places
        // does not seem to interfere with anything, so will keep it like it is for now
        return false; 
    } else {
        if (!in_threatened_by) {
            if (check_king_threatened(x, y, is_white, board)) {
                return true;
            }
        }

        if (show_result) {
            draw_rectangle(60 + x * 25, y * 25, 25, 25, VGA_COLOR_GREEN);
        }
    }
    possible_moves_pos[len_pm_pos][0] = x;
    possible_moves_pos[len_pm_pos][1] = y;
    len_pm_pos++;

    return true;
}

// TODO: add so that you cant put king in danger, either in is_possible_move or possible_moves.

void possible_moves(int x, int y, bool show_result, char board[8][8]) {
    len_pm_pos = 0;
    char piece = board[y][x];
    bool is_white = !(piece >= 'a' && piece <= 'z');

    if (is_white) { // makes all pieces lowercase
        piece += ('a' - 'A');
    }

    struct chess_piece_moves* moves = find_piece(piece);

    if (moves == NULL) {
        draw_rectangle(0, test * 5, 5, 5, VGA_COLOR_GREEN);
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
                        draw_rectangle(60 + (x - 1) * 25, y * 25, 25, 25, VGA_COLOR_RED);
                    }
                } else if ((pm_two_pos[0] == x + 1 && pm_two_pos[1] == y) && board[y - 1][x + 1] == 0) {
                    is_possible_move(x + 1, y - 1, is_white, show_result, board);
                    if (show_result) {
                        draw_rectangle(60 + (x + 1) * 25, y * 25, 25, 25, VGA_COLOR_RED);
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
                        draw_rectangle(60 + (x - 1) * 25, y * 25, 25, 25, VGA_COLOR_RED);
                    }
                } else if ((pm_two_pos[0] == x + 1 && pm_two_pos[1] == y) && board[y + 1][x + 1] == 0) {
                    is_possible_move(x + 1, y + 1, is_white, show_result, board);
                    if (show_result) {
                        draw_rectangle(60 + (x + 1) * 25, y * 25, 25, 25, VGA_COLOR_RED);
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
    int color = VGA_COLOR_BLACK;
    int color2 = VGA_COLOR_WHITE;


    if (piece < 97) {
        piece += 32;
        color = VGA_COLOR_WHITE;
        color2 = VGA_COLOR_BLACK;
    }

 
    if (piece == 'p') { // pawn
        draw_rectangle(63 + x * 25, 18 + y * 25, 19, 4, color2);
        draw_rectangle(64 + x * 25, 19 + y * 25, 17, 2, color);
    } else if (piece == 'r') { // rook
        draw_rectangle(63 + x * 25, 15 + y * 25, 19, 7, color2);
        draw_rectangle(64 + x * 25, 16 + y * 25, 17, 5, color);
    } else if (piece == 'n') { // knight
        draw_rectangle(63 + x * 25, 12 + y * 25, 19, 10, color2);
        draw_rectangle(64 + x * 25, 13 + y * 25, 17, 8, color);
    } else if (piece == 'b') { // bishop
        draw_rectangle(63 + x * 25, 9 + y * 25, 19, 13, color2);
        draw_rectangle(64 + x * 25, 10 + y * 25, 17, 11, color);
    } else if (piece == 'q') { // queen
        draw_rectangle(63 + x * 25, 6 + y * 25, 19, 16, color2);
        draw_rectangle(64 + x * 25, 7 + y * 25, 17, 14, color);
    } else if (piece == 'k') { // king
        draw_rectangle(63 + x * 25, 3 + y * 25, 19, 19, color2);
        draw_rectangle(64 + x * 25, 4 + y * 25, 17, 17, color);
    }
}

void print_cursor() {
    int color = (cursor_pos[0] + cursor_pos[1]) % 2 == 0 ? color_light : color_dark;
    char piece[2];
    piece[0] = chess_board[cursor_pos[1]][cursor_pos[0]];
    piece[1] = '\0';

    draw_rectangle(60 + cursor_pos[0] * 25, cursor_pos[1] * 25, 25, 25, VGA_COLOR_RED);
    draw_rectangle(63 + cursor_pos[0] * 25, 3 + cursor_pos[1] * 25, 19, 19, color);

    if (piece[0] != 0) {
        draw_piece(cursor_pos[0], cursor_pos[1], piece[0]);
    }
}

void remove_cursor() {
    char piece[2];
    piece[0] = chess_board[cursor_pos[1]][cursor_pos[0]];
    piece[1] = '\0';

    int color = (cursor_pos[0] + cursor_pos[1]) % 2 == 0 ? color_light : color_dark;
    draw_rectangle(60 + cursor_pos[0] * 25, cursor_pos[1] * 25, 25, 25, color);
    if (piece[0] != 0) {
        draw_piece(cursor_pos[0], cursor_pos[1], piece[0]);
    }
    if (chosen_piece) {
        for (int i = 0; i < len_pm_pos; i++) {
            bool is_piece = chess_board[possible_moves_pos[i][1]][possible_moves_pos[i][0]] != 0;

            draw_rectangle(60 + possible_moves_pos[i][0] * 25, possible_moves_pos[i][1] * 25, 25, 25, is_piece ? VGA_COLOR_RED : VGA_COLOR_GREEN);
        }
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

void move_piece(int x, int y) {
    for (int i = 0; i < len_pm_pos; i++) {
        if (possible_moves_pos[i][0] == x && possible_moves_pos[i][1] == y) {
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

            if (piece[0] == 'K') {
                if (x == 2) {
                    chess_board[7][0] = 0;
                    chess_board[7][3] = 'R'; 
                } else if (x == 6) {
                    chess_board[7][7] = 0;
                    chess_board[7][5] = 'R'; 
                }

                castle_rights[0][1] = false;
            } else if (piece[0] == 'k') {
                if (x == 2) {
                    chess_board[0][0] = 0;
                    chess_board[0][3] = 'r'; 
                } else if (x == 6) {
                    chess_board[0][7] = 0;
                    chess_board[0][5] = 'r'; 
                }
            
                castle_rights[1][1] = false;
            }

            chess_board[y][x] = piece[0];
            chess_board[chosen_piece_pos[1]][chosen_piece_pos[0]] = 0;

            int king_pos[2] = {0};
            int total_moves = 0;

            for (int i = 0; i < 8; i++) { // y
                for (int j = 0; j < 8; j++) { // x
                    if ((chess_board[i][j] > 97 && white_turn) || (chess_board[i][j] < 97 && chess_board[i][j] > 0 && !white_turn)) {
                        chosen_piece_pos[0] = j; // jank, i dont care
                        chosen_piece_pos[1] = i;
                        chosen_piece = true;

                        if (chess_board[i][j] == 'k' || chess_board[i][j] == 'K') {
                            king_pos[0] = j;
                            king_pos[1] = i;
                        }

                        possible_moves(j, i, false, chess_board);

                        total_moves += len_pm_pos;
                    }
                }   
            }

            if (total_moves == 0) {
                piece_threatened_by(king_pos[0], king_pos[1], piece[0] < 97, chess_board);
                if (threatened_by_len > 0) {
                    winner = piece < 97 ? 1 : 2;
                    in_chess_game = false;
                } else {
                    winner = 3;
                    in_chess_game = false;
                }
            }

            possible_moves(x, y, false, chess_board);

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
    chess_print_board();
}

int is_protected(int x, int y, int is_white, char board[8][8]) {
    piece_threatened_by(x, y, !is_white, board);
    int protected_by = threatened_by_len;    
    piece_threatened_by(x, y, is_white, board);

    if (protected_by < threatened_by_len) {
        return threatened_by_len - protected_by;
    }

    return 0;
}

void chess_bot() {
    int points_best_move = -10;
    int best_move[2][2] = {{0}, {0}};

    int threatened_pieces[64][3] = {{0}, {0}, {0}};
    int threatened_pieces_len = 0;

    for (int i = 0; i < 8; i++) { // y
        for (int j = 0; j < 8; j++) { // x
            if (chess_board[i][j] >= 97) { 
                int protected = is_protected(j, i, false, chess_board);

                if (protected > 0) {
                    threatened_pieces[threatened_pieces_len][0] = j;
                    threatened_pieces[threatened_pieces_len][1] = i;
                    threatened_pieces[threatened_pieces_len][2] = protected;
                    threatened_pieces_len++;
                }
            } 
        }
    }

    int threatening_pieces[64][3] = {{0}, {0}, {0}};
    int threatening_pieces_len = 0;

    for (int i = 0; i < 8; i++) { // y
        for (int j = 0; j < 8; j++) { // x
            if (chess_board[i][j] < 97 && chess_board[i][j] != 0) { 
                int protected = is_protected(j, i, true, chess_board);

                if (protected > 0) {
                    threatening_pieces[threatening_pieces_len][0] = j;
                    threatening_pieces[threatening_pieces_len][0] = i;
                    threatening_pieces[threatening_pieces_len][0] = protected;
                    threatening_pieces_len++;
                }
            } 
        }
    }


    for (int i = 0; i < 8; i++) { // y
        for (int j = 0; j < 8; j++) { // x
            if (chess_board[i][j] >= 97) {
                chosen_piece_pos[0] = j;
                chosen_piece_pos[1] = i;

                possible_moves(j, i, false, chess_board);

                for (int k = 0; k < len_pm_pos; k++) {
                    int x = possible_moves_pos[k][0];
                    int y = possible_moves_pos[k][1];
                    int points = 0;

                    char temp_board[8][8] = {0};

                    for (int l = 0; l < 8; l++) {
                        for (int m = 0; m < 8; m++) {
                            temp_board[m][l] = chess_board[m][l];
                        }
                    }

                    temp_board[y][x] = temp_board[i][j];
                    temp_board[i][j] = 0;

                    if (chess_board[y][x] != 0) {
                        struct chess_piece_moves* piece = find_piece(chess_board[y][x] + 32);

                        points += piece->value;                    
                    }

                    int protected = is_protected(x, y, false, temp_board);

                    if (protected > 0) {
                        struct chess_piece_moves* piece = find_piece(chess_board[i][j]);

                        points -= piece->value;     
                    }

                    for (int l = 0; l < threatened_pieces_len; l++) {
                        if (is_protected(threatened_pieces[l][0], threatened_pieces[l][1], false, temp_board) > 0 && (threatened_pieces[l][0] != j && threatened_pieces[l][1] != i)) {
                            struct chess_piece_moves* piece = find_piece(chess_board[threatened_pieces[l][1]][threatened_pieces[l][0]]);

                            points -= piece->value;
                        }
                    }

                    for (int l = 0; l < threatening_pieces_len; l++) {
                        if (is_protected(threatening_pieces[l][0], threatening_pieces[l][1], false, temp_board) > 0) {
                            struct chess_piece_moves* piece = find_piece(chess_board[threatening_pieces[l][1]][threatening_pieces[l][0]]);

                            points += piece->value / 2;
                        }
                    }


                    if (chess_board[i][j] == 'k') {
                        if (x - j == -2 || x - j == 2) {
                            points += 2;
                        } else {
                            points -= 1;
                        }
                    } else if (chess_board[i][j] == 'p') {
                        if (j < 5 && j > 2) {
                            points += 1;
                        } 
                        
                        if (castle_rights[1][1] && ((castle_rights[1][0] && j < 3) || castle_rights[1][2] && j > 4)) {
                            points -= 1;
                        } else {
                            for (int l = 0; l < 3; l++) {
                                if ((chess_board[i][l] == 'k' && j < 3) || (chess_board[i][l + 5] == 'k' && j > 4)) {
                                    points -= 2;
                                }
                            }
                        }
                    }

                    if (points_best_move <= points) {
                        points_best_move = points;
                        best_move[0][0] = j;
                        best_move[0][1] = i;
                        best_move[1][0] = x;
                        best_move[1][1] = y;
                    }
                }                      
            }
        }   
    }

    chosen_piece_pos[0] = best_move[0][0];
    chosen_piece_pos[1] = best_move[0][1];
    chosen_piece = true;

    possible_moves(chosen_piece_pos[0], chosen_piece_pos[1], false, chess_board);

    move_piece(best_move[1][0], best_move[1][1]);
}

void chess_keyboard_handler(int c) {
    if (!in_chess_game) {
        if (c == 75) { // left
            if (cursor_pos[0] > 4) {
                remove_cursor();
                draw_rectangle(60 + cursor_pos[0] * 25, cursor_pos[1] * 25, 25, 25, VGA_COLOR_LIGHT_RED);
                cursor_pos[0] -= 3;
                print_cursor();
                draw_rectangle(63 + cursor_pos[0] * 25, 3 + cursor_pos[1] * 25, 19, 19, VGA_COLOR_GREEN);
            }
        } else if (c == 77) { // right
            if (cursor_pos[0] < 3) {
                remove_cursor();
                draw_rectangle(60 + cursor_pos[0] * 25, cursor_pos[1] * 25, 25, 25, VGA_COLOR_GREEN);
                cursor_pos[0] += 3;
                print_cursor();
                draw_rectangle(63 + cursor_pos[0] * 25, 3 + cursor_pos[1] * 25, 19, 19, VGA_COLOR_LIGHT_RED);
            }
        } else if (c == 28) { // enter
            // reset all counters and stuff, then set in_chess_game to true and in_chess to false.
        }
 
        return;
    }


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
        char piece[2];
        piece[0] = chess_board[cursor_pos[1]][cursor_pos[0]];
        piece[1] = '\0';

        if (piece[0] != 0 && !chosen_piece) {
            if ((piece[0] < 97 && white_turn) || (piece[0] >= 97 && !white_turn)) {
                chosen_piece_pos[0] = cursor_pos[0];
                chosen_piece_pos[1] = cursor_pos[1];
                chosen_piece = true;

                possible_moves(cursor_pos[0], cursor_pos[1], true, chess_board); 
            }
        } else if (chosen_piece) {
            move_piece(cursor_pos[0], cursor_pos[1]);
            
            if (!white_turn) { // add multiplayer later
                chess_bot();
            }
        }
    } else if (c == 1) { // esc
        winner = 1;
        in_chess_game = false;
    }
}

void chess_play() {
    while (in_chess_game) {
        sleep(100); // sleep for 1s

        // this is just so that other stuff wont be running
        // may be adding a clock or something later, that will go here.
    }


    vga_clear_screen();

    if (winner == 1) {
        draw_rectangle(0, 0, 25, 25, VGA_COLOR_WHITE);
    } else if (winner == 2) {
        draw_rectangle(0, 0, 25, 25, VGA_COLOR_WHITE);
        draw_rectangle(1, 1, 23, 23, VGA_COLOR_BLACK);
    } else if (winner == 3) {
        draw_rectangle(0, 0, 25, 25, VGA_COLOR_GREEN);
    }

    cursor_pos[0] = 2;
    cursor_pos[1] = 3;

    draw_rectangle(60 + cursor_pos[0] * 25, cursor_pos[1] * 25, 25, 25, VGA_COLOR_GREEN);
    draw_rectangle(60 + 75 + cursor_pos[0] * 25, cursor_pos[1] * 25, 25, 25, VGA_COLOR_LIGHT_RED);

    while (in_chess) {
        sleep(100); // sleep for 1s
    }

    if (in_chess_game) { // if it aint broke, dont fix
        in_chess = true;

        chess_play();
    }

    // exit stuff
    vga_exit(); // not working

}  

void chess_start() {
    in_chess = true;
    in_chess_game = true;

    vga_enter();
    vga_clear_screen();


    cursor_pos[0] = 0;
    cursor_pos[1] = 7;
    chess_print_board();
}



