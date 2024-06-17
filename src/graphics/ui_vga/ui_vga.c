#include <system.h> 

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

typedef struct {
    void (*func)(bool left_click, int clickable_box_index, int box_index);
} click_handler_t;

typedef struct {
    int x;
    int y;
    int width;
    int height;
    click_handler_t handler;
    int box_index;
} clickable_box;

typedef struct {
    int x;
    int y;
    int width;
    int height;
    bool clickable;
    bool draggable;
    bool closeable;
    click_handler_t handler;
} box;

clickable_box clickable_boxes[100] = {0};
int num_clickable_boxes = 0;

box boxes[100] = {0};
int num_boxes = 0;

int ui_default[320][200] = {0};

void box_handler(bool left_click, int clickable_box_index, int box_index) {
    clickable_box box = clickable_boxes[clickable_box_index];
    
    if (left_click) {
        draw_rectangle(box.x, box.y + 10, box.width, box.height - 10, VGA_COLOR_RED, false);
    }
};

void box_close_handler(bool left_click, int clickable_box_index, int box_index) {
    clickable_box box = clickable_boxes[clickable_box_index];
    
    if (left_click) {
        for (int i = clickable_box_index - 1; i < num_clickable_boxes - 2; i++) {
            clickable_boxes[i] = clickable_boxes[i+2];
        }
        num_clickable_boxes -= 2;

        for (int i = box_index - 1; i < num_boxes - 1; i++) {
            boxes[i] = boxes[i+1];
        }
        num_boxes--;
    }

    refresh_ui();
}

void add_box(int x, int y, int width, int height, bool clickable, bool draggable, void (*func)(bool left_click, int clickable_box_index, int box_index), bool closeable) {
    if (clickable) {
        clickable_box box;
        box.x = x;
        box.y = y;
        box.width = width;
        box.height = height;
        box.handler.func = func;
        box.box_index = num_boxes;

        clickable_boxes[num_clickable_boxes] = box;
        num_clickable_boxes++;
    }

    box b;
    b.x = x;
    b.y = y;
    b.width = width;
    b.height = height;
    b.clickable = clickable;
    b.draggable = draggable;
    b.closeable = closeable;
    b.handler.func = func;

    boxes[num_boxes] = b;
    num_boxes++;
   
    draw_box(x, y, width, height, closeable);


    if (draggable) {
        // TODO
    }

    if (closeable) {
        clickable_box box;
        box.x = x + width - 10;
        box.y = y;
        box.width = 10;
        box.height = 10;
        box.handler.func = box_close_handler;

        clickable_boxes[num_clickable_boxes] = box;
        num_clickable_boxes++;
    }
}

void refresh_ui() {
    for (int i = 0; i < num_boxes; i++) {
        box b = boxes[i];
        draw_box(b.x, b.y, b.width, b.height, b.closeable);
    }

    vga_print_frame_buffer();
}

void draw_box(int x, int y, int width, int height, bool closeable) {
    draw_rectangle(x, y, width, height, VGA_COLOR_WHITE, true);
    draw_rectangle(x, y, width, 10, VGA_COLOR_LIGHT_GREY, true);

    if (closeable) {
        draw_rectangle(x + width - 10, y, 10, 10, VGA_COLOR_RED, true);
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
        for (int i = 0; i < num_clickable_boxes; i++) {
            clickable_box box = clickable_boxes[i];

            if (mouse_x >= box.x && mouse_x <= box.x + box.width && mouse_y >= box.y && mouse_y <= box.y + box.height) {
                box.handler.func(mouse_byte[0] & 0x01, i, box.box_index);
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

    add_box(10, 10, 100, 100, true, true, box_handler, true);

    vga_print_frame_buffer();
}