#include <system.h>

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>


char tic_tac_toe_board[5][5] = {
	{' ', '|', ' ', '|', ' '},
	{'-', '+', '-', '+', '-'},
	{' ', '|', ' ', '|', ' '},
	{'-', '+', '-', '+', '-'},
	{' ', '|', ' ', '|', ' '}
};

int tic_tac_toe_occupied[5][5] = {
	{0, 1, 0, 1, 0},
	{1, 1, 1, 1, 1},
	{0, 1, 0, 1, 0},
	{1, 1, 1, 1, 1},
	{0, 1, 0, 1, 0}
};


// Prints the current tic tac toe board
void print_tic_tac_toe_board() {
	for (int i = 0; i < 5; i++) {
		for (int j = 0; j < 5; j++) {	
			char str[2] = {tic_tac_toe_board[i][j], '\0'};
			terminal_writestring(str);
		}
		if (i != 4) {
			newline();
		}
	}
}


// Called at the end of a game of tic tac toe, resets the board
void end_tic_tac_toe() {
	for (int i = 0; i < 5; i = i + 2) {
		tic_tac_toe_board[i][0] = ' ';
		tic_tac_toe_board[i][2] = ' ';
		tic_tac_toe_board[i][4] = ' ';
		tic_tac_toe_occupied[i][0] = 0;
		tic_tac_toe_occupied[i][2] = 0;
		tic_tac_toe_occupied[i][4] = 0;
	}

	game_round = 1;
	in_game = false;
}


// Checks if someone has won in tic tac toe
// Returns 1 if the player has won, 2 if the pc has won and 0 if no one has won
int check_for_win_ttt() {
	for (size_t i = 0; i < 5; i = i + 2) {
		if (tic_tac_toe_board[i][0] == tic_tac_toe_board[i][2] && tic_tac_toe_board[i][2] == tic_tac_toe_board[i][4]) {
			if (tic_tac_toe_board[i][0] == 'X') {
				return 1;
			} else if (tic_tac_toe_board[i][0] == 'O') {
				return 2;
			}
		}

		if (tic_tac_toe_board[0][i] == tic_tac_toe_board[2][i] && tic_tac_toe_board[2][i] == tic_tac_toe_board[4][i]) {
			if (tic_tac_toe_board[0][i] == 'X') {
				return 1;
			} else if (tic_tac_toe_board[0][i] == 'O') {
				return 2;
			}
		}
	}
	if ((tic_tac_toe_board[0][0] == tic_tac_toe_board[2][2] && tic_tac_toe_board[2][2] == tic_tac_toe_board[4][4]) || (tic_tac_toe_board[0][4] == tic_tac_toe_board[2][2] && tic_tac_toe_board[2][2] == tic_tac_toe_board[4][0])) {
		if (tic_tac_toe_board[2][2] == 'X') {
			return 1;
		} else if (tic_tac_toe_board[2][2] == 'O') {
			return 2;
		}
	}
	
	return 0;
}


// Tic tac toe game TODO: make the pc better at the game
void tic_tac_toe() {
	if (game_round == 1) {
		terminal_writestring("Tic tac toe, you go first!");
		newline();
		terminal_writestring("Type the coordinates of the square you want to place your X in");
		newline();
		terminal_writestring("(for example 1,1 for upper left)");
		newline();
		terminal_writestring("Current board:");
		newline();
		print_tic_tac_toe_board();

		game_round++;
		return;

	} else {
		int x = command[0] - '0';
		int y = command[2] - '0';

		if (x == 1) {
			x = 0;
		} else if (x == 3) {
			x = 4;
		} else if (x != 2) {
			print_tic_tac_toe_board();
			newline();
			terminal_writestring("That is not a valid move, Try again.");
			return;
		}

		if (y == 1) {
			y = 0;
		} else if (y == 3) {
			y = 4;
		} else if (y != 2) {
			print_tic_tac_toe_board();
			newline();
			terminal_writestring("That is not a valid move, Try again.");
			return;
		}

		if (tic_tac_toe_occupied[x][y] == 1) {
			print_tic_tac_toe_board();
			newline();
			terminal_writestring("That place is occupied, Try again.");
			return;
		}
		tic_tac_toe_board[x][y] = 'X';
		tic_tac_toe_occupied[x][y] = 1;

		check_for_win_ttt();
		if (check_for_win_ttt() == 1) {
			print_tic_tac_toe_board();
			newline();
			terminal_writestring("You win!");
			end_tic_tac_toe();
			return;
		} else if (check_for_win_ttt() == 2) {
			print_tic_tac_toe_board();
			newline();
			terminal_writestring("You lose!");
			end_tic_tac_toe();
			return;
		}

		bool placed = false;
		for (size_t i = 0; i < 5; i = i + 2) {
			for (size_t j = 0; j < 5; j = j + 2) {
				if (tic_tac_toe_occupied[i][j] == 0) {
					tic_tac_toe_board[i][j] = 'O';
					tic_tac_toe_occupied[i][j] = 1;
					placed = true;					
				}
				if (placed) {
					break;
				}
			}
			if (placed) {
				break;
			}
		}
		if (!placed) {
			print_tic_tac_toe_board();
			newline();
			terminal_writestring("Its a tie!");
			end_tic_tac_toe();
			return;
		}

		check_for_win_ttt();
		if (check_for_win_ttt() == 1) {
			print_tic_tac_toe_board();
			newline();
			terminal_writestring("You win!");
			end_tic_tac_toe();
			return;
		} else if (check_for_win_ttt() == 2) {
			print_tic_tac_toe_board();
			newline();
			terminal_writestring("You lose!");
			end_tic_tac_toe();
			return;
		}

		print_tic_tac_toe_board();
		newline();
		terminal_writestring("Your turn.");
		game_round++;
		return;
	}
}
