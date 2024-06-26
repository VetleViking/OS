#ifndef __CHESS_H
#define __CHESS_H

// these are the things the chess bot needs from the chess game


extern char chess_board[8][8];
extern int chosen_piece_pos[2];
extern int len_pm_pos;
extern int possible_moves_pos[64][2];
extern int test;
extern int prev_moves_white[256][2][2];
extern int prev_moves_black[256][2][2];
extern int prev_moves_white_len;
extern int prev_moves_black_len;
extern bool castle_rights[2][3];
extern bool chosen_piece;
extern bool have_castled[2];

extern struct chess_piece_moves {
    bool straight_or_diagonal [2];
    int moves [8][2];
    bool is_pawn;
    bool is_king;
    int value;
};

extern int threatened_by_len;
extern int threatened_by[64][2];
extern int protected_by_len;
extern int protected_by[64][2];

extern void possible_moves(int x, int y, bool show_result, char board[8][8], bool is_white);
extern int is_protected(int x, int y, bool is_white, char board[8][8]);
extern bool check_mate(bool king_white, char board[8][8]);
extern void move_piece(int x, int y);
extern void chess_print_board(char board[8][8]);
extern struct chess_piece_moves* find_piece(char piece);
extern void piece_threatened_by(int x, int y, bool is_white, char board[8][8]);
extern void piece_protected_by(int x, int y, bool is_white, char board[8][8]);
extern void chess_bot_experimental(bool is_white, char board[8][8]);
extern void chess_bot(bool is_white);
extern void chess_bot_experimental_depth(bool is_white, char board[8][8], int depth);

#endif