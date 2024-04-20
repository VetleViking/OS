#include <system.h> 
#include <chess.h>

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>


int camera_pos[3] = {0, 2, 0};

// 0, 0 is straight ahead
// uses degrees
int camera_angle[2] = {0, 0}; 

typedef struct {
    int x;
    int y;
    int z;
    unsigned short color;
    char name[10];
} objects_and_positions;

objects_and_positions objects[10];
int num_objects = 0;

int buffer[1080][1920];

void plot_cube(int x, int y, int z, unsigned short color, char name[10]) {
    objects[num_objects].x = x;
    objects[num_objects].y = y;
    objects[num_objects].z = z;
    objects[num_objects].color = color;
    
    for (int i = 0; i < 10; i++) {
        objects[num_objects].name[i] = name[i];

        if (name[i] == '\0') {
            break;
        }
    }

    num_objects++;
}

void draw_view() {


    int end_x = 960;
    int y = 1080;
    int end_y = 540;

    bool print_cube = false;
    int cube_pos[2][2] = {0};

    // grid lines for the coordinates
    for (int i = 0; i < 1000; i++) {
        int num = i * 200;

        int x = num;
        int x2 = -num;

        bga_draw_trapezoid(end_x, end_y, end_x + 1, end_y, x + 5, y, x, y, 0xFFFFFF);
        bga_draw_trapezoid(end_x, end_y, end_x + 1, end_y, x2 + 5, y, x2, y, 0xFFFFFF);
    }

    while (y > 540) {
        int space_between = 100 * (y - 540) / 540;
        int height = 5 * (y - 540) / 540;

        if (height == 0) {
            height = 1;
        }

        bga_draw_rectangle(0, y, 1920, height, 0xFFFFFF);

        y = y - (space_between + height);
    }

    bga_draw_rectangle(958, 538, 4, 4, 0xFF0000);
}

void enter_3d_test() {
    bga_set_video_mode(1920, 1080, 32, 1, 1);

    plot_cube(5, 0, 5, 0xFF0000, "cube1");

    draw_view();
}