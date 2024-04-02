#include <system.h> 

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

mouse_x = 0;
mouse_y = 0;

bool in_mouse_test = false; // bool to track if in mouse test

void mouse_test_handler(int8_t mouse_byte[3]) {
    remove_mouse(mouse_x, mouse_y);

    mouse_x += mouse_byte[1] * 1;
    mouse_y -= mouse_byte[2] * 1;

    if (mouse_byte[0] & 0x01) { // left click
        draw_rectangle(0, 0, 10, 10, VGA_COLOR_BLUE);
    }
    if (mouse_byte[0] & 0x02) { // right click
        draw_rectangle(0, 10, 10, 10, VGA_COLOR_GREEN);
    }
    if (mouse_byte[0] & 0x04) { // middle click
        draw_rectangle(0, 20, 10, 10, VGA_COLOR_RED);
    }

    print_mouse(mouse_x, mouse_y);
}

void mouse_test_loop() {
    while (in_mouse_test) {
        sleep(100); // sleep for 1s
    }
}  

void mouse_test() {
    in_mouse_test = true;

    vga_enter(); 
    vga_clear_screen(); 

    mouse_x = 160;
    mouse_y = 100;

    add_mouse_handler(mouse_test_handler);
}



