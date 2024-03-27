#include <system.h> 

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

mouse_x = 0;
mouse_y = 0;

bool in_mouse_test = false; // bool to track if in mouse test


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
}



