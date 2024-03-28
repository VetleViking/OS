#include <system.h>

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

uint8_t mouse_cycle = 0;
int8_t  mouse_byte[3];


#define MOUSE_PORT   0x60
#define MOUSE_STATUS 0x64
#define MOUSE_ABIT   0x02
#define MOUSE_BBIT   0x01
#define MOUSE_WRITE  0xD4
#define MOUSE_F_BIT  0x20
#define MOUSE_V_BIT  0x08

#define MOUSE_HEIGHT 16
#define MOUSE_WIDTH 11

// array size is 684
int mouse_img[176] = {
    0x00, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 
    0x00, 0x00, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 
    0x00, 0x0f, 0x00, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 
    0x00, 0x0f, 0x0f, 0x00, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 
    0x00, 0x0f, 0x0f, 0x0f, 0x00, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 
    0x00, 0x0f, 0x0f, 0x0f, 0x0f, 0x00, 0xff, 0xff, 0xff, 0xff, 0xff, 
    0x00, 0x0f, 0x0f, 0x0f, 0x0f, 0x0f, 0x00, 0xff, 0xff, 0xff, 0xff,
    0x00, 0x0f, 0x0f, 0x0f, 0x0f, 0x0f, 0x0f, 0x00, 0xff, 0xff, 0xff, 
    0x00, 0x0f, 0x0f, 0x0f, 0x0f, 0x0f, 0x0f, 0x0f, 0x00, 0xff, 0xff, 
    0x00, 0x0f, 0x0f, 0x0f, 0x0f, 0x0f, 0x0f, 0x0f, 0x0f, 0x00, 0xff, 
    0x00, 0x0f, 0x0f, 0x0f, 0x0f, 0x0f, 0x0f, 0x0f, 0x0f, 0x0f, 0x00,
    0x00, 0x0f, 0x0f, 0x0f, 0x0f, 0x0f, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x0f, 0x0f, 0x00, 0x0f, 0x0f, 0x00, 0xff, 0xff, 0xff, 0xff, 
    0x00, 0x0f, 0x00, 0xff, 0x00, 0x0f, 0x0f, 0x00, 0xff, 0xff, 0xff, 
    0x00, 0x00, 0xff, 0xff, 0x00, 0x0f, 0x0f, 0x00, 0xff, 0xff, 0xff, 
    0xff, 0xff, 0xff, 0xff, 0xff, 0x00, 0x00, 0xff, 0xff, 0xff, 0xff, 
};

int under_mouse[176] = {0};


typedef struct {
    void (*func)(int8_t mouse_byte[3]);
    bool should_call;
} mouse_handler_t;

#define MAX_MOUSE_HANDLERS 10
mouse_handler_t mouse_handlers[MAX_MOUSE_HANDLERS];

void init_mouse_handlers() {
    for (int i = 0; i < MAX_MOUSE_HANDLERS; i++) {
        mouse_handlers[i].func = NULL;
        mouse_handlers[i].should_call = false;
    }
}

void add_mouse_handler(void (*func)(int8_t mouse_byte[3]), bool should_call) {
    for (int i = 0; i < MAX_MOUSE_HANDLERS; i++) {
        if (mouse_handlers[i].func == NULL) {
            mouse_handlers[i].func = func;
            mouse_handlers[i].should_call = should_call;
            return;
        }
    }
}

void edit_mouse_handler(void (*func)(int8_t mouse_byte[3]), bool should_call) {
    for (int i = 0; i < MAX_MOUSE_HANDLERS; i++) {
        if (mouse_handlers[i].func == func) {
            mouse_handlers[i].should_call = should_call;
            return;
        }
    }
}

void init_under_mouse() {
    for (int i = 0; i < MOUSE_WIDTH; i++) {
        for (int j = 0; j < MOUSE_HEIGHT; j++) {
            under_mouse[j * MOUSE_WIDTH + i] = 0xff;
        }
    }
}

void mouse_wait(bool a_type) {
	uint32_t timeout = 100000;
	if (!a_type) {
		while (--timeout) {
			if ((inb(MOUSE_STATUS) & MOUSE_BBIT) == 1) {
				return;
			}
		}
		return;
	} else {
		while (--timeout) {
			if (!((inb(MOUSE_STATUS) & MOUSE_ABIT))) {
				return;
			}
		}
		return;
	}
}

void mouse_write(uint8_t write) {
	mouse_wait(true);
	outportb(MOUSE_STATUS, MOUSE_WRITE);
	mouse_wait(true);
	outportb(MOUSE_PORT, write);
}

uint8_t mouse_read() {
	mouse_wait(false);
	char t = inb(MOUSE_PORT);
	return t;
}

void print_mouse(int x, int y) {
    for (int i = 0; i < MOUSE_WIDTH; i++) {
        for (int j = 0; j < MOUSE_HEIGHT; j++) {
            under_mouse[j * MOUSE_WIDTH + i] = vga_get_pixel_color(i + x, j + y);
            unsigned int color = mouse_img[j * MOUSE_WIDTH + i];

            if (color == 0xff) {
                continue;
            } else if (x + i >= 320 || y + j >= 200) {
                continue;
            } else if (x + i < 0 || y + j < 0) {
                continue;
            }

            vga_plot_pixel(i + x, j + y, color);
        }
    }
}

void remove_mouse(int x, int y) {
    for (int i = 0; i < MOUSE_WIDTH; i++) {
        for (int j = 0; j < MOUSE_HEIGHT; j++) {
            unsigned int color = under_mouse[j * MOUSE_WIDTH + i];

            if (color == 0xff) {
                continue;
            }

            vga_plot_pixel(i + x, j + y, color);
        }
    }
}

void mouse_handler(struct regs *r) {
    uint8_t status = inb(MOUSE_STATUS);
    while (status & MOUSE_BBIT) {
        int8_t mouse_in = inb(MOUSE_PORT);
        if (status & MOUSE_F_BIT) {
            switch (mouse_cycle) {
                case 0:
                    mouse_byte[0] = mouse_in;
                    if (!(mouse_in & MOUSE_V_BIT)) return;
                    ++mouse_cycle;
                    break;
                case 1:
                    mouse_byte[1] = mouse_in;
                    ++mouse_cycle;
                    break;
                case 2:
                    mouse_byte[2] = mouse_in;
                    if (mouse_byte[0] & 0x80 || mouse_byte[0] & 0x40) {
                        break;
                    }

                    for (int i = 0; i < MAX_MOUSE_HANDLERS; i++) {
                        if (mouse_handlers[i].should_call && mouse_handlers[i].func != NULL) {
                            mouse_handlers[i].func(mouse_byte);
                        }
                    }

                    mouse_cycle = 0;	
                    break;
            }
        }
        status = inb(MOUSE_STATUS);
    }

	outportb(0xA0, 0x20);
}

void mouse_install() {
	uint8_t status;
	mouse_wait(true);
	outportb(MOUSE_STATUS, 0xA8);
	mouse_wait(true);
	outportb(MOUSE_STATUS, 0x20);
	mouse_wait(false);
	status = inb(0x60) | 2;
	mouse_wait(true);
	outportb(MOUSE_STATUS, 0x60);
	mouse_wait(false);
	outportb(MOUSE_PORT, status);
	mouse_write(0xF6);
	mouse_read();
	mouse_write(0xF4);
	mouse_read();
	irq_install_handler(12, mouse_handler);
    init_mouse_handlers();
    init_under_mouse();
}