#include <system.h>

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>



bool in_pong = false;



void pong_make_map() {
    // make the pong map
}

void pong_keyboard_handler(c) {
    if (c == 72) { // up

	} else if (c == 80) { // down

    }
}

void pong_play() {
    while (in_pong) {
        // do
    }
}  

void pong_start() {
    in_pong = true;

    vga_enter();
    vga_clear_screen();
    pong_make_map();
}



