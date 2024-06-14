#include <system.h>

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#define VGA_ADDRESS 0xA0000

#define VGA_AC_INDEX 0x3C0
#define VGA_AC_WRITE 0x3C0
#define VGA_AC_READ 0x3C1
#define VGA_INSTAT_READ 0x3DA
#define VGA_MISC_WRITE 0x3C2
#define VGA_MISC_READ 0x3CC

#define VGA_CRTC_INDEX 0x3D4
#define VGA_CRTC_DATA 0x3D5
#define VGA_GC_INDEX 0x3CE
#define VGA_GC_DATA 0x3CF
#define VGA_SEQ_INDEX 0x3C4
#define VGA_SEQ_DATA 0x3C5

#define VGA_NUM_AC_REGS 21
#define VGA_NUM_CRTC_REGS 25
#define VGA_NUM_GC_REGS 9
#define VGA_NUM_SEQ_REGS 5

int frame_buffer[320][200] = {0};

unsigned char g_320x200x256[] = {
/* MISC */
	0x63,
/* SEQ */
	0x03, 0x01, 0x0F, 0x00, 0x0E,
/* CRTC */
	0x5F, 0x4F, 0x50, 0x82, 0x54, 0x80, 0xBF, 0x1F,
	0x00, 0x41, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x9C, 0x0E, 0x8F, 0x28,	0x40, 0x96, 0xB9, 0xA3,
	0xFF,
/* GC */
	0x00, 0x00, 0x00, 0x00, 0x00, 0x40, 0x05, 0x0F,
	0xFF,
/* AC */
	0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07,
	0x08, 0x09, 0x0A, 0x0B, 0x0C, 0x0D, 0x0E, 0x0F,
	0x41, 0x00, 0x0F, 0x00,	0x00
};

unsigned char g_80x25_text[] = {
/* MISC */
	0x67,
/* SEQ */
	0x03, 0x00, 0x03, 0x00, 0x02,
/* CRTC */
	0x5F, 0x4F, 0x50, 0x82, 0x55, 0x81, 0xBF, 0x1F,
	0x00, 0x4F, 0x0D, 0x0E, 0x00, 0x00, 0x00, 0x50,
	0x9C, 0x0E, 0x8F, 0x28, 0x1F, 0x96, 0xB9, 0xA3,
	0xFF,
/* GC */
	0x00, 0x00, 0x00, 0x00, 0x00, 0x10, 0x0E, 0x00,
	0xFF,
/* AC */
	0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x14, 0x07,
	0x38, 0x39, 0x3A, 0x3B, 0x3C, 0x3D, 0x3E, 0x3F,
	0x0C, 0x00, 0x0F, 0x08, 0x00
};


void vga_enter() {
    terminal_writestring("Attempting to switch modes...");
    write_regs(g_320x200x256);
    vga_clear_screen();
}

void vga_exit() {
	vga_clear_screen();
	write_regs(g_80x25_text);
	is_writing_command = false;
	clear_screen();
	new_kernel_line();
	is_writing_command = true;
}

void vga_print_frame_buffer() {
	for (int i = 0; i < 320; i++) {
		for (int j = 0; j < 200; j++) {
			if (frame_buffer[i][j] != vga_get_pixel_color(i, j)) {
				vga_plot_pixel(i, j, frame_buffer[i][j], false);
			}
			
			frame_buffer[i][j] = 0;
		}
	}
}

void draw_line(int x, int y, int x2, int y2, unsigned short color, bool use_buffer) {
	int dx = x2 - x;
	int dy = y2 - y;
	int dx1 = 0, dy1 = 0, dx2 = 0, dy2 = 0;

	if (dx < 0) dx1 = -1; else if (dx > 0) dx1 = 1;
	if (dy < 0) dy1 = -1; else if (dy > 0) dy1 = 1;
	if (dx < 0) dx2 = -1; else if (dx > 0) dx2 = 1;

	int longest = dx < 0 ? -dx : dx;
	int shortest = dy < 0 ? -dy : dy;

	if (!(longest > shortest)) {
		longest = dy < 0 ? -dy : dy;
		shortest = dx < 0 ? -dx : dx;
		if (dy < 0) dy2 = -1; else if (dy > 0) dy2 = 1;
		dx2 = 0;            
	}

	int numerator = longest >> 1;
	for (int i = 0; i <= longest; i++) {
		vga_plot_pixel(x, y, color, use_buffer);
		numerator += shortest;
		if (!(numerator < longest)) {
			numerator -= longest;
			x += dx1;
			y += dy1;
		} else {
			x += dx2;
			y += dy2;
		}
	}
}

void draw_rectangle(int x, int y, int width, int height, unsigned short color, bool use_buffer) {
	for (int i = 0; i < width; i++) {
		for (int j = 0; j < height; j++) {
			vga_plot_pixel(x+i, y+j, color, use_buffer);
		}
	}
}

void draw_circle(int x, int y, int radius, unsigned short color, bool use_buffer) {
	for (int i = 0; i < 320; i++) {
		for (int j = 0; j < 200; j++) {
			if ((i-x)*(i-x) + (j-y)*(j-y) <= radius*radius) {
				vga_plot_pixel(i,j,color, use_buffer);
			}
		}
	}
}

void draw_triangle(int x1, int y1, int x2, int y2, int x3, int y3, unsigned short color, bool use_buffer) {
	draw_line(x1, y1, x2, y2, color, use_buffer);
	draw_line(x2, y2, x3, y3, color, use_buffer);
	draw_line(x3, y3, x1, y1, color, use_buffer);

	// for (int i = 0; i < 320; i++) {
	// 	for (int j = 0; j < 200; j++) {
	// 		if ((i-x1)*(y2-y1) - (x2-x1)*(j-y1) > 0 && (i-x2)*(y3-y2) - (x3-x2)*(j-y2) > 0 && (i-x3)*(y1-y3) - (x1-x3)*(j-y3) > 0) {
	// 			vga_plot_pixel(i,j,color, use_buffer);
	// 		}
	// 	}
	// }
}

inline int min(int a, int b) {
    return a < b ? a : b;
}

inline int max(int a, int b) {
    return a > b ? a : b;
}

void draw_triangle_fill(int x1, int y1, int x2, int y2, int x3, int y3, unsigned short color, bool use_buffer) {
    int minX = min(x1, min(x2, x3));
    int minY = min(y1, min(y2, y3));
    int maxX = max(x1, max(x2, x3));
    int maxY = max(y1, max(y2, y3));

    int edge1_x = y1 - y2;
    int edge1_y = x2 - x1;
    int edge2_x = y2 - y3;
    int edge2_y = x3 - x2;
    int edge3_x = y3 - y1;
    int edge3_y = x1 - x3;

    int c1 = edge1_x * x1 + edge1_y * y1;
    int c2 = edge2_x * x2 + edge2_y * y2;
    int c3 = edge3_x * x3 + edge3_y * y3;

    for (int i = minX; i <= maxX; i++) {
        for (int j = minY; j <= maxY; j++) {
            int w1 = edge1_x * i + edge1_y * j - c1;
            int w2 = edge2_x * i + edge2_y * j - c2;
            int w3 = edge3_x * i + edge3_y * j - c3;

            if (w1 >= 0 && w2 >= 0 && w3 >= 0) {
                vga_plot_pixel(i, j, color, use_buffer);
            }
        }
    }
}

void vga_clear_screen() {
    for (int i = 0; i < 320; i++) {
        for (int j = 0; j < 200; j++) {
            vga_plot_pixel(i,j,VGA_COLOR_BLACK, false);
			vga_plot_pixel(i,j,VGA_COLOR_BLACK, true);
        }
    }
}

void vga_plot_pixel(int x, int y, unsigned short color, bool use_buffer) {
    unsigned short offset = x + 320 * y;
    unsigned char *VGA = (unsigned char*) VGA_ADDRESS;
	
	if (use_buffer) {
		frame_buffer[x][y] = color;
	} else {	
    	VGA[offset] = color;
	}
}

unsigned short vga_get_pixel_color(int x, int y) {
    unsigned short offset = x + 320 * y;
    unsigned char *VGA = (unsigned char*) VGA_ADDRESS;
    return VGA[offset];
}

void draw_image(int x, int y, int width, int height, unsigned short *image, bool use_buffer) {
	for (int i = 0; i < width; i++) {
		for (int j = 0; j < height; j++) {
			vga_plot_pixel(x+i, y+j, image[i+j*width], use_buffer);
		}
	}
}

// Copied code from osdev.org
void write_regs(unsigned char *regs) {
	unsigned i;

	// write MISCELLANEOUS reg
	outb(VGA_MISC_WRITE, *regs);
	regs++;

	// write SEQUENCER regs
	for (i = 0; i < VGA_NUM_SEQ_REGS; i++) {
		outb(VGA_SEQ_INDEX, i);
		outb(VGA_SEQ_DATA, *regs);
		regs++;
	}

	// unlock CRTC registers
	outb(VGA_CRTC_INDEX, 0x03);
	outb(VGA_CRTC_DATA, inb(VGA_CRTC_DATA) | 0x80);
	outb(VGA_CRTC_INDEX, 0x11);
	outb(VGA_CRTC_DATA, inb(VGA_CRTC_DATA) & ~0x80);

	regs[0x03] |= 0x80;
	regs[0x11] &= ~0x80;

	// write CRTC regs
	for (i = 0; i < VGA_NUM_CRTC_REGS; i++) {
		outb(VGA_CRTC_INDEX, i);
		outb(VGA_CRTC_DATA, *regs);
		regs++;
	}

	// write GRAPHICS CONTROLLER regs
	for (i = 0; i < VGA_NUM_GC_REGS; i++) {
		outb(VGA_GC_INDEX, i);
		outb(VGA_GC_DATA, *regs);
		regs++;
	}

	// write ATTRIBUTE CONTROLLER regs
	for (i = 0; i < VGA_NUM_AC_REGS; i++) {
		(void)inb(VGA_INSTAT_READ);
		outb(VGA_AC_INDEX, i);
		outb(VGA_AC_WRITE, *regs);
		regs++;
	}

	// lock 16-color, bool use_buffer palette and unblank display
	(void)inb(VGA_INSTAT_READ);
	outb(VGA_AC_INDEX, 0x20);
}
