#include <system.h>

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>


// Rock, paper, scissors game (the pc always wins)
void rock_paper_scissors() {
	if (game_round == 1) {
		terminal_writestring("Rock, paper, scissors, you go first!");
		newline();
		terminal_writestring("Type rock, paper or scissors");
		game_round++;
		return;

	} else if (game_round == 2) {
		if (strcmp(command, "rock") == 0) {
			terminal_writestring("I choose paper, You lose!");
		} else if (strcmp(command, "paper") == 0) {
			terminal_writestring("I choose scissors, You lose!");
		} else if (strcmp(command, "scissors") == 0) {
			terminal_writestring("I choose rock, You lose!");
		} else {
			terminal_writestring("That is not a valid move, You lose!");
		}
		game_round = 1;
		in_game = false;
		return;
	}
}