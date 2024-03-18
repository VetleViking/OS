//
// This is the code for a chess bot, for the chess game in the same folder.
//
// It uses a point system that gives points for good things (like castling, taking pieces and protecting or moving pieces in danger),
// and takes points for bad things (like putting pieces in danger and moving the king out from cover)
//

#include <system.h> 
#include <chess.h>

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

void chess_bot(bool is_white) {
    int points_best_move = 0;
    int best_move[2][2] = {{0}, {0}};
    bool chosen_move = false;

    int threatened_pieces[64][3] = {{0}, {0}, {0}};
    int threatened_pieces_len = 0;

    for (int i = 0; i < 8; i++) { // y
        for (int j = 0; j < 8; j++) { // x
            if ((chess_board[i][j] != 0 && chess_board[i][j] < 97 && is_white) || (chess_board[i][j] >= 97 && !is_white)) { 
                int protected = is_protected(j, i, is_white, chess_board);

                if (protected > 0) {
                    threatened_pieces[threatened_pieces_len][0] = j;
                    threatened_pieces[threatened_pieces_len][1] = i;
                    threatened_pieces[threatened_pieces_len][2] = protected;
                    threatened_pieces_len++;
                }
            } 
        }
    }

    // dont know if this is working, will keep it for now

    // int threatening_pieces[64][3] = {{0}, {0}, {0}};
    // int threatening_pieces_len = 0;

    // for (int i = 0; i < 8; i++) { // y
    //     for (int j = 0; j < 8; j++) { // x
    //         if ((chess_board[i][j] < 97 && is_white) || (chess_board[i][j] < 97 && chess_board[i][j] != 0 && !is_white)) { 
    //             int protected = is_protected(j, i, !is_white, chess_board);

    //             if (protected > 0) {
    //                 threatening_pieces[threatening_pieces_len][0] = j;
    //                 threatening_pieces[threatening_pieces_len][0] = i;
    //                 threatening_pieces[threatening_pieces_len][0] = protected;
    //                 threatening_pieces_len++;
    //             }
    //         } 
    //     }
    // }

    for (int i = 0; i < 8; i++) { // y
        for (int j = 0; j < 8; j++) { // x
            if ((chess_board[i][j] != 0 && chess_board[i][j] < 97 && is_white) || (chess_board[i][j] >= 97 && !is_white)) {
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

                    temp_board[y][x] = chess_board[i][j];
                    temp_board[i][j] = 0;

                    if (temp_board[y][x] == 0) {
                        draw_rectangle(0, test * 5, 5, 5, VGA_COLOR_BLUE);
                        test++;
                    }
                    if (temp_board[y][x] != chess_board[i][j]) {
                        draw_rectangle(0, test * 5, 5, 5, VGA_COLOR_RED);
                        test++;
                    }

                    if (chess_board[y][x] != 0) {
                        char p[2];
                        p[0] = chess_board[y][x];
                        p[1] = '\0';

                        struct chess_piece_moves* piece = find_piece(p[0] < 97 ? p[0] + 32 : p[0]);

                        if (piece == NULL) {
                            draw_rectangle(0, test * 5, 5, 5, VGA_COLOR_GREEN);
                            test++;
                        }

                        points += piece->value;                    
                    }

                    int protected = is_protected(x, y, is_white, temp_board);

                    if (protected > 0) {
                        char p[2];
                        p[0] = chess_board[i][j];
                        p[1] = '\0';
                        
                        struct chess_piece_moves* piece = find_piece(p[0] < 97 ? p[0] + 32 : p[0]);

                        if (piece == NULL) {
                            draw_rectangle(0, test * 5, 5, 5, VGA_COLOR_RED);
                            test++;
                        }

                        points -= piece->value;     
                    }

                    for (int l = 0; l < threatened_pieces_len; l++) {
                        char p[2];
                        p[0] = chess_board[threatened_pieces[l][1]][threatened_pieces[l][0]];
                        p[1] = '\0';

                        if (is_protected(threatened_pieces[l][0], threatened_pieces[l][1], is_white, temp_board) > 0 && p[0] != 0) {
                            struct chess_piece_moves* piece = find_piece(p[0] < 97 ? p[0] + 32 : p[0]);

                            if (piece == NULL) {
                                draw_rectangle(0, test * 5, 5, 5, VGA_COLOR_WHITE);
                                test++;
                            }

                            points -= piece->value;
                        }
                    }

                    possible_moves(x, y, false, temp_board);

                    for (int l = 0; l < len_pm_pos; l++) {
                        int x2 = possible_moves_pos[l][0];
                        int y2 = possible_moves_pos[l][1];

                        if (is_protected(x2, y2, is_white, temp_board) > 0 && temp_board[y2][x2] != 0) {
                            points += 1;

                            if (temp_board[y2][x2] == 'K' || temp_board[y2][x2] == 'k') {
                                points += 2;
                            }
                        }
                    }

                    // i dont know if this is working, will keep it for now

                    // for (int l = 0; l < threatening_pieces_len; l++) {
                    //     char p[2];
                    //     p[0] = chess_board[threatening_pieces[l][1]][threatening_pieces[l][0]];
                    //     p[1] = '\0';

                    //     if (is_protected(threatening_pieces[l][0], threatening_pieces[l][1], is_white, temp_board) > 0 && p[0] != 0) {
                    //         struct chess_piece_moves* piece = find_piece(p[0] < 97 ? p[0] + 32 : p[0]);

                    //         if (piece == NULL) {
                    //             draw_rectangle(0, test * 5, 5, 5, VGA_COLOR_BROWN);
                    //             test++;
                    //         }

                    //         points += piece->value / 2;
                    //     }
                    // }

                    if ((prev_moves_white_len > 1 && is_white) || (prev_moves_black_len > 1 && !is_white)) {
                        int pm_from_x = 0;
                        int pm_from_y = 0;
                        int pm_to_x = 0;
                        int pm_to_y = 0;

                        int len = is_white ? prev_moves_white_len : prev_moves_black_len;

                        for (int l = 0; l < len; l++) {
                            if (is_white) {
                                pm_from_x = prev_moves_white[l][0][0];
                                pm_from_y = prev_moves_white[l][0][1];
                                pm_to_x = prev_moves_white[l][1][0];
                                pm_to_y = prev_moves_white[l][1][1];
                            } else {
                                pm_from_x = prev_moves_black[l][0][0];
                                pm_from_y = prev_moves_black[l][0][1];
                                pm_to_x = prev_moves_black[l][1][0];
                                pm_to_y = prev_moves_black[l][1][1];
                            }

                            if (pm_from_x == x && pm_from_y == y && pm_to_x == j && pm_to_y == i) {
                                points -= 2;
                            }
                        }
                    }


                    if ((chess_board[i][j] == 'K' && is_white) || (chess_board[i][j] == 'k' && !is_white)) {
                        if (x - j == -2 || x - j == 2) {
                            points += 2;
                        } else {
                            points -= 1;
                        }
                    } else if ((chess_board[i][j] == 'P' && is_white) || (chess_board[i][j] == 'p' && !is_white)) {
                        if (j < 5 && j > 2) {
                            points += 1;
                        } 
                        
                        int castle_thing = is_white ? 0 : 1;

                        if (castle_rights[castle_thing][1] && ((castle_rights[castle_thing][0] && j < 3) || (castle_rights[castle_thing][2] && j > 4))) {
                            points -= 1;
                        } else {
                            for (int l = 0; l < 3; l++) {
                                if ((((chess_board[i][l] == 'K' && is_white) || (chess_board[i][l] == 'k' && !is_white)) && j < 3) || (((chess_board[i][l + 5] == 'K' && is_white) || (chess_board[i][l + 5] == 'k' && !is_white)) && j > 4)) {
                                    points -= 2;
                                }
                            }
                        }
                    }

                    if (points_best_move <= points || !chosen_move) {
                        points_best_move = points;
                        best_move[0][0] = j;
                        best_move[0][1] = i;
                        best_move[1][0] = x;
                        best_move[1][1] = y;
                        chosen_move = true;
                    }

                    possible_moves(j, i, false, chess_board);
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





// This is an experimental version of the chess bot, that uses a different point system
// currently trying 10x

void chess_bot_experimental(bool is_white) {
    int points_best_move = 0;
    int best_move[2][2] = {{0}, {0}};
    bool chosen_move = false;

    int threatened_pieces[64][3] = {{0}, {0}, {0}};
    int threatened_pieces_len = 0;

    for (int i = 0; i < 8; i++) { // y
        for (int j = 0; j < 8; j++) { // x
            if ((chess_board[i][j] != 0 && chess_board[i][j] < 97 && is_white) || (chess_board[i][j] >= 97 && !is_white)) { 
                int protected = is_protected(j, i, is_white, chess_board);

                if (protected > 0) {
                    threatened_pieces[threatened_pieces_len][0] = j;
                    threatened_pieces[threatened_pieces_len][1] = i;
                    threatened_pieces[threatened_pieces_len][2] = protected;
                    threatened_pieces_len++;
                }
            } 
        }
    }

    // dont know if this is working, will keep it for now

    int threatening_pieces[64][3] = {{0}, {0}, {0}};
    int threatening_pieces_len = 0;

    for (int i = 0; i < 8; i++) { // y
        for (int j = 0; j < 8; j++) { // x
            if ((chess_board[i][j] < 97 && is_white) || (chess_board[i][j] < 97 && chess_board[i][j] != 0 && !is_white)) { 
                int protected = is_protected(j, i, !is_white, chess_board);

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
            if ((chess_board[i][j] != 0 && chess_board[i][j] < 97 && is_white) || (chess_board[i][j] >= 97 && !is_white)) {
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

                    temp_board[y][x] = chess_board[i][j];
                    temp_board[i][j] = 0;

                    if (chess_board[y][x] != 0) {
                        char p[2];
                        p[0] = chess_board[y][x];
                        p[1] = '\0';

                        struct chess_piece_moves* piece = find_piece(p[0] < 97 ? p[0] + 32 : p[0]);

                        if (piece == NULL) {
                            draw_rectangle(0, test * 5, 5, 5, VGA_COLOR_GREEN);
                            test++;
                        }

                        points += piece->value * 10;                    
                    }

                    int protected = is_protected(x, y, is_white, temp_board);

                    if (protected > 0) {
                        char p[2];
                        p[0] = chess_board[i][j];
                        p[1] = '\0';
                        
                        struct chess_piece_moves* piece = find_piece(p[0] < 97 ? p[0] + 32 : p[0]);

                        if (piece == NULL) {
                            draw_rectangle(0, test * 5, 5, 5, VGA_COLOR_RED);
                            test++;
                        }

                        points -= piece->value * 10;     
                    }

                    for (int l = 0; l < threatened_pieces_len; l++) {
                        char p[2];
                        p[0] = chess_board[threatened_pieces[l][1]][threatened_pieces[l][0]];
                        p[1] = '\0';

                        if (is_protected(threatened_pieces[l][0], threatened_pieces[l][1], is_white, temp_board) > 0 && p[0] != 0) {
                            struct chess_piece_moves* piece = find_piece(p[0] < 97 ? p[0] + 32 : p[0]);

                            if (piece == NULL) {
                                draw_rectangle(0, test * 5, 5, 5, VGA_COLOR_WHITE);
                                test++;
                            }

                            points -= piece->value * 5;
                        }
                    }

                    possible_moves(x, y, false, temp_board);

                    for (int l = 0; l < len_pm_pos; l++) {
                        int x2 = possible_moves_pos[l][0];
                        int y2 = possible_moves_pos[l][1];

                        if (is_protected(x2, y2, is_white, temp_board) > 0 && temp_board[y2][x2] != 0) {
                            points += 1;

                            if (temp_board[y2][x2] == 'K' || temp_board[y2][x2] == 'k') {
                                points += 2;
                            }
                        }
                    }

                    // i dont know if this is working, will keep it for now

                    for (int l = 0; l < threatening_pieces_len; l++) {
                        char p[2];
                        p[0] = chess_board[threatening_pieces[l][1]][threatening_pieces[l][0]];
                        p[1] = '\0';

                        if (is_protected(threatening_pieces[l][0], threatening_pieces[l][1], is_white, temp_board) > 0 && p[0] != 0) {
                            struct chess_piece_moves* piece = find_piece(p[0] < 97 ? p[0] + 32 : p[0]);

                            if (piece == NULL) {
                                draw_rectangle(0, test * 5, 5, 5, VGA_COLOR_BROWN);
                                test++;
                            }

                            points += piece->value / 2;
                        }
                    }

                    if ((prev_moves_white_len > 1 && is_white) || (prev_moves_black_len > 1 && !is_white)) {
                        int pm_from_x = 0;
                        int pm_from_y = 0;
                        int pm_to_x = 0;
                        int pm_to_y = 0;

                        int len = is_white ? prev_moves_white_len : prev_moves_black_len;

                        for (int l = 0; l < len; l++) {
                            if (is_white) {
                                pm_from_x = prev_moves_white[l][0][0];
                                pm_from_y = prev_moves_white[l][0][1];
                                pm_to_x = prev_moves_white[l][1][0];
                                pm_to_y = prev_moves_white[l][1][1];
                            } else {
                                pm_from_x = prev_moves_black[l][0][0];
                                pm_from_y = prev_moves_black[l][0][1];
                                pm_to_x = prev_moves_black[l][1][0];
                                pm_to_y = prev_moves_black[l][1][1];
                            }

                            if (pm_from_x == x && pm_from_y == y && pm_to_x == j && pm_to_y == i) {
                                points -= 2;
                            }
                        }
                    }


                    if ((chess_board[i][j] == 'K' && is_white) || (chess_board[i][j] == 'k' && !is_white)) {
                        if (x - j == -2 || x - j == 2) {
                            points += 2;
                        } else {
                            points -= 1;
                        }
                    } else if ((chess_board[i][j] == 'P' && is_white) || (chess_board[i][j] == 'p' && !is_white)) {
                        if (j < 5 && j > 2) {
                            points += 1;
                        } 
                        
                        int castle_thing = is_white ? 0 : 1;

                        if (castle_rights[castle_thing][1] && ((castle_rights[castle_thing][0] && j < 3) || (castle_rights[castle_thing][2] && j > 4))) {
                            points -= 1;
                        } else {
                            for (int l = 0; l < 3; l++) {
                                if ((((chess_board[i][l] == 'K' && is_white) || (chess_board[i][l] == 'k' && !is_white)) && j < 3) || (((chess_board[i][l + 5] == 'K' && is_white) || (chess_board[i][l + 5] == 'k' && !is_white)) && j > 4)) {
                                    points -= 2;
                                }
                            }
                        }
                    }

                    if (points_best_move <= points || !chosen_move) {
                        points_best_move = points;
                        best_move[0][0] = j;
                        best_move[0][1] = i;
                        best_move[1][0] = x;
                        best_move[1][1] = y;
                        chosen_move = true;
                    }

                    possible_moves(j, i, false, chess_board);
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