#include <system.h>

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>



#define VBE_DISPI_IOPORT_INDEX 0x01CE
#define VBE_DISPI_IOPORT_DATA 0x01CF

#define VBE_DISPI_INDEX_ID 0x0
#define VBE_DISPI_INDEX_XRES 0x1
#define VBE_DISPI_INDEX_YRES 0x2
#define VBE_DISPI_INDEX_BPP 0x3
#define VBE_DISPI_INDEX_ENABLE 0x4
#define VBE_DISPI_INDEX_BANK 0x5

#define VBE_DISPI_DISABLED 0x00
#define VBE_DISPI_ENABLED 0x01
#define VBE_DISPI_LFB_ENABLED 0x40
#define VBE_DISPI_NOCLEARMEM 0x80

#define VBE_DISPI_ID4 0xB0C5

#define BGA_VIDEO_MEMORY 0xFD000000
int current_width = 0;

void outpw(unsigned short port, unsigned short value) {
    __asm__ __volatile__ ("outw %0, %1" : : "a"(value), "Nd"(port));
}

unsigned short inpw(unsigned short port) {
    unsigned short value;
    __asm__ __volatile__ ("inw %1, %0" : "=a"(value) : "Nd"(port));
    return value;
}

void bga_write_register(unsigned short IndexValue, unsigned short DataValue) {
    outpw(VBE_DISPI_IOPORT_INDEX, IndexValue);
    outpw(VBE_DISPI_IOPORT_DATA, DataValue);
}
 
unsigned short bga_read_register(unsigned short IndexValue) {
    outpw(VBE_DISPI_IOPORT_INDEX, IndexValue);
    return inpw(VBE_DISPI_IOPORT_DATA);
}
 
int bga_is_available(void) {
    return (bga_read_register(VBE_DISPI_INDEX_ID) == VBE_DISPI_ID4);
}
 
void bga_set_video_mode(unsigned int Width, unsigned int Height, unsigned int BitDepth, int UseLinearFrameBuffer, int ClearVideoMemory) {
    bga_write_register(VBE_DISPI_INDEX_ENABLE, VBE_DISPI_DISABLED);
    bga_write_register(VBE_DISPI_INDEX_XRES, Width);
    bga_write_register(VBE_DISPI_INDEX_YRES, Height);
    bga_write_register(VBE_DISPI_INDEX_BPP, BitDepth);
    bga_write_register(VBE_DISPI_INDEX_ENABLE, VBE_DISPI_ENABLED |
        (UseLinearFrameBuffer ? VBE_DISPI_LFB_ENABLED : 0) |
        (ClearVideoMemory ? 0 : VBE_DISPI_NOCLEARMEM));

    current_width = Width;
}
 
void bga_set_bank(unsigned short BankNumber) {
    bga_write_register(VBE_DISPI_INDEX_BANK, BankNumber);
}

void bga_plot_pixel(int x, int y, unsigned int color) {
    unsigned int* video_memory = (unsigned int*) BGA_VIDEO_MEMORY;
    unsigned int offset = y * current_width + x;

    video_memory[offset] = color;
}

void bga_clear_screen() {
    unsigned int* video_memory = (unsigned int*) BGA_VIDEO_MEMORY;
    for (int i = 0; i < 1920 * 1080; i++) {
        video_memory[i] = 0;
    }
}

void bga_draw_rectangle(int x, int y, int width, int height, unsigned short color) {
    for (int i = x; i < x + width; i++) {
        for (int j = y; j < y + height; j++) {
            bga_plot_pixel(i, j, color);
        }
    }
}

void bga_draw_circle(int x, int y, int radius, unsigned short color) {
    for (int i = 0; i < 1920; i++) {
        for (int j = 0; j < 1080; j++) {
            if ((i-x)*(i-x) + (j-y)*(j-y) <= radius*radius) {
                bga_plot_pixel(i,j,color);
            }
        }
    }
}

void bga_draw_triangle(int x1, int y1, int x2, int y2, int x3, int y3, unsigned short color) {
    for (int i = 0; i < 1920; i++) {
        for (int j = 0; j < 1080; j++) {
            if ((i-x1)*(y2-y1) - (x2-x1)*(j-y1) > 0 && (i-x2)*(y3-y2) - (x3-x2)*(j-y2) > 0 && (i-x3)*(y1-y3) - (x1-x3)*(j-y3) > 0) {
                bga_plot_pixel(i,j,color);
            }
        }
    }
}