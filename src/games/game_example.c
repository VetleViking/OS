//
// This file contains the example of code for a game.
// 

#include <system.h> 

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>



bool in_gamename = false; // bool to track if in the game

void gamename_keyboard_handler(c) {
    if (c == 75) { // left

    } else if (c == 77) { // right

    } else if (c == 72) { // up

	} else if (c == 80) { // down

    } else if (c == 28) { // enter

    } else if (c == 1) { // esc
        in_gamename = false;
    }
}

void gamename_play() {
    while (in_gamename) {
        sleep(100); // sleep for 1s

        // do game stuff
    }
}  

void gamename_start() {
    in_gamename = true;

    vga_enter(); // if the game uses VGA
    vga_clear_screen(); // VGA

    // game specific setup
}



