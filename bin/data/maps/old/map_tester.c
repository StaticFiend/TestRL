#include <stdio.h>
#include <math.h>
#include "../../../include/libtcod.h"

//#define DEBUG //Comment/uncomment me when you don't/do need debugging

#define MAP_WIDTH 80
#define MAP_HEIGHT 40
#define SCREEN_WIDTH 80
#define SCREEN_HEIGHT 50

#define WALL 35
#define LAVA 39
#define GRASS 44
#define GROUND 46
#define STAIRS 62
#define PLAYER 64
#define WATER 126

int player_x;
int player_y;

int player_currentHP;
int player_currentMP;
int player_maxHP;
int player_maxMP;

float player_hpPct;
float player_mpPct;

char player_sym;

/*
 * This system is not really designed well for TCOD's FOV system, to do this more effectively I'd
 * have to read in the map to a char, pass that to another function to build a TCOD_map, compute
 * the FOV, then draw the map.  Think about changing the whole system to accomidate this.  Also
 * building a TCOD_map_t of the level is smart so we can use not only the FOV system, but we can
 * also use TCOD's pathfinding system.
*/
void drawMap(const char *filename, int *done, TCOD_color_t *grass_color) {
//	static TCOD_map_t map_fov;

	static char map[MAP_HEIGHT * MAP_WIDTH];
	static int first_run = 1;

	int randNum;

	TCOD_color_t water_temp;
//	TCOD_color_t fov_color[2];

	int x = 0, y = 0;
	FILE *file;

	file = fopen(filename, "r");

	if (!file) {
		printf("[Console] Could not find map %s, exiting...\n", filename);
		*done = 1;
		return;
	}

	//For drawing if things aren't visible with TCOD's FOV system, I've thought of using
	//color multiplication to represent this, if the object is visible it should look like
	//the regular colors, if not it'll be a bit darker.

	//0 = not visible
//	fov_color[0].r = 127;
//	fov_color[0].g = 127;
//	fov_color[0].b = 127;
	//1 = visible
//	fov_color[1].r = 255;
//	fov_color[1].g = 255;
//	fov_color[1].b = 255;

	TCOD_console_clear(NULL);

	while (!feof(file)) {
		for (x = 0; x < MAP_HEIGHT * MAP_WIDTH; x++) {

			map[x] = fgetc(file);
			if (map[x] == -1 || map[x] == 10)
				map[x] = 35;
			if ((x % 80) == 0 && y < 39 && x != 0)
				y++;
			#ifdef DEBUG
			printf("(%i, %i, %c, %i)\n", x%80, y, map[x], map[x]);
			#endif

			switch((int)map[x]) {
				case WALL:
					TCOD_console_put_char_ex(NULL, x%80, y + 5, (int)map[x], TCOD_dark_grey, TCOD_black);
					break;
				case GROUND:
				case STAIRS:
					TCOD_console_put_char_ex(NULL, x%80, y + 5, (int)map[x], TCOD_white, TCOD_black);
					break;
				case PLAYER:
					TCOD_console_put_char_ex(NULL, x%80, y + 5, 46, TCOD_light_blue, TCOD_black);
					if (first_run == 1) {
						player_x = x%80;
						player_y = y + 5;
					}
					break;
				case GRASS:
					TCOD_console_put_char_ex(NULL, x%80, y + 5, (int)map[x], grass_color[x % 15], TCOD_black);
					break;
				case LAVA:
					TCOD_console_put_char_ex(NULL, x%80, y + 5, (int)map[x], TCOD_red, TCOD_black);
					break;
				case WATER:
					randNum = TCOD_random_get_int(NULL, 130, 200);

					water_temp.r = 0;
					water_temp.g = 0;
					water_temp.b = randNum;

					TCOD_console_put_char_ex(NULL, x%80, y + 5, (int)map[x], water_temp, TCOD_black);
					break;
			}

			if (x == 79 && y == 39)
				break;
		}
	}

	fclose(file);

	first_run = 0;
}

// * Figure out a cleaner way to present all this information.
// * Idea: Use HP/MP percentages like shown here, except use TCOD_console_put_char_ex to fill in the *'s
//   based on percentages.  There are 10 slots (each represent 10%) so this should be easy to fill in on the fly.
void drawHud() {
	int i;

	TCOD_console_set_color_control(TCOD_COLCTRL_1, TCOD_dark_grey, TCOD_black);
	
	if (player_currentHP < (player_maxHP / 2))
		TCOD_console_set_color_control(TCOD_COLCTRL_2, TCOD_red, TCOD_black);
	else if (player_currentHP < (player_maxHP / 1.25))
		TCOD_console_set_color_control(TCOD_COLCTRL_2, TCOD_yellow, TCOD_black);
	else
		TCOD_console_set_color_control(TCOD_COLCTRL_2, TCOD_white, TCOD_black);

	TCOD_console_print_left(NULL, 0, 46, TCOD_BKGND_NONE, "  \"The Debugger\"    HP: |%c----------%c| %c%i%c/%i     Str: 4  Dex: 5  Con: 3", TCOD_COLCTRL_1, TCOD_COLCTRL_STOP, TCOD_COLCTRL_2, player_currentHP, TCOD_COLCTRL_STOP, player_maxHP);
	TCOD_console_print_left(NULL, 0, 47, TCOD_BKGND_NONE, "                    MP: |%c----------%c| %i/%i     Int: 8  Wis: 7  Lck: 5", TCOD_COLCTRL_1, TCOD_COLCTRL_STOP, player_currentMP, player_maxMP);
	TCOD_console_print_left(NULL, 0, 48, TCOD_BKGND_NONE, "  Dlvl: 1                                      Lvl: 1          Exp: 1/4");
	for (i = 25; i < 25 + player_hpPct; i++) {
		TCOD_console_put_char_ex(NULL, i, 46, 42, TCOD_light_red, TCOD_black);
	}
	for (i = 25; i < 25 + player_mpPct; i++) {
		TCOD_console_put_char_ex(NULL, i, 47, 42, TCOD_light_blue, TCOD_black);
	}
	//Set up borders, this is done here because of drawing order.
	for (i = 0; i < SCREEN_WIDTH; i++) {
		if (i != 0 && i != SCREEN_WIDTH - 1){
			TCOD_console_put_char_ex(NULL, i, 45, TCOD_CHAR_HLINE, TCOD_grey, TCOD_black);
			TCOD_console_put_char_ex(NULL, i, 49, TCOD_CHAR_HLINE, TCOD_grey, TCOD_black);
		}
		else if (i == 0) {
			TCOD_console_put_char_ex(NULL, i, 45, TCOD_CHAR_NW, TCOD_grey, TCOD_black);
			TCOD_console_put_char_ex(NULL, i, 49, TCOD_CHAR_SW, TCOD_grey, TCOD_black);
			
			TCOD_console_put_char_ex(NULL, i, 46, TCOD_CHAR_VLINE, TCOD_grey, TCOD_black);
			TCOD_console_put_char_ex(NULL, i, 47, TCOD_CHAR_VLINE, TCOD_grey, TCOD_black);
			TCOD_console_put_char_ex(NULL, i, 48, TCOD_CHAR_VLINE, TCOD_grey, TCOD_black);
		}
		else if (i == SCREEN_WIDTH - 1){
			TCOD_console_put_char_ex(NULL, i, 45, TCOD_CHAR_NE, TCOD_grey, TCOD_black);
			TCOD_console_put_char_ex(NULL, i, 49, TCOD_CHAR_SE, TCOD_grey, TCOD_black);

			TCOD_console_put_char_ex(NULL, i, 46, TCOD_CHAR_VLINE, TCOD_grey, TCOD_black);
			TCOD_console_put_char_ex(NULL, i, 47, TCOD_CHAR_VLINE, TCOD_grey, TCOD_black);
			TCOD_console_put_char_ex(NULL, i, 48, TCOD_CHAR_VLINE, TCOD_grey, TCOD_black);
		}
	}
}

void initPlayer() {
	player_maxHP = 25;
	player_maxMP = 50;
	player_currentHP = 25;
	player_currentMP = 50;

	player_hpPct = (player_currentHP / player_maxHP) * 10;
	player_mpPct = (player_currentMP / player_maxMP) * 10;
}

int main(int argc, char **argv) {
	TCOD_key_t key;
	
	int done = 0, i;

	TCOD_color_t grass_color[15];

	player_sym = '@';

	if (argc > 1) {
		printf("[Console] Starting program...\n");
	}
	else {
		printf("[Console] No map specified, please try again.\n");
		return 0;
	}

	//Fall through, file was specified.
	TCOD_console_set_custom_font("terminal.png", TCOD_FONT_LAYOUT_TCOD|TCOD_FONT_TYPE_GREYSCALE, 0, 0);
	TCOD_console_init_root(SCREEN_WIDTH, SCREEN_HEIGHT, "Testing map rendering...", false);
	TCOD_mouse_show_cursor(false);

	initPlayer();

	for (i = 0; i < 15; i++){
		grass_color[i].r = 0;
		grass_color[i].g = TCOD_random_get_int(NULL, 70, 140);
		grass_color[i].b = 0;
	}

	while (!done) {
		drawMap(argv[1], &done, grass_color);
		drawHud();
		TCOD_console_put_char_ex(NULL, player_x, player_y, (int)player_sym, TCOD_light_blue, TCOD_black);

		key = TCOD_console_check_for_keypress(true);

		TCOD_console_flush();

		key = TCOD_console_wait_for_keypress(TCOD_KEY_PRESSED);

		if (key.vk == TCODK_ESCAPE)
			done = 1;
		if (key.vk == TCODK_UP && TCOD_console_get_char(NULL, player_x, player_y - 1) != WALL)
			player_y--;
		if (key.vk == TCODK_DOWN && TCOD_console_get_char(NULL, player_x, player_y + 1) != WALL)
			player_y++;
		if (key.vk == TCODK_LEFT && TCOD_console_get_char(NULL, player_x - 1, player_y) != WALL)
			player_x--;
		if (key.vk == TCODK_RIGHT && TCOD_console_get_char(NULL, player_x + 1, player_y) != WALL)
			player_x++;
		if (key.vk == TCODK_PRINTSCREEN || key.vk == TCODK_BACKSPACE)
			TCOD_sys_save_screenshot(NULL);
		if (key.c == 'h') {
			if (player_currentHP > 0)
				player_currentHP--;

			player_hpPct = floor(((float)player_currentHP / (float)player_maxHP) * 10);
		}
		if (key.c == 'H') {
			if (player_currentHP < player_maxHP)
				player_currentHP++;

			player_hpPct = floor(((float)player_currentHP / (float)player_maxHP) * 10);
		}
		if (key.c == 'm') {
			if (player_currentMP > 0)
				player_currentMP--;

			player_mpPct = floor(((float)player_currentMP / (float)player_maxMP) * 10);
		}
	}

	return 0;
}
