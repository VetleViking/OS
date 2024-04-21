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

// Absolute value function for integers
int my_abs(int x) {
    return (x >= 0) ? x : -x;
}

// Absolute value function for floating point numbers
float my_fabs(float x) {
    return (x >= 0.0) ? x : -x;
}

// Square root function
float my_sqrt(float x) {
    if (x < 0) {
        // Handle negative input
        return 0.0; // Or whatever appropriate action
    }
    float guess = x / 2.0; // Initial guess
    float error = 0.0001; // Error tolerance
    while (true) {
        float new_guess = (guess + x / guess) / 2.0; // Newton's method
        if (my_fabs(new_guess - guess) < error) { // Check convergence
            return new_guess;
        }
        guess = new_guess;
    }
}

// Power function (x^y)
float my_pow(float x, int y) {
    if (y == 0) {
        return 1.0; // x^0 = 1
    }
    float result = x;
    for (int i = 1; i < my_abs(y); i++) {
        result *= x;
    }
    if (y < 0) {
        return 1.0 / result; // Handle negative exponent
    } else {
        return result;
    }
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



    float vertices[8][2] = {
        {100, 100},   
        {200, 100}, 
        {200, 200},  
        {100, 200},   
        {150, 50},   
        {250, 50},  
        {250, 150},  
        {150, 150}   
    };

    float center_x = 960;
    float center_y = 540;

    for (int i = 0; i < 8; i++) {
        float distance_to_center = my_sqrt(my_pow(my_abs(vertices[i][0] - center_x), 2) + my_pow(vertices[i][1] - center_y, 2));
        float scale_factor = 1.0 / (distance_to_center + 1.0); 
        vertices[i][0] = center_x + (vertices[i][0] - center_x) * scale_factor;
        vertices[i][1] = center_y + (vertices[i][1] - center_y) * scale_factor;
    }

    int faces[6][4] = {
        {0, 1, 2, 3},  
        {1, 5, 6, 2}, 
        {5, 4, 7, 6}, 
        {4, 0, 3, 7},  
        {4, 5, 1, 0},   
        {3, 2, 6, 7} 
    };

    for (int i = 0; i < 6; i++) {
        int face[4][2];
        for (int j = 0; j < 4; j++) {
            face[j][0] = vertices[faces[i][j]][0];
            face[j][1] = vertices[faces[i][j]][1];
        }    
        bga_draw_trapezoid(face[0][0], face[0][1], face[1][0], face[1][1], face[2][0], face[2][1], face[3][0], face[3][1], 0xff0000);
    }


    bga_draw_rectangle(958, 538, 4, 4, 0xFF0000);
}


void enter_3d_test() {
    bga_set_video_mode(1920, 1080, 32, 1, 1);

    plot_cube(5, 0, 5, 0xFF0000, "cube1");

    draw_view();
}