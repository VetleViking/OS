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
                            draw_rectangle(0, test * 5, 5, 5, VGA_COLOR_GREEN, false);
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
                            draw_rectangle(0, test * 5, 5, 5, VGA_COLOR_RED, false);
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
                                draw_rectangle(0, test * 5, 5, 5, VGA_COLOR_WHITE, false);
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
    
    draw_rectangle(0, 0, 60, 200, VGA_COLOR_BLACK, false);


    // this is for testing, prints the points and the sources for the points of the best move

    print_whole_num(0, 0, points_best_move);

    for (int i = 0; i < 10; i++) {
        print_whole_num(0, 20 + (i * 20), test3[i]);
    }

    // 0 total
    // 1 + (taking piece), 2 - (moving pieces threatened)
    //3 - (other of bots pieces threatened), 4 + (enemies pieces threatened)
    // 5 + (if piece moving is threatening pieces), 6 - (repeat move)
    // 7 -/+ (castling / king moving), 8 + (middle pawn moving / pawn moving two), 9 + (king protecting bonus)

    possible_moves(chosen_piece_pos[0], chosen_piece_pos[1], false, chess_board);

    move_piece(best_move[1][0], best_move[1][1]);  
}

// points for the bot, makes it easier to tweak the bot
// currently all pieces are 10x points, and other things are calculated from that

int take_lose_multiplier_exp = 10; // subtracts 1 if taking a piece, so that the move is beneficial, not just neutral
int threaten_multiplier_exp = 1; 
int castling_bonus_exp = 9;
int repeat_move_penalty_exp = 5;
int check_bonus_exp = 8;
int king_move_penalty_exp = 1;
int king_protect_bonus_exp = 2;
int pawn_move_bonus_exp = 1;

int test3_exp[10] = {0};
int test2_exp[10] = {0};


int taking_piece(int x, int y, char board[8][8], bool is_white) {
    int points = 0;

    if (board[y][x] != 0) {
        char p[2];
        p[0] = board[y][x];
        p[1] = '\0';

        struct chess_piece_moves* piece = find_piece(p[0] < 97 ? p[0] + 32 : p[0]);

        if (piece == NULL) {
            draw_rectangle(0, test * 5, 5, 5, VGA_COLOR_GREEN, false);
            test++;
        }

        // subtracts 1 if taking a piece, so that the move is beneficial, not just neutral
        points += piece->value * take_lose_multiplier_exp - 1;
        test2_exp[0] += piece->value * take_lose_multiplier_exp - 1;
    }

    return points;
}


// checks if the bots pieces are protected
int pieces_protected(char board[8][8], bool is_white) {
    int points = 0;

    for (int i = 0; i < 8; i++) { // y
        for (int j = 0; j < 8; j++) { // x
            if (board[i][j] != 0 && ((board[i][j] < 97 && is_white) || (board[i][j] >= 97 && !is_white))) {
                piece_protected(j, i, board, is_white);
            }
        }
    }

    return points;
}


// checks if a piece is protected
int piece_protected(int x, int y, char board[8][8], bool is_white) {
    int points = 0;

    // finds the pieces threatening and defending the piece
    piece_threatened_by(x, y, is_white, board);
    int temp_threatened_by_len = threatened_by_len;
    piece_threatened_by(x, y, !is_white, board);

    struct chess_piece_moves* piece_taken = find_piece(board[y][x] < 97 ? board[y][x] + 32 : board[y][x]);

    if (piece_taken == NULL) {
        draw_rectangle(0, test * 5, 5, 5, VGA_COLOR_GREEN, false);
        test++;
    }

    // if the piece is threatened by more pieces than it is protected by
    if (temp_threatened_by_len > threatened_by_len) {
        points -= piece_taken->value * threaten_multiplier_exp;
        test2_exp[2] += piece_taken->value * threaten_multiplier_exp;

        return points;
    }

    // finds the pieces threatening the piece, then loops through them
    piece_threatened_by(x, y, is_white, board);

    for (int i = 0; i < threatened_by_len; i++) {
        int x2 = threatened_by[i][0];
        int y2 = threatened_by[i][1];

        struct chess_piece_moves* piece_taking = find_piece(board[y2][x2] < 97 ? board[y2][x2] + 32 : board[y2][x2]);
        
        if (piece_taking == NULL) {
            draw_rectangle(0, test * 5, 5, 5, VGA_COLOR_BLUE, false);
            test++;
        }

        // if the piece taking is worth less than the piece taken
        // looses points equal to the difference in value
        if (piece_taking->value < piece_taken->value) {
            if ((points * -1) / take_lose_multiplier_exp < piece_taken->value - piece_taking->value) {
                points -= (piece_taken->value - piece_taking->value) * take_lose_multiplier_exp;
                test2_exp[1] += (piece_taken->value - piece_taking->value) * take_lose_multiplier_exp;
            }
        }
    }

    return points;
}

// needs updating, probably not working
// checks if the other players pieces are threatened
int other_players_threatened(char board[8][8], bool is_white) {
    int points = 0;

    for (int i = 0; i < 8; i++) { // y
        for (int j = 0; j < 8; j++) { // x
            if (board[i][j] != 0 && ((board[i][j] < 97 && is_white) || (board[i][j] >= 97 && !is_white))) {

                possible_moves(j, i, false, board);

                // saves possible moves to a temp array
                // so that the original array can be used in the next loop              
                int possible_moves_pos_temp[64][2] = {{0}, {0}};
                int len_pm_pos_temp = len_pm_pos;

                for (int k = 0; k < len_pm_pos; k++) {
                    possible_moves_pos_temp[k][0] = possible_moves_pos[k][0];
                    possible_moves_pos_temp[k][1] = possible_moves_pos[k][1];
                }
                
                // loops through the possible moves saved in the temp array
                for (int k = 0; k < len_pm_pos_temp; k++) {
                    int x2 = possible_moves_pos_temp[k][0];
                    int y2 = possible_moves_pos_temp[k][1];
                
                    if (board[y2][x2] != 0 && ((board[y2][x2] < 97 && !is_white) || (board[y2][x2] >= 97 && is_white))) {
                        // checks if the piece that is moving is threatening pieces
                        int protected = is_protected(x2, y2, !is_white, board);

                        if (protected > 0) {
                            if (board[y2][x2] == 'K' || board[y2][x2] == 'k') { 

                                if (check_mate(!is_white, board)) {
                                    points += 1000; // checkmate
                                    test2_exp[4] += 1000;
                                } else {
                                    points += check_bonus_exp; // check
                                    test2_exp[4] += check_bonus_exp;
                                }
                            } else {
                                char p[2];
                                p[0] = chess_board[y2][x2];
                                p[1] = '\0';

                                struct chess_piece_moves* piece = find_piece(p[0] < 97 ? p[0] + 32 : p[0]);

                                points += piece->value * threaten_multiplier_exp;
                                test2_exp[4] += piece->value * threaten_multiplier_exp;
                            }
                        }
                    }
                }
            }
        }
    }

    return points;
}


int repeat_moves(int x, int y, int from_y, int from_x, bool is_white) {
    int points = 0;

    if ((prev_moves_white_len > 1 && is_white) || (prev_moves_black_len > 1 && !is_white)) {
        int pm_from_x = 0;
        int pm_from_y = 0;
        int pm_to_x = 0;
        int pm_to_y = 0;

        int len = is_white ? prev_moves_white_len : prev_moves_black_len;

        for (int from_y = 0; from_y < len; from_y++) {
            if (is_white) {
                pm_from_x = prev_moves_white[from_y][0][0];
                pm_from_y = prev_moves_white[from_y][0][1];
                pm_to_x = prev_moves_white[from_y][1][0];
                pm_to_y = prev_moves_white[from_y][1][1];
            } else {
                pm_from_x = prev_moves_black[from_y][0][0];
                pm_from_y = prev_moves_black[from_y][0][1];
                pm_to_x = prev_moves_black[from_y][1][0];
                pm_to_y = prev_moves_black[from_y][1][1];
            }

            if (pm_from_x == x && pm_from_y == y && pm_to_x == from_x && pm_to_y == from_y) {
                points -= repeat_move_penalty_exp;
                test2_exp[5] += repeat_move_penalty_exp;
            }
        }
    }

    return points;
}

int other_bonus_penalties(int x, int y, int from_y, int from_x, char temp_board[8][8]) {
    int points = 0;
    
    if (chess_board[from_y][from_x] == 'K' || chess_board[from_y][from_x] == 'k') {
        if (x - from_x == -2 || x - from_x == 2) {
            points += castling_bonus_exp;
            test2_exp[6] += castling_bonus_exp;
        } else {
            points -= king_move_penalty_exp;
            test2_exp[6] += king_move_penalty_exp;
        }
    } else if (chess_board[from_y][from_x] == 'P' || chess_board[from_y][from_x] == 'p') {
        if (from_x < 5 && from_x > 2) {
            points += pawn_move_bonus_exp;
            test2_exp[7] += pawn_move_bonus_exp;
        }

        if (from_y - y == 2 || y - from_y == 2) {
            points += pawn_move_bonus_exp;
            test2_exp[7] += pawn_move_bonus_exp;
        }
    }

    return points;
}


// This is an experimental version of the chess bot
void chess_bot_experimental(bool is_white) {
    int points_best_move = 0;
    int best_move[2][2] = {{0}, {0}};
    bool chosen_move = false;
    
    for (int i = 0; i < 10; i++) {
        test3_exp[i] = 0;
    }

    for (int i = 0; i < 8; i++) { // y
        for (int j = 0; j < 8; j++) { // x
            if ((chess_board[i][j] != 0 && chess_board[i][j] < 97 && is_white) || (chess_board[i][j] >= 97 && !is_white)) {
                chosen_piece_pos[0] = j;
                chosen_piece_pos[1] = i;

                int from_x = j;
                int from_y = i;

                possible_moves(from_x, from_y, false, chess_board);

                for (int k = 0; k < len_pm_pos; k++) {
                    int x = possible_moves_pos[k][0];
                    int y = possible_moves_pos[k][1];
                    int points = 0;
                    
                    for (int l = 0; l < 10; l++) {
                        test2_exp[l] = 0;
                    }

                    char temp_board[8][8] = {0};

                    for (int l = 0; l < 8; l++) {
                        for (int m = 0; m < 8; m++) {
                            temp_board[m][l] = chess_board[m][l];
                        }
                    }

                    temp_board[y][x] = chess_board[from_y][from_x];
                    temp_board[from_y][from_x] = 0;

                    // calculating points for the move
                    points += taking_piece(x, y, chess_board, is_white);
                    points += pieces_protected(temp_board, is_white);
                    points += other_players_threatened(temp_board, is_white);
                    points += repeat_moves(x, y, from_y, from_x, is_white);
                    points += other_bonus_penalties(x, y, from_y, from_x, temp_board);
                    
                    if (points_best_move <= points || !chosen_move) {
                        points_best_move = points;
                        best_move[0][0] = from_x;
                        best_move[0][1] = from_y;
                        best_move[1][0] = x;
                        best_move[1][1] = y;
                        chosen_move = true;

                        for (int l = 0; l < 10; l++) {
                            test3_exp[l] = test2_exp[l];
                        }
                    }

                    possible_moves(from_x, from_y, false, chess_board);
                }              
            }
        }   
    }

    chosen_piece_pos[0] = best_move[0][0];
    chosen_piece_pos[1] = best_move[0][1];
    chosen_piece = true;
    
    draw_rectangle(0, 0, 60, 200, VGA_COLOR_BLACK, false);


    // this is for testing, prints the points and the sources for the points of the best move

    print_whole_num(0, 0, points_best_move);

    for (int i = 0; i < 10; i++) {
        print_whole_num(0, 20 + (i * 20), test3_exp[i]);
    }

    // 0 total
    // 1 + (taking piece), 2 - (bots pieces threatened)
    //3 (not used), 4 + (not used)
    // 5 + (enemies pieces threatened), 6 - (repeat move)
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
            draw_rectangle(x + (i * 10), y, 10, 20, VGA_COLOR_BLACK, false);
            draw_rectangle(x + (i * 10), y + 8, 9, 2, VGA_COLOR_WHITE, false);
        } else {
            print_num(x + (i * 10), y, p[i] - '0');
        }
    }
}


void print_num(int x, int y, int number) {
    bool use_buffer = true;

    draw_rectangle(x, y, 10, 20, VGA_COLOR_BLACK, use_buffer);

    switch (number) {
        case 0:
            draw_rectangle(x, y, 9, 18, VGA_COLOR_WHITE, use_buffer);
            draw_rectangle(x + 2, y + 2, 5, 14, VGA_COLOR_BLACK, use_buffer);            
            break;

        case 1:
            draw_rectangle(x + 7, y, 2, 18, VGA_COLOR_WHITE, use_buffer);            
            break;

        case 2:
            draw_rectangle(x, y, 9, 2, VGA_COLOR_WHITE, use_buffer);
            draw_rectangle(x + 7, y, 2, 10, VGA_COLOR_WHITE, use_buffer);
            draw_rectangle(x, y + 8, 9, 2, VGA_COLOR_WHITE, use_buffer);
            draw_rectangle(x, y + 9, 2, 9, VGA_COLOR_WHITE, use_buffer);
            draw_rectangle(x, y + 16, 9, 2, VGA_COLOR_WHITE, use_buffer);
            break;

        case 3:
            draw_rectangle(x, y, 9, 2, VGA_COLOR_WHITE, use_buffer);
            draw_rectangle(x, y + 8, 9, 2, VGA_COLOR_WHITE, use_buffer);
            draw_rectangle(x, y + 16, 9, 2, VGA_COLOR_WHITE, use_buffer);
            draw_rectangle(x + 7, y, 2, 18, VGA_COLOR_WHITE, use_buffer);
            break;

        case 4: 
            draw_rectangle(x, y, 2, 9, VGA_COLOR_WHITE, use_buffer);
            draw_rectangle(x + 7, y, 2, 18, VGA_COLOR_WHITE, use_buffer);
            draw_rectangle(x, y + 8, 9, 2, VGA_COLOR_WHITE, use_buffer);
            break;

        case 5:
            draw_rectangle(x, y, 9, 2, VGA_COLOR_WHITE, use_buffer);
            draw_rectangle(x, y, 2, 9, VGA_COLOR_WHITE, use_buffer);
            draw_rectangle(x, y + 8, 9, 2, VGA_COLOR_WHITE, use_buffer);
            draw_rectangle(x + 7, y + 9, 2, 9, VGA_COLOR_WHITE, use_buffer);
            draw_rectangle(x, y + 16, 9, 2, VGA_COLOR_WHITE, use_buffer);
            break;

        case 6:
            draw_rectangle(x, y, 9, 2, VGA_COLOR_WHITE, use_buffer);
            draw_rectangle(x, y, 2, 18, VGA_COLOR_WHITE, use_buffer);
            draw_rectangle(x, y + 8, 9, 2, VGA_COLOR_WHITE, use_buffer);
            draw_rectangle(x + 7, y + 9, 2, 9, VGA_COLOR_WHITE, use_buffer);
            draw_rectangle(x, y + 16, 9, 2, VGA_COLOR_WHITE, use_buffer);
            break;

        case 7:
            draw_rectangle(x, y, 9, 2, VGA_COLOR_WHITE, use_buffer);
            draw_rectangle(x + 7, y, 2, 18, VGA_COLOR_WHITE, use_buffer);
            break;

        case 8:
            draw_rectangle(x, y, 9, 18, VGA_COLOR_WHITE, use_buffer);
            draw_rectangle(x + 2, y + 2, 5, 14, VGA_COLOR_BLACK, use_buffer); 
            draw_rectangle(x, y + 8, 9, 2, VGA_COLOR_WHITE, use_buffer);
            break;

        case 9:
            draw_rectangle(x, y, 9, 2, VGA_COLOR_WHITE, use_buffer);
            draw_rectangle(x + 7, y, 2, 18, VGA_COLOR_WHITE, use_buffer);
            draw_rectangle(x, y + 8, 9, 2, VGA_COLOR_WHITE, use_buffer);
            draw_rectangle(x, y, 2, 9, VGA_COLOR_WHITE, use_buffer);
            draw_rectangle(x, y + 16, 9, 2, VGA_COLOR_WHITE, use_buffer);
            break;
        default:
            draw_rectangle(x, y, 9, 18, VGA_COLOR_RED, use_buffer);
            break;
    };
}