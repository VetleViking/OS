#include <system.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

typedef struct {
    void (*func)(bool left_click, int box_index);
} click_handler_t;

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

typedef struct {
    int x;
    int y;
    int width;
    int height;
    click_handler_t handler;
    int box_index;
} closeable_box;

typedef struct {
    int x;
    int y;
    int width;
    int height;
    click_handler_t handler;
    int box_index;
    int mouse_last_x;
    int mouse_last_y;
    bool dragging;
} draggable_box;

box boxes[100] = {0};
int num_boxes = 0;

closeable_box closeable_boxes[100] = {0};
int num_closeable_boxes = 0;

draggable_box draggable_boxes[100] = {0};
int num_draggable_boxes = 0;

int ui_default[320][200] = {0};

bool last_left_click_ui = false;

void box_handler2(bool left_click, int box_index) {
    box b = boxes[box_index];
    
    if (left_click) {
        add_box(b.x + 50, b.y + 50, 50, 50, true, true, box_handler, true);
        refresh_ui();
    }
}

void box_handler(bool left_click, int box_index) {
    box b = boxes[box_index];
    
    if (left_click) {
        draw_rectangle(b.x, b.y, b.width, b.height, VGA_COLOR_RED, false);
    }
}

void box_close_handler(bool left_click, int closeable_box_index) {
    closeable_box c_box = closeable_boxes[closeable_box_index];
    int box_index = c_box.box_index;

    if (left_click && !last_left_click_ui) {
        for (int i = closeable_box_index; i < num_closeable_boxes - 1; i++) {
            closeable_boxes[i] = closeable_boxes[i + 1];
        }
        num_closeable_boxes--;

        for (int i = box_index; i < num_boxes - 1; i++) {
            boxes[i] = boxes[i + 1];
        }
        num_boxes--;
    }

    refresh_ui();
}

void box_drag_handler(bool left_click, int draggable_box_index) {
    draggable_box d_box = draggable_boxes[draggable_box_index];
    int box_index = d_box.box_index;

    if (left_click && !last_left_click_ui) {
        draggable_boxes[draggable_box_index].mouse_last_x = mouse_x;
        draggable_boxes[draggable_box_index].mouse_last_y = mouse_y;
        draggable_boxes[draggable_box_index].dragging = true;
    } else if (left_click && last_left_click_ui && draggable_boxes[draggable_box_index].dragging) {
        int x_diff = mouse_x - draggable_boxes[draggable_box_index].mouse_last_x;
        int y_diff = mouse_y - draggable_boxes[draggable_box_index].mouse_last_y;

        boxes[box_index].x += x_diff;
        boxes[box_index].y += y_diff;

        closeable_boxes[box_index].x += x_diff;
        closeable_boxes[box_index].y += y_diff;

        draggable_boxes[draggable_box_index].x += x_diff;
        draggable_boxes[draggable_box_index].y += y_diff;

        draggable_boxes[draggable_box_index].mouse_last_x = mouse_x;
        draggable_boxes[draggable_box_index].mouse_last_y = mouse_y;

        refresh_ui();
    }
}


void add_box(int x, int y, int width, int height, bool clickable, bool draggable, void (*func)(bool left_click, int box_index), bool closeable) {
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

    if (closeable) {
        closeable_box c_box;
        c_box.x = x + width - 10;
        c_box.y = y - 10;
        c_box.width = 10;
        c_box.height = 10;
        c_box.handler.func = box_close_handler;
        c_box.box_index = num_boxes;

        closeable_boxes[num_closeable_boxes] = c_box;
        num_closeable_boxes++;
    }

    if (draggable) {
        draggable_box d_box;
        d_box.x = x;
        d_box.y = y - 10;
        d_box.width = width;
        d_box.height = 10;
        d_box.handler.func = box_drag_handler;
        d_box.box_index = num_boxes;

        draggable_boxes[num_draggable_boxes] = d_box;
        num_draggable_boxes++;
    }

    num_boxes++;
    draw_box(x, y, width, height, closeable);
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
    draw_rectangle(x, y - 10, width, 10, VGA_COLOR_LIGHT_GREY, true);

    if (closeable) {
        draw_rectangle(x + width - 10, y - 10, 10, 10, VGA_COLOR_RED, true);
    }
}

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
            box b = boxes[i];

            if (b.clickable && mouse_x >= b.x && mouse_x <= b.x + b.width && mouse_y >= b.y && mouse_y <= b.y + b.height) {
                b.handler.func(mouse_byte[0] & 0x01, i);
            }
        }

        for (int i = 0; i < num_closeable_boxes; i++) {
            closeable_box c_box = closeable_boxes[i];

            if (mouse_x >= c_box.x && mouse_x <= c_box.x + c_box.width && mouse_y >= c_box.y && mouse_y <= c_box.y + c_box.height) {
                c_box.handler.func(mouse_byte[0] & 0x01, i);
            }
        }
    }

            for (int i = 0; i < num_draggable_boxes; i++) { // draggable boxes should only be called on left click
            draggable_box d_box = draggable_boxes[i];

            if (mouse_byte[0] & 0x01) {    
                if (mouse_x >= d_box.x && mouse_x <= d_box.x + d_box.width && mouse_y >= d_box.y && mouse_y <= d_box.y + d_box.height) {
                    d_box.handler.func(true, i);
                } else if (d_box.dragging) {
                    d_box.handler.func(true, i);
                }
            }
            
            if (d_box.dragging && !(mouse_byte[0] & 0x01)) {
                d_box.dragging = false;

                draggable_boxes[i] = d_box;
            }
        }

    if (mouse_byte[0] & 0x01) { // left click


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
    add_box(120, 10, 50, 50, true, true, box_handler, true);
    add_box(230, 10, 70, 70, true, true, box_handler, true);
    add_box(10, 120, 30, 30, true, true, box_handler2, true);

    vga_print_frame_buffer();
}
