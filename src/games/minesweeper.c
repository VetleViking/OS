#include <system.h>

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>


char mine_sweeper_map[25][80] = {
	'\0',
	'\0',
	'\0',
	'\0',
	'\0',
	'\0',
	'\0',
	'\0',
	'\0',
	'\0',
	'\0',
	'\0',
	'\0',
	'\0',
	'\0',
	'\0',
	'\0',
	'\0',
	'\0',
	'\0',
	'\0',
	'\0',
	'\0',
	'\0',
	'\0'
};

char mine_sweeper_shown_map[25][80] = {
	'\0',
	'\0',
	'\0',
	'\0',
	'\0',
	'\0',
	'\0',
	'\0',
	'\0',
	'\0',
	'\0',
	'\0',
	'\0',
	'\0',
	'\0',
	'\0',
	'\0',
	'\0',
	'\0',
	'\0',
	'\0',
	'\0',
	'\0',
	'\0',
	'\0'
};


struct Color_coords{
	int x;
	int y;
	int color;
};

struct Color_coords color_coords[2000] = {
	{0, 0, 0}
};

int w = 20;
int h = 10;

int num_color_coords = 0;

int start_ms_time = 0;

bool in_mine_sweeper = false;


void print_ms_map() {
	bool was_writing_command = is_writing_command;

	is_writing_command = false;

	int prev_ter_col = terminal_column;
	int prev_ter_row = terminal_row;

	terminal_column = 0;
	terminal_row = 0;
	for (int i = 0; i < h; i++) {
		terminal_writestring(mine_sweeper_shown_map[i]);
		terminal_writestring("|");
		newline();
	}
	for (int i = 0; i < w; i++) {
		terminal_writestring("-");
	}
	terminal_writestring("|");

	for (int i = 0; i < num_color_coords; i++) {
		int x = color_coords[i].x;
		int y = color_coords[i].y;
		terminal_putentryat(mine_sweeper_shown_map[y][x], vga_entry_color(color_coords[i].color, VGA_COLOR_BLACK), x, y);
	}

	terminal_column = prev_ter_col;
	terminal_row = prev_ter_row;

	is_writing_command = was_writing_command;
}


bool check_ms_win() {
	for (int i = 0; i < h; i++) {
		for (int j = 0; j < w; j++) {
			if (mine_sweeper_map[i][j] == 'M' && mine_sweeper_shown_map[i][j] != 'F') {
				return false;
			}
		}
	}
	return true;
}


void remove_color_coords(int x, int y) {
	for (int i = 0; i < num_color_coords; i++) {
		if (color_coords[i].x == x && color_coords[i].y == y) {
			for (int j = i; j < num_color_coords; j++) {
				color_coords[j] = color_coords[j + 1];
			}
			num_color_coords--;
			break;
		}
	}
}


void add_num_color(int num_mines) {
	switch (num_mines) {
		case 1:
		
			color_coords[num_color_coords].color = VGA_COLOR_GREEN;
			break;
		case 2:
			color_coords[num_color_coords].color = VGA_COLOR_LIGHT_RED;
			break;

		case 3:
		case 4:
		case 5:
		case 6:
		case 7:
		case 8:
			color_coords[num_color_coords].color = VGA_COLOR_RED;
			break;
	}
}

int times_checked = 0;
void check_surroundings(int y, int x) {
	int start_x = x - 1;
	int start_y = y - 1;

	for (int i = start_x; i < start_x + 3; i++) {
		for (int j = start_y; j < start_y + 3; j++) {
			if (i < 0 || i > w - 1 || j < 0 || j > h - 1) {
				continue;
			} 
			if (mine_sweeper_shown_map[j][i] == ' ') {
				continue;
			}
			if (mine_sweeper_shown_map[j][i] == 'F') {
				continue;
			}
			if (mine_sweeper_shown_map[j][i] == '1') {
				continue;
			}
			
			int num_mines = check_mines(j, i);

			if (num_mines == 0) {
				mine_sweeper_shown_map[j][i] = ' ';
				times_checked++;

				if (times_checked > 200) {
					return;
				}

				check_surroundings(j, i);
				continue;
			}
			
			add_num_color(num_mines);
			color_coords[num_color_coords].x = i;
			color_coords[num_color_coords].y = j;
			num_color_coords++;

			mine_sweeper_shown_map[j][i] = num_mines + '0';
		}
	}
}


int num_mines = 0;
bool mines_setup = false;

void setup_mines() {
	int num_mines_placed = 0;
	int i = 0;

	while (num_mines_placed < num_mines) {
		int x = rand(num_mines_placed + i * 1332126) % w;
		int y = i; // did not work if both were random

		if (mine_sweeper_map[y][x] == 'M' || (x == terminal_column && y == terminal_row)) {
			continue;
		} else if (mine_sweeper_map[y][x] == '#'){
			mine_sweeper_map[y][x] = 'M';
			num_mines_placed++;
		}

		i++;
		if (i == h) {
			i = 0;
		}
	}

	mines_setup = true;
}


int check_mines(int y, int x) {
	int num_mines = 0;

	int start_x = x - 1;
	int start_y = y - 1;

	for (int i = start_x; i < start_x + 3; i++) {
		for (int j = start_y; j < start_y + 3; j++) {
			if (i < 0 || i > w - 1 || j < 0 || j > h - 1) {
				continue;
			}
			if (mine_sweeper_map[j][i] == 'M') {
				num_mines++;
			}
		}
	}

	return num_mines;
}


void ms_keyboard_handler(c) {
	is_writing_command = false;

	if (c == 75) { // left
		if (terminal_column > 0) {
			terminal_column--;
			move_cursor(terminal_column, terminal_row);
		}
	} else if (c == 77) { // right
		if (terminal_column < w - 1) {
			terminal_column++;
			move_cursor(terminal_column, terminal_row);
		}
	} else if (c == 72) { // up
		if (terminal_row > 0) {
			terminal_row--;
			move_cursor(terminal_column, terminal_row);
		}
	} else if (c == 80) { // down
		if (terminal_row < h - 1) {
			terminal_row++;
			move_cursor(terminal_column, terminal_row);
		}
	} else if (c == 28)	{ // enter
		if (mine_sweeper_shown_map[terminal_row][terminal_column] != 'F') {
			if (mine_sweeper_map[terminal_row][terminal_column] == 'M') {
				in_game = false;
				in_mine_sweeper = false;
				is_writing_command = false;
				check_scroll = true;
				clear_screen();
				for (int i = 0; i < h; i++) {
					for (int j = 0; j < w; j++) {
						if (mine_sweeper_map[i][j] == 'M') {
							terminal_putentryat('M', vga_entry_color(VGA_COLOR_RED, VGA_COLOR_BLACK), j, i);
						} else {
							terminal_putentryat(' ', vga_entry_color(VGA_COLOR_LIGHT_MAGENTA, VGA_COLOR_BLACK), j, i);
						}
					}
					terminal_column = w;
					terminal_row = i;
					vga_entry_color(VGA_COLOR_LIGHT_MAGENTA, VGA_COLOR_BLACK);
					terminal_writestring("|");				
					newline();
				}
				vga_entry_color(VGA_COLOR_LIGHT_MAGENTA, VGA_COLOR_BLACK);
				
				for (int i = 0; i < w; i++) {
					terminal_writestring("-");
				}
				terminal_writestring("|");

				newline();
				terminal_writestring("You lost!");
				new_kernel_line();
				move_cursor(terminal_column, terminal_row);
				return;
			} else {
				if (!mines_setup) {
					setup_mines();
				}

				int num_mines = 0;
				num_mines = check_mines(terminal_row, terminal_column);

				if (num_mines == 0) {
					mine_sweeper_shown_map[terminal_row][terminal_column] = ' ';
					times_checked = 0;
					check_surroundings(terminal_row, terminal_column);
				} else {
					add_num_color(num_mines);
					color_coords[num_color_coords].x = terminal_column;
					color_coords[num_color_coords].y = terminal_row;
					num_color_coords++;

					mine_sweeper_shown_map[terminal_row][terminal_column] = num_mines + '0';
				}
			}
		}
		print_ms_map();
	} else if (c == 33) { //F
		if (mine_sweeper_shown_map[terminal_row][terminal_column] == 'F') {
			remove_color_coords(terminal_column, terminal_row);
			mine_sweeper_shown_map[terminal_row][terminal_column] = '#';
		} else if (mine_sweeper_shown_map[terminal_row][terminal_column] == '#') {
			mine_sweeper_shown_map[terminal_row][terminal_column] = 'F';

			color_coords[num_color_coords].x = terminal_column;
			color_coords[num_color_coords].y = terminal_row;
			color_coords[num_color_coords].color = VGA_COLOR_LIGHT_BLUE;
			num_color_coords++;

			if (check_ms_win()) {
				in_game = false;
				in_mine_sweeper = false;
				is_writing_command = false;
				check_scroll = true;
				clear_screen();
				terminal_writestring("You won!");
				newline();
				terminal_writestring("You won in ");
				print_ticks_to_time(timer_ticks - start_ms_time);
				new_kernel_line();
				move_cursor(terminal_column, terminal_row);
				return;
			}
		}
		print_ms_map();
	}

	is_writing_command = true;
}

void mine_sweeper_play() {
	int framerate = 100 / 100; // 100 is 1 second

	int update_rate = framerate * 50;
	int last_update = 0;

	is_writing_command = true;
	in_mine_sweeper = true;

	print_ms_map();

	while (in_mine_sweeper) {
		last_update++;

		if (last_update == update_rate) { // dont think ill need this, but just in case
			last_update = 0;
		}

		sleep(framerate);
	}
}


void mine_sweeper_start() {
	start_ms_time = timer_ticks;

	check_scroll = false;	
	in_mine_sweeper = true;
	mines_setup = false;

	clear_screen();

	w = 20;
	h = 10;
	num_mines = 30;


	if (has_params) {
		char buffer[10];
		char params[10][10];
		int num_params = 0;

		int len = strlen(game_params);
        int start = 0;

        while (start < len) {
            int i;
            for (i = start; i < len; i++) {
                if (game_params[i] == ' ' || game_params[i] == '\0') {
                    buffer[i - start] = '\0';
                    break;
                }
                buffer[i - start] = game_params[i];
            }

            if (i == len) {
                buffer[i - start] = '\0';
            }

            // Skip the minus sign when copying to params
            strcpy(params[num_params], buffer + 1);
            num_params++;

            start = i + 1;
        }

		for (int i = 0; i < num_params; i++) {
			if (strcmplen(params[i], "w", 1) == 0) {
				for (int j = 1; j < strlen(params[i]); j++) {
					buffer[j - 1] = params[i][j];
				}
				buffer[strlen(params[i]) - 1] = '\0';
				w = atoi(buffer);
				
			} else if (strcmplen(params[i], "h", 1) == 0) {
				for (int j = 1; j < strlen(params[i]); j++) {
					buffer[j - 1] = params[i][j];
				}
				buffer[strlen(params[i]) - 1] = '\0';
				h = atoi(buffer);

			} else if (strcmplen(params[i], "m", 1) == 0) {
				for (int j = 1; j < strlen(params[i]); j++) {
					buffer[j - 1] = params[i][j];
				}
				buffer[strlen(params[i]) - 1] = '\0';
				num_mines = atoi(buffer);
			}
		}

		if (w > 78) {
			w = 78;
		}
		if (h > 23) {
			h = 23;
		}
		if (num_mines > w * h) {
			num_mines = w * h;
		}
	}


	for (int i = 0; i < h; i++) {
		for (int j = 0; j < w; j++) {
			mine_sweeper_map[i][j] = '#';
			mine_sweeper_shown_map[i][j] = '#';
		}
		mine_sweeper_map[i][w] = '\0';
		mine_sweeper_shown_map[i][w] = '\0';
	}

	while (num_color_coords > 0) {
		color_coords[num_color_coords].x = 0;
		color_coords[num_color_coords].y = 0;
		color_coords[num_color_coords].color = 0;
		num_color_coords--;
	}
}