#include <system.h>

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>



bool in_pong = false;

int p1_pos = 80;
int p2_pos = 80;

int p1_score = 0;
int p2_score = 0;

int ball_pos[2] = {160, 100};
int ball_dir = 0;


// this function will draw the number at the given x and y coordinates
// the number is 40 pixels high and 25 pixels wide
void print_number(int x, int y, int number) {
    draw_rectangle(x, y, 25, 40, VGA_COLOR_BLACK);

    switch (number) {
        case 0:
            draw_rectangle(x, y, 25, 40, VGA_COLOR_WHITE);
            draw_rectangle(x + 3, y + 3, 19, 34, VGA_COLOR_BLACK);            
            break;

        case 1:
            draw_rectangle(x + 22, y, 3, 40, VGA_COLOR_WHITE);            
            break;

        case 2:
            draw_rectangle(x, y, 25, 3, VGA_COLOR_WHITE);
            draw_rectangle(x + 22, y, 3, 20, VGA_COLOR_WHITE);
            draw_rectangle(x, y + 19, 25, 3, VGA_COLOR_WHITE);
            draw_rectangle(x, y + 20, 3, 20, VGA_COLOR_WHITE);
            draw_rectangle(x, y + 37, 25, 3, VGA_COLOR_WHITE);
            break;

        case 3:
            draw_rectangle(x, y, 25, 3, VGA_COLOR_WHITE);
            draw_rectangle(x, y + 19, 25, 3, VGA_COLOR_WHITE);
            draw_rectangle(x, y + 37, 25, 3, VGA_COLOR_WHITE);
            draw_rectangle(x + 22, y, 3, 40, VGA_COLOR_WHITE);
            break;

        case 4: 
            draw_rectangle(x, y, 3, 20, VGA_COLOR_WHITE);
            draw_rectangle(x + 22, y, 3, 40, VGA_COLOR_WHITE);
            draw_rectangle(x, y + 19, 25, 3, VGA_COLOR_WHITE);
            break;

        case 5:
            draw_rectangle(x, y, 25, 3, VGA_COLOR_WHITE);
            draw_rectangle(x, y, 3, 20, VGA_COLOR_WHITE);
            draw_rectangle(x, y + 19, 25, 3, VGA_COLOR_WHITE);
            draw_rectangle(x + 22, y + 20, 3, 20, VGA_COLOR_WHITE);
            draw_rectangle(x, y + 37, 25, 3, VGA_COLOR_WHITE);
            break;

        case 6:
            draw_rectangle(x, y, 25, 3, VGA_COLOR_WHITE);
            draw_rectangle(x, y, 3, 40, VGA_COLOR_WHITE);
            draw_rectangle(x, y + 19, 25, 3, VGA_COLOR_WHITE);
            draw_rectangle(x + 22, y + 20, 3, 20, VGA_COLOR_WHITE);
            draw_rectangle(x, y + 37, 25, 3, VGA_COLOR_WHITE);
            break;

        case 7:
            draw_rectangle(x, y, 25, 3, VGA_COLOR_WHITE);
            draw_rectangle(x + 22, y, 3, 40, VGA_COLOR_WHITE);
            break;

        case 8:
            draw_rectangle(x, y, 25, 40, VGA_COLOR_WHITE);
            draw_rectangle(x + 3, y + 3, 19, 34, VGA_COLOR_BLACK); 
            draw_rectangle(x, y + 19, 25, 3, VGA_COLOR_WHITE);
            break;

        case 9:
            draw_rectangle(x, y, 25, 3, VGA_COLOR_WHITE);
            draw_rectangle(x + 22, y, 3, 40, VGA_COLOR_WHITE);
            draw_rectangle(x, y + 19, 25, 3, VGA_COLOR_WHITE);
            draw_rectangle(x, y, 3, 20, VGA_COLOR_WHITE);
            draw_rectangle(x, y + 37, 25, 3, VGA_COLOR_WHITE);
            break;
    };
}


void pong_draw_map_parts() {
    for (int i = 5; i < 200; i += 15) {
        draw_rectangle(159, i, 3, 10, VGA_COLOR_WHITE);
    }

    if (p1_score >= 10) {
        print_number(90, 5, p1_score / 10);
    }
    print_number(125, 5, p1_score % 10);

    if (p2_score >= 10) {
        print_number(170, 5, p2_score / 10);
        print_number(205, 5, p2_score % 10);
    } else {
        print_number(170, 5, p2_score % 10);
    }

}


void pong_make_map() {
    draw_rectangle(10, 80, 3, 40, VGA_COLOR_WHITE);

    draw_rectangle(310, 80, 3, 40, VGA_COLOR_WHITE);

    pong_draw_map_parts();
}

void pong_keyboard_handler(c) {
    if (c == 72) { // up
        if (p2_pos > 0) {
            draw_rectangle(310, p2_pos, 3, 40, VGA_COLOR_BLACK);
            p2_pos -= 5;
            draw_rectangle(310, p2_pos, 3, 40, VGA_COLOR_WHITE);
        }
        if (p2_pos > 0) {
            draw_rectangle(310, p2_pos, 3, 40, VGA_COLOR_BLACK);
            p2_pos -= 5;
            draw_rectangle(310, p2_pos, 3, 40, VGA_COLOR_WHITE);
        }
	} else if (c == 80) { // down
        if (p2_pos < 160) {
            draw_rectangle(310, p2_pos, 3, 40, VGA_COLOR_BLACK);
            p2_pos += 5;
            draw_rectangle(310, p2_pos, 3, 40, VGA_COLOR_WHITE);
        }
    } else if (c == 17) { // w
        if (p1_pos > 0) {
            draw_rectangle(10, p1_pos, 3, 40, VGA_COLOR_BLACK);
            p1_pos -= 5;
            draw_rectangle(10, p1_pos, 3, 40, VGA_COLOR_WHITE);
        }
    } else if (c == 31) { // s
        if (p1_pos < 160) {
            draw_rectangle(10, p1_pos, 3, 40, VGA_COLOR_BLACK);
            p1_pos += 5;
            draw_rectangle(10, p1_pos, 3, 40, VGA_COLOR_WHITE);
        }
    } else if (c == 17) { // w
        if (p1_pos > 0) {
            draw_rectangle(10, p1_pos, 3, 40, VGA_COLOR_BLACK);
            p1_pos -= 5;
            draw_rectangle(10, p1_pos, 3, 40, VGA_COLOR_WHITE);
        }
    } else if (c == 31) { // s
        if (p1_pos < 160) {
            draw_rectangle(10, p1_pos, 3, 40, VGA_COLOR_BLACK);
            p1_pos += 5;
            draw_rectangle(10, p1_pos, 3, 40, VGA_COLOR_WHITE);
        }
    }
}

void pong_play() {
    ball_dir = 0;

    while (in_pong) {
        sleep(1); // sleep for 0.1s
        
        // Start ball movement
        if (ball_pos[1] < 0) {
            ball_dir = (ball_dir == 1) ? 4 : 3;
        } else if (ball_pos[1] > 195) {
            ball_dir = (ball_dir == 3) ? 2 : 1;
        }

        if (ball_pos[0] < 0) {
            p2_score++;
            print_number(170, 5, p2_score);
            draw_rectangle(ball_pos[0], ball_pos[1], 3, 40, VGA_COLOR_BLACK);
            ball_pos[0] = 160;
            ball_pos[1] = 100;
            ball_dir = 0;
        } else if (ball_pos[0] > 315) {
            p1_score++;
            print_number(125, 5, p1_score);
            draw_rectangle(ball_pos[0], ball_pos[1], 3, 40, VGA_COLOR_BLACK);
            ball_pos[0] = 160;
            ball_pos[1] = 100;
            ball_dir = 0;
        }

        if (ball_pos[0] < 14 && ball_pos[1] < p1_pos + 40 && ball_pos[1] > p1_pos) {
            ball_dir = (ball_dir == 1) ? 2 : 3;
        } else if (ball_pos[0] > 305 && ball_pos[1] < p2_pos + 40 && ball_pos[1] > p2_pos) {
            ball_dir = (ball_dir == 2) ? 1 : 4;
        }

        if (ball_dir == 0) {
            do { ball_dir = rand(p1_pos + p2_pos + p1_score + p2_score) % 4 + 1;
            } while (ball_dir < 1 || ball_dir > 4);
        }

        draw_rectangle(ball_pos[0], ball_pos[1], 3, 3, VGA_COLOR_BLACK);

        switch (ball_dir) {
            case 1:
                ball_pos[0] -= 1;
                ball_pos[1] -= 1;
                break;
            case 2:
                ball_pos[0] += 1;
                ball_pos[1] -= 1;
                break;
            case 3:
                ball_pos[0] += 1;
                ball_pos[1] += 1;
                break;
            case 4:
                ball_pos[0] -= 1;
                ball_pos[1] += 1;
                break;
            default:
                ball_pos[0] = 20;
                ball_pos[1] = 20;
                
                
        }

        pong_draw_map_parts();
        draw_rectangle(ball_pos[0], ball_pos[1], 3, 3, VGA_COLOR_WHITE);
        
        // End ball movement
    }

    remove_keyboard_handler(pong_keyboard_handler);
    remove_main_loop_call(pong_play);
}  

void pong_start() {
    add_keyboard_handler(pong_keyboard_handler);
    add_main_loop_call(pong_play);
    in_pong = true;

    vga_enter();
    vga_clear_screen();
    pong_make_map();
}



