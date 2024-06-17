#include <system.h> 

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

typedef struct {
    void (*func)(bool left_click, int clickable_box_index);
} click_handler_t;

typedef struct {
    int x;
    int y;
    int width;
    int height;
    click_handler_t handler;
} clickable_box;

clickable_box boxes[100] = {0};

int num_boxes = 0;

int ui_default[320][200] = {0};

void box_handler(bool left_click, int clickable_box_index) {
    clickable_box box = boxes[clickable_box_index];
    
    if (left_click) {
        draw_rectangle(box.x, box.y, box.width, box.height, VGA_COLOR_RED, false);
    }
};




void add_box(int x, int y, int width, int height, bool clickable, bool draggable, void (*func)(bool left_click, int clickable_box_index)) {
    draw_rectangle(x, y, width, height, VGA_COLOR_WHITE, true);

    if (clickable) {
        clickable_box box;
        box.x = x;
        box.y = y;
        box.width = width;
        box.height = height;
        box.handler.func = func;

        boxes[num_boxes] = box;
        num_boxes++;
    }
}

bool last_left_click_ui = false;

void ui_mouse_handler(int8_t mouse_byte[3]) {
    remove_mouse(mouse_x, mouse_y);

    mouse_x += mouse_byte[1] * 1;
    mouse_y -= mouse_byte[2] * 1;

    if (mouse_x < 0) {
        mouse_x = 0;
    } else if (mouse_x > 320) {
        mouse_x = 320;
    }

    if (mouse_y < 0) {
        mouse_y = 0;
    } else if (mouse_y > 200) {
        mouse_y = 200;
    }

    if (mouse_byte[0] & 0x01 || mouse_byte[0] & 0x02) {
        for (int i = 0; i < num_boxes; i++) {
            clickable_box box = boxes[i];

            if (mouse_x >= box.x && mouse_x <= box.x + box.width && mouse_y >= box.y && mouse_y <= box.y + box.height) {
                box.handler.func(mouse_byte[0] & 0x01, i);
            }
        }
    }

    if (mouse_byte[0] & 0x01) { // left click
            if (!last_left_click_ui) { // start of click
            }
        last_left_click_ui = true;
    } else {
        last_left_click_ui = false;
    }
    if (mouse_byte[0] & 0x02) { } // right click
    if (mouse_byte[0] & 0x04) { } // middle click

    print_mouse(mouse_x, mouse_y);
}

void ui_vga_init() {
    vga_enter();

    add_mouse_handler(ui_mouse_handler);

    add_box(10, 10, 100, 100, true, true, box_handler);

    vga_print_frame_buffer();
}