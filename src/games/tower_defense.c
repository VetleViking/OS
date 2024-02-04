#include <system.h>

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>


int tower_defense_money = 100;
int tower_defense_health = 100;
char tower_defense_map [22][81] = {
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
};

int td_color_placement[2][1] = {
	0, 0
};


bool in_TD = false;


void change_all_price_colors(int color) {
	int prev_ter_col = terminal_column;
	for (int i = terminal_column; i < 80; i++) {
		if (tower_defense_map[terminal_row][i] == ' ') {
			terminal_column = prev_ter_col;
			move_cursor(terminal_column, terminal_row);
			return;
		} else {
			terminal_column = i;
			change_cursorpos_bgcolor(color);
		}
	}
}


void change_cursorpos_bgcolor(int color) {
	terminal_color = vga_entry_color(chosen_color, color);
	const size_t index = terminal_column + terminal_row * VGA_WIDTH;
	terminal_buffer[index] = vga_entry(terminal_buffer[index], terminal_color);
	terminal_color = vga_entry_color(chosen_color, chosen_bg_color);	
}

void print_td_map() {
	bool was_writing_command = is_writing_command;
	bool was_in_TD = in_TD;

	is_writing_command = false;
	in_TD = false;

	int prev_ter_col = terminal_column;
	int prev_ter_row = terminal_row;

	terminal_column = 0;
	terminal_row = 0;
	for (int i = 0; i < 22; i++) {
		terminal_writestring(tower_defense_map[i]);
		if (td_color_placement[0][0] != 0 && td_color_placement[0][1] != 0) {
			if (i == td_color_placement[0][1]) {
				terminal_column = td_color_placement[0][0];
				terminal_row = td_color_placement[0][1];
				change_all_price_colors(VGA_COLOR_BLUE);
				terminal_row = i + 1;
				terminal_column = 0;
			}
		
		}
	}

	terminal_column = prev_ter_col;
	terminal_row = prev_ter_row;

	is_writing_command = was_writing_command;
	in_TD = was_in_TD;
}

struct Tower{
	int x;
	int y;
	int damage;
	int fire_rate;
	int last_shot;
	int direction;
};	

struct Tower tower_list[40] = {
	{0, 0, 0, 0, 0, 0}
};

int num_towers = 0;

bool buying_tower = false;

void td_keyboard_handler(c) {
	is_writing_command = false;
	in_TD = false; // janky, fix later mby

	if (c == 48) { // b
		if (!buying_tower) {
			buying_tower = true;
			char help_str[80] = "Where do you want to place the tower? (arrow keys to move,enter to place)";

			int row = 1;
			int i2 = 22;
			for (int i = 0; i < strlen(help_str); i++) {
				if (i2 > 79) {
					row = 2;
					i2 = 22;
				}
				tower_defense_map[row][i2] = help_str[i];
				i2++;
			}
			
			terminal_column = 5;	
			terminal_row = 7;

			td_color_placement[0][0] = terminal_column;
			td_color_placement[0][1] = terminal_row;
		}
	} else if (c == 75) { // left
		if (buying_tower) {
			for (int i = terminal_column - 1; i > 0; i--) {
				if (tower_defense_map[terminal_row][i] == '$' || tower_defense_map[terminal_row][i] == 'T') {
					terminal_column = i;

					td_color_placement[0][0] = terminal_column;
					td_color_placement[0][1] = terminal_row;
					break;
				}
			}
		}
	} else if (c == 77) { // right
		if (buying_tower) {
			for (int i = terminal_column + 1; i < 80; i++) {
				if (tower_defense_map[terminal_row][i] == '$' || tower_defense_map[terminal_row][i] == 'T') {
					terminal_column = i;
					
					td_color_placement[0][0] = terminal_column;
					td_color_placement[0][1] = terminal_row;
					break;
				}
			}
		}
	} else if (c == 72) { // up
		if (buying_tower) {
			for (int i = terminal_row - 1; i > 0; i--) {
				if (tower_defense_map[i][terminal_column] == '$' || tower_defense_map[i][terminal_column] == 'T') {
					terminal_row = i;

					td_color_placement[0][0] = terminal_column;
					td_color_placement[0][1] = terminal_row;
					break;
				}
			}
		}
	} else if (c == 80) { // down
		if (buying_tower) {
			for (int i = terminal_row + 1; i < 22; i++) {
				if (tower_defense_map[i][terminal_column] == '$' || tower_defense_map[i][terminal_column] == 'T') {
					terminal_row = i;

					td_color_placement[0][0] = terminal_column;
					td_color_placement[0][1] = terminal_row;
					break;
				}
			}
		}
	} else if (c == 28) { // enter
		if (buying_tower) {
			if (tower_defense_map[terminal_row][terminal_column] == '$') {
				int tower_price = 0;

				for (int i = terminal_column + 1; i < 80; i++) {
					if (tower_defense_map[terminal_row][i] == ' ') {
						break;
					} else {
						tower_price = tower_price * 10 + (tower_defense_map[terminal_row][i] - '0' || tower_defense_map[i][terminal_column] == 'T');
					}
				}

				if (tower_defense_money >= tower_price) {
					tower_defense_money = tower_defense_money - tower_price;

					int row = 1;
					int i2 = 22;
					for (int i = 0; i < 116; i++) {
						if (i2 > 79) {
							row = 2;
							i2 = 22;
						}
						tower_defense_map[row][i2] = ' ';
						i2++;
					}

					td_color_placement[0][0] = 0;
					td_color_placement[0][1] = 0;

					tower_defense_map[terminal_row][terminal_column] = 'T';					

					tower_list[num_towers].x = terminal_column;
					tower_list[num_towers].y = terminal_row;
					tower_list[num_towers].damage = 10;
					tower_list[num_towers].fire_rate = 10;
					tower_list[num_towers].last_shot = 0;
					
					if (tower_list[num_towers].y < 10) {
						tower_list[num_towers].direction = 1;
					} else {
						tower_list[num_towers].direction = 2;
					}

					num_towers++;
				
					for (int i = terminal_column + 1; i < 80; i++) {
						if (tower_defense_map[terminal_row][i] == ' ') {
							break;
						} else {
							tower_defense_map[terminal_row][i] = ' ';
						}
					}

					terminal_column = 0;
					terminal_row = 26;
					move_cursor(terminal_column, terminal_row);
					
					 
				}
			}
			buying_tower = false;
		}
	}

	in_TD = true;
	is_writing_command = true;

}



int start_time = 0;


struct Bullet{
    int x;
    int y;
    int direction; // 1 for up, 2 for down
	char under_bullet;
};

struct Bullet bullet_list[40] = {
	{0, 0, 0, ' '}
};

int num_bullets = 0;



void remove_bullet(int index) {
	for (int i = index; i < num_bullets; i++) {
		bullet_list[i] = bullet_list[i + 1];
	}
	num_bullets--;
}

void tower_defense_play() {
	start_time = timer_ticks;

	int framerate = 100 / 100; // 100 is 1 second

	int update_rate = framerate * 50; // lower is faster, used for most things, like enemies moving and bullets moving
	int last_update = 0;

	int enemy_health = 10;
	int enemy_speed = 1; // uses update_rate, 1 is every update.
	int last_walked = 0;
	int time_since_last_enemy = 0;

	int tower_damage = 10;
	int fire_rate = 2; // uses update_rate, 2 is every second update.
	int last_shot = 0;


	is_writing_command = true;
	in_TD = true;

	while (in_TD) {
		last_update++;

		char health_str[10];
		itoa(tower_defense_health, health_str, 10);
		char money_str[10];
		itoa(tower_defense_money, money_str, 10);

		for (int i = 0; i < 7; i++) {
			if (tower_defense_map[1][i + 9] == ' ' && tower_defense_map[2][i + 9] == ' ') {
				break;				
			}

			tower_defense_map[1][i + 9] = ' ';
			tower_defense_map[2][i + 9] = ' ';

			if (i < strlen(health_str)) {
				tower_defense_map[1][i + 9] = health_str[i];
			}

			if (i < strlen(money_str)) {
				tower_defense_map[2][i + 9] = money_str[i];
			}
		}

		if (last_update == update_rate) {
			last_update = 0;

			last_walked++;
			last_shot++;

			if (last_walked == enemy_speed) {
				time_since_last_enemy++;
				last_walked = 0;

				int moved_enemy = 100;

				if (time_since_last_enemy > 5) {
					time_since_last_enemy = 0;
					tower_defense_map[10][0] = 'E';
					moved_enemy = 0;
				}

				for (int i = 0; i < 80; i++) {
					if (tower_defense_map[10][i] == 'E') {
						if (i == 79) {
							tower_defense_health = tower_defense_health - 10;
							
							if (tower_defense_health <= 0) {
								in_game = false;
								in_TD = false;
								is_writing_command = false;
								check_scroll = true;
								clear_screen();
								terminal_writestring("You lost!");
								newline();
								terminal_writestring("You survived for ");
								print_ticks_to_time(timer_ticks - start_time);
								new_kernel_line();
								move_cursor(terminal_column, terminal_row);
								return;
							}

							tower_defense_map[10][i] = ' ';
						} else {
							if (moved_enemy - i == 0) {
								continue;
							}
							tower_defense_map[10][i] = ' ';
							tower_defense_map[10][i + 1] = 'E';
							moved_enemy = i + 1;
						}
					}
				}
			}

			
			for (int i = 0; i < num_bullets; i++) {
				tower_defense_map[bullet_list[i].y][bullet_list[i].x] = bullet_list[i].under_bullet;	
				if (bullet_list[i].direction == 1) {
					bullet_list[i].y++;
				} else if (bullet_list[i].direction == 2) {
					bullet_list[i].y--;
				}

				bullet_list[i].under_bullet = tower_defense_map[bullet_list[i].y][bullet_list[i].x];

				if (tower_defense_map[bullet_list[i].y][bullet_list[i].x] == 'E') {
					enemy_health = enemy_health - tower_damage;
					if (enemy_health <= 0) {
						tower_defense_money = tower_defense_money + 10;
						tower_defense_map[bullet_list[i].y][bullet_list[i].x] = 'd';
					}

					remove_bullet(i);
					i--;
				} else if (bullet_list[i].y == 10) {
					remove_bullet(i);
					i--;
				} else {
					tower_defense_map[bullet_list[i].y][bullet_list[i].x] = 'B';
				}
			}
				
			for (int i = 0; i < num_towers; i++) {
				if (tower_list[i].fire_rate == tower_list[i].last_shot) {
					if (tower_defense_map[10][tower_list[i].x - 2] == 'E') {
						tower_list[i].last_shot = 0;
						
						bullet_list[num_bullets].x = tower_list[i].x;
						bullet_list[num_bullets].direction = tower_list[i].direction;
							
						if (tower_list[i].direction == 1) {
							bullet_list[num_bullets].y = tower_list[i].y + 1;
							bullet_list[num_bullets].under_bullet = tower_defense_map[tower_list[i].y + 1][tower_list[i].x];

							tower_defense_map[tower_list[i].y + 1][tower_list[i].x] = 'B';
						}
						
						if (tower_list[i].direction == 2) {
							bullet_list[num_bullets].y = tower_list[i].y - 1;
							bullet_list[num_bullets].under_bullet = tower_defense_map[tower_list[i].y - 1][tower_list[i].x];

							tower_defense_map[tower_list[i].y - 1][tower_list[i].x] = 'B';
						}
							
						num_bullets++;
					}
				} else {
					tower_list[i].last_shot++;
				}
			}

			for (int i = 0; i < 80; i++) {
				if (tower_defense_map[10][i] == 'd') {
					tower_defense_map[10][i] = '#';
				} else if (tower_defense_map[10][i] == '#') {	
					tower_defense_map[10][i] = '*';
				} else if (tower_defense_map[10][i] == '*') {
					tower_defense_map[10][i] = '\'';
				} else if (tower_defense_map[10][i] == '\'') {
					tower_defense_map[10][i] = ' ';
				}
			}
		}

		print_td_map();

		sleep(framerate);
	}
}


void tower_defense_start() {
	check_scroll = false;	

	in_TD = true;

	clear_screen();

	for (int i = 0; i < 22; i++) { // clears the map
		for (int j = 0; j < 80; j++) {
			tower_defense_map[i][j] = ' ';
		}
		tower_defense_map[i][80] = '\0';
	}

	for (int i = 0; i < 20; i++) { // makes the top of the map
		tower_defense_map[3][i] = '-';
	}
	tower_defense_map[3][20] = 217;
	tower_defense_map[2][20] = 179;
	tower_defense_map[1][20] = 179;
	tower_defense_map[0][20] = 179;

	char health_str[10];
	itoa(tower_defense_health, health_str, 10);
	char money_str[10];
	itoa(tower_defense_money, money_str, 10);

	char health_str2[20] = "Health: ";
	char money_str2[20] = "Money:  ";

	int health_len = strlen(health_str2);
	int money_len = strlen(money_str2);

	for (int i = health_len; i < health_len + strlen(health_str); i++) {
		health_str2[i] = health_str[i - health_len];
	}
	for (int i = money_len; i < money_len + strlen(money_str); i++) {
		money_str2[i] = money_str[i - money_len];
	}

	for (int i = 0; i < strlen(health_str2); i++) { // prints the health
		tower_defense_map[1][i + 1] = health_str2[i];
	}
	for (int i = 0; i < strlen(money_str2); i++) { // prints the money
		tower_defense_map[2][i + 1] = money_str2[i];
	}
	
	for (int i = 0; i < 80; i++) { // makes the path
		tower_defense_map[9][i] = '-';
		tower_defense_map[11][i] = '-';
	}

	for (int i = 5; i < 75;) { // makes the tower placement areas
		tower_defense_map[7][i] = '$';
		tower_defense_map[7][i + 1] = '1';
		tower_defense_map[7][i + 2] = '0';
		tower_defense_map[13][i] = '$';
		tower_defense_map[13][i + 1] = '2';
		tower_defense_map[13][i + 2] = '0';

		i = i + 10;
	}

	for (int i = 0; i < 25; i++) { // prints the map
		terminal_writestring(tower_defense_map[i]);
	}

	terminal_putchar(201);
	for (size_t i = 0; i < 78; i++) {
		terminal_putchar(205);
	}
	terminal_putchar(187);
	terminal_putchar(186);
	terminal_writestring(" Tower defense ----------------------------------------------- B to buy tower ");
	terminal_putchar(186);
	terminal_putchar(200);
	for (size_t i = 0; i < 78; i++) {
		terminal_putchar(205);
	}
	terminal_putchar(188);

	game_round++;
}