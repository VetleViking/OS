
#define DOOM_IMPLEMENTATION
#include <system.h>
#include <PureDOOM.h> 

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>



bool in_doom = false; // bool to track if in the game

void doom_keyboard_handler(c) {
    if (c == 75) { // left
    } else if (c == 77) { // right
    } else if (c == 72) { // up
	} else if (c == 80) { // down
    } else if (c == 28) { // enter
    } else if (c == 1) { // esc
        in_doom = false;
        in_game = false;
        remove_keyboard_handler(doom_keyboard_handler);
        remove_main_loop_call(doom_play);
    }
}

void test_write(char *str) {

    for (size_t i = 0; i < strlen(str); i++) {
        terminal_putentryat(str[i], terminal_color, terminal_column, terminal_row);

        if (++terminal_column == VGA_WIDTH) {
            terminal_column = 0;
            if (++terminal_row == VGA_HEIGHT) {
                terminal_row = 0;
            }
        }
    }
    //const size_t index = 0 * VGA_WIDTH + 0;
	//terminal_buffer[index] = vga_entry('c', terminal_color);
}

void doom_play() {

    doom_set_print(test_write);
    doom_init(0, NULL, 0);
    while (true) {
        // do game stuff
        sleep(1);
        //doom_update();
    }
}  

void doom_start() {
    add_keyboard_handler(doom_keyboard_handler);
    add_main_loop_call(doom_play);

    in_doom = true;

    //vga_enter(); // if the game uses VGA
    //vga_clear_screen(); // VGA

    // game specific setup
}



