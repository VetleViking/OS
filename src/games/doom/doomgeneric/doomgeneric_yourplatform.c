#include <system.h>

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

int start_ticks = 0;


void DG_Init() {
    start_ticks = timer_ticks;
    vga_enter();
}

void DG_DrawFrame() {
    draw_image(0, 0, 320, 200, DG_ScreenBuffer, false);
}

void DG_SleepMs(uint32_t ms) {
    sleep(ms / 10);
}

uint32_t DG_GetTicksMs() {
    return (timer_ticks - start_ticks) /10;
}

int DG_GetKey(int* pressed, int* doomKey) {
    int keycode = getkey();
    if (keycode == -1) {
        return 0;
    }

    *pressed = (keycode & 0x80) != 0;
    *doomKey = keycode & 0x7F;
    return 1;
}

int DG_main(int argc, char **argv)
{
    doomgeneric_Create(argc, argv);

    while (1)
    {
        doomgeneric_Tick();
    }
    
    return 0;
}