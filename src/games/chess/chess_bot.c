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


    // if other of bots pieces are threatened
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
    // if enemy pieces are threatened
    int threatening_pieces[64][3] = {{0}, {0}, {0}};
    int threatening_pieces_len = 0;

    for (int i = 0; i < 8; i++) { // y
        for (int j = 0; j < 8; j++) { // x
            if ((chess_board[i][j] >= 97 && is_white) || (chess_board[i][j] < 97 && chess_board[i][j] != 0 && !is_white)) { 
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

    // points for the bot, makes it easier to tweak the bot
    // currently all pieces are 10x points, and other things are calculated from that

    int take_lose_multiplier = 10; // subtracts 1 if taking a piece, so that the move is beneficial, not just neutral
    int threaten_bonus = 6; 
    int castling_bonus = 9;
    int repeat_move_penalty = 5;
    int check_bonus = 8;
    int king_move_penalty = 1;
    int king_protect_bonus = 2;
    int pawn_move_bonus = 1;

    
    int test3[10] = {0};

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
                    int test2[10] = {0};

                    char temp_board[8][8] = {0};

                    for (int l = 0; l < 8; l++) {
                        for (int m = 0; m < 8; m++) {
                            temp_board[m][l] = chess_board[m][l];
                        }
                    }

                    temp_board[y][x] = chess_board[i][j];
                    temp_board[i][j] = 0;

                    // taking a piece
                    if (chess_board[y][x] != 0) {
                        char p[2];
                        p[0] = chess_board[y][x];
                        p[1] = '\0';

                        struct chess_piece_moves* piece = find_piece(p[0] < 97 ? p[0] + 32 : p[0]);

                        if (piece == NULL) {
                            draw_rectangle(0, test * 5, 5, 5, VGA_COLOR_GREEN);
                            test++;
                        }

                        points += piece->value * take_lose_multiplier - 1;
                        test2[0] += piece->value * take_lose_multiplier - 1;
                    }

                    // if the piece that is moving is protected
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

                        points -= piece->value * take_lose_multiplier;    
                        test2[1] += piece->value * take_lose_multiplier;
                    }

                    // if other of bots pieces are threatened
                    // info gotten higher before the loop
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

                            points -= piece->value * take_lose_multiplier;
                            test2[2] += piece->value * take_lose_multiplier;
                        }
                    }

                    
                    // i dont know if this is working, will keep it for now

                    // if enemys pieces are threatened
                    // info gotten higher before the loop
                    for (int l = 0; l < threatening_pieces_len; l++) {
                        char p[2];
                        p[0] = chess_board[threatening_pieces[l][1]][threatening_pieces[l][0]];
                        p[1] = '\0';

                        int protected = is_protected(threatening_pieces[l][0], threatening_pieces[l][1], is_white, temp_board);

                        if (protected >= threatening_pieces[l][2] && p[0] != 0) {
                            if (p[0] == 'K' || p[0] == 'k') {
                                possible_moves(threatening_pieces[l][0], threatening_pieces[l][1], false, temp_board);
                                if (len_pm_pos == 0) {
                                    points += 1000; // checkmate
                                    test2[3] += 1000;
                                } else {
                                    points += check_bonus; // check
                                    test2[3] += check_bonus;
                                }
                            } else {
                                points += threaten_bonus;
                                test2[3] += threaten_bonus;
                            }
                        }
                    }



                    // if the piece that is moving is threatening pieces
                    possible_moves(x, y, false, temp_board);

                    int possible_moves_pos_temp[64][2] = {{0}, {0}};
                    int len_pm_pos_temp = len_pm_pos;

                    for (int l = 0; l < len_pm_pos; l++) {
                        possible_moves_pos_temp[l][0] = possible_moves_pos[l][0];
                        possible_moves_pos_temp[l][1] = possible_moves_pos[l][1];
                    }
                    
                    for (int l = 0; l < len_pm_pos_temp; l++) {
                        int x2 = possible_moves_pos_temp[l][0];
                        int y2 = possible_moves_pos_temp[l][1];

                        int protected = is_protected(x2, y2, !is_white, temp_board);

                        if (protected > 0 && temp_board[y2][x2] != 0) {
                            if (temp_board[y2][x2] == 'K' || temp_board[y2][x2] == 'k') { 
                                char temp_temp_board[8][8] = {0};

                                for (int m = 0; m < 8; m++) {
                                    for (int n = 0; n < 8; n++) {
                                        temp_temp_board[n][m] = temp_board[n][m];
                                    }
                                }

                                if (check_mate(!is_white, temp_temp_board)) {
                                    points += 1000; // checkmate
                                    test2[4] += 1000;
                                } else {
                                    points += check_bonus; // check
                                    test2[4] += check_bonus;
                                }
                            } else {
                                points += threaten_bonus;
                                test2[4] += threaten_bonus;
                            }
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
                                points -= repeat_move_penalty;
                                test2[5] += repeat_move_penalty;
                            }
                        }
                    }


                    if (chess_board[i][j] == 'K' || chess_board[i][j] == 'k') {
                        if (x - j == -2 || x - j == 2) {
                            points += castling_bonus;
                            test2[6] += castling_bonus;
                        } else {
                            points -= king_move_penalty;
                            test2[6] += king_move_penalty;
                        }
                    } else if (chess_board[i][j] == 'P' || chess_board[i][j] == 'p') {
                        if (j < 5 && j > 2) {
                            points += pawn_move_bonus;
                            test2[7] += pawn_move_bonus;
                        }

                        if (i - y == 2 || y - i == 2) {
                            points += pawn_move_bonus;
                            test2[7] += pawn_move_bonus;
                        }
                    }

                    // checks if the king is protected if it has castled
                    if (have_castled[0] || have_castled[1]) {
                        int king_x = 0;
                        int king_y = 0;

                        for (int l = 0; l < 8; l++) {
                            for (int m = 0; m < 8; m++) {
                                if ((temp_board[l][m] == 'K' && is_white) || (temp_board[l][m] == 'k' && !is_white)) {
                                    king_x = m;
                                    king_y = l;
                                }
                            }
                        }

                        for (int l = king_y - 1; l < king_y + 2; l++) {
                            for (int m = king_x - 1; m < king_x + 2; m++) {
                                if (l >= 0 && l < 8 && m >= 0 && m < 8) {
                                    if ((temp_board[l][m] != 0 && temp_board[l][m] < 97 && is_white) || (temp_board[l][m] >= 97 && !is_white) && (l != king_y || m != king_x)) {
                                        if (temp_board[l][m] == 'P' || temp_board[l][m] == 'p') {
                                            points += king_protect_bonus;
                                            test2[8] += king_protect_bonus;
                                        }
                                    }
                                } else {
                                    points += king_protect_bonus;
                                    test2[8] += king_protect_bonus;
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

                        for (int l = 0; l < 10; l++) {
                            test3[l] = test2[l];
                        }
                    }

                    possible_moves(j, i, false, chess_board);
                }              
            }
        }   
    }

    chosen_piece_pos[0] = best_move[0][0];
    chosen_piece_pos[1] = best_move[0][1];
    chosen_piece = true;
    
    draw_rectangle(0, 0, 60, 200, VGA_COLOR_BLACK);


    // this is for testing, prints the points and the sources for the points of the best move

    print_whole_num(0, 0, points_best_move);

    for (int i = 0; i < 10; i++) {
        print_whole_num(0, 20 + (i * 20), test3[i]);
    }

    // 1 + (taking piece), 2 - (moving piece threatened)
    //3 - (other of bots pieces threatened), 4 + (enemies pieces threatened)
    // 5 + (if piece moving is threatening pieces), 6 - (repeat move)
    // 7 -/+ (castling / king moving), 8 + (middle pawn moving / pawn moving two), 9 + (king protecting bonus)


    possible_moves(chosen_piece_pos[0], chosen_piece_pos[1], false, chess_board);

    move_piece(best_move[1][0], best_move[1][1]);  
}


void print_whole_num(int x, int y, int number) {
    char p[10];
    itoa(number, p, 10);

    for (int i = 0; i < 10; i++) {
        if (p[i] == '\0') {
            break; 
        } else if (p[i] == '-') {
            draw_rectangle(x + (i * 10), y, 10, 20, VGA_COLOR_BLACK);
            draw_rectangle(x + (i * 10), y + 8, 9, 2, VGA_COLOR_WHITE);
        } else {
            print_num(x + (i * 10), y, p[i] - '0');
        }
    }
}
// 1 +, 2 -, 3 -, 4 +, 5 +, 6 -, 7 -/+, 8 +, 9 +


void print_num(int x, int y, int number) {
    draw_rectangle(x, y, 10, 20, VGA_COLOR_BLACK);

    switch (number) {
        case 0:
            draw_rectangle(x, y, 9, 18, VGA_COLOR_WHITE);
            draw_rectangle(x + 2, y + 2, 5, 14, VGA_COLOR_BLACK);            
            break;

        case 1:
            draw_rectangle(x + 7, y, 2, 18, VGA_COLOR_WHITE);            
            break;

        case 2:
            draw_rectangle(x, y, 9, 2, VGA_COLOR_WHITE);
            draw_rectangle(x + 7, y, 2, 10, VGA_COLOR_WHITE);
            draw_rectangle(x, y + 8, 9, 2, VGA_COLOR_WHITE);
            draw_rectangle(x, y + 9, 2, 9, VGA_COLOR_WHITE);
            draw_rectangle(x, y + 16, 9, 2, VGA_COLOR_WHITE);
            break;

        case 3:
            draw_rectangle(x, y, 9, 2, VGA_COLOR_WHITE);
            draw_rectangle(x, y + 8, 9, 2, VGA_COLOR_WHITE);
            draw_rectangle(x, y + 16, 9, 2, VGA_COLOR_WHITE);
            draw_rectangle(x + 7, y, 2, 18, VGA_COLOR_WHITE);
            break;

        case 4: 
            draw_rectangle(x, y, 2, 9, VGA_COLOR_WHITE);
            draw_rectangle(x + 7, y, 2, 18, VGA_COLOR_WHITE);
            draw_rectangle(x, y + 8, 9, 2, VGA_COLOR_WHITE);
            break;

        case 5:
            draw_rectangle(x, y, 9, 2, VGA_COLOR_WHITE);
            draw_rectangle(x, y, 2, 9, VGA_COLOR_WHITE);
            draw_rectangle(x, y + 8, 9, 2, VGA_COLOR_WHITE);
            draw_rectangle(x + 7, y + 9, 2, 9, VGA_COLOR_WHITE);
            draw_rectangle(x, y + 16, 9, 2, VGA_COLOR_WHITE);
            break;

        case 6:
            draw_rectangle(x, y, 9, 2, VGA_COLOR_WHITE);
            draw_rectangle(x, y, 2, 18, VGA_COLOR_WHITE);
            draw_rectangle(x, y + 8, 9, 2, VGA_COLOR_WHITE);
            draw_rectangle(x + 7, y + 9, 2, 9, VGA_COLOR_WHITE);
            draw_rectangle(x, y + 16, 9, 2, VGA_COLOR_WHITE);
            break;

        case 7:
            draw_rectangle(x, y, 9, 2, VGA_COLOR_WHITE);
            draw_rectangle(x + 7, y, 2, 18, VGA_COLOR_WHITE);
            break;

        case 8:
            draw_rectangle(x, y, 9, 18, VGA_COLOR_WHITE);
            draw_rectangle(x + 2, y + 2, 5, 14, VGA_COLOR_BLACK); 
            draw_rectangle(x, y + 8, 9, 2, VGA_COLOR_WHITE);
            break;

        case 9:
            draw_rectangle(x, y, 9, 2, VGA_COLOR_WHITE);
            draw_rectangle(x + 7, y, 2, 18, VGA_COLOR_WHITE);
            draw_rectangle(x, y + 8, 9, 2, VGA_COLOR_WHITE);
            draw_rectangle(x, y, 2, 9, VGA_COLOR_WHITE);
            draw_rectangle(x, y + 16, 9, 2, VGA_COLOR_WHITE);
            break;
        default:
            draw_rectangle(x, y, 9, 18, VGA_COLOR_RED);
            break;
    };
}