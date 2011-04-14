/*
TestRL
Copyright (c) 2011, Nick "Static_Fiend" Franklin
All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:
	* Redistributions of source code must retain the above copyright
	  notice, this list of conditions and the following disclaimer.
	* Redistributions in binary form must reproduce the above copyright
          notice, this list of conditions and the following disclaimer in the
          documentation and/or other materials provided with the distribution.
        * Neither the name of the Nick "Static_Fiend" Franklin nor the
          names of its contributors may be used to endorse or promote products
          derived from this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL Nick "Static_Fiend" Franklin BE LIABLE FOR ANY
DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
(INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

draw.c - Handles drawing related functions (using 2D Arrays)
*/

#include "trl-main.h"

//Implement this system similar to creating a map in map.c from a file, should be simple.
void animate_tile(char map[][MAP_WIDTH], TCOD_color_t colors[][MAP_WIDTH], const char tile) {
	int x, y;

	for (y = 0; y < MAP_HEIGHT; y++) {
		for (x = 0; x < MAP_WIDTH; x++) {
			if (map[y][x] == tile) {
				if (tile == WATER) {
					colors[y][x].b = TCOD_random_get_int(NULL, 150, 250);
				}
				else if (tile == LAVA) {
					colors[y][x].r = TCOD_random_get_int(NULL, 180, 200);
					colors[y][x].g = TCOD_random_get_int(NULL, 32, 128);
				}
			}
		}
	}
}

void draw_player(object_t player) {
	TCOD_color_t player_color;

	player_color.r = player.color[0];
	player_color.g = player.color[1];
	player_color.b = player.color[2];

	TCOD_console_put_char_ex(NULL, player.x, player.y, player.symbol, player_color, TCOD_black);
}

void draw_monster(object_t *monster, int num_of_mon, TCOD_map_t fov_map) {
	TCOD_color_t monster_color;

	int i;

	for (i = 0; i < num_of_mon; i++) {
		monster_color.r = monster[i].color[0];
		monster_color.g = monster[i].color[1];
		monster_color.b = monster[i].color[2];

		if (TCOD_map_is_in_fov(fov_map, monster[i].x, monster[i].y - 5) == true)
			TCOD_console_put_char_ex(NULL, monster[i].x, monster[i].y,
					monster[i].symbol, monster_color, TCOD_black);
	}
}

void draw_map(char map[][MAP_WIDTH], TCOD_color_t color_map[][MAP_WIDTH],
		TCOD_map_t fov_map, uint8_t discovered[][MAP_WIDTH]) {

	TCOD_color_t darken = {127, 127, 127};

	int x, y;

	for (y = 0; y < MAP_HEIGHT; y++) {
		for (x = 0; x < MAP_WIDTH; x++) {
			if (TCOD_map_is_in_fov(fov_map, x, y) == true && discovered[y][x] == 0) {
				discovered[y][x] = 1;
				TCOD_console_put_char_ex(NULL, x, y + 5, (int)map[y][x],
						color_map[y][x], TCOD_black);
				//Disabling these calls for the mean time, doesn't work quite how I want it to
//				if (map[y][x] == WATER)
//					animate_tile(map, color_map, WATER);
//				else if (map[y][x] == LAVA)
//					animate_tile(map, color_map, LAVA);
			}
			else if (TCOD_map_is_in_fov(fov_map, x, y) == true && discovered[y][x] == 1) {
				TCOD_console_put_char_ex(NULL, x, y + 5, (int)map[y][x],
						color_map[y][x], TCOD_black);
//				if (map[y][x] == WATER)
//					animate_tile(map, color_map, WATER);
//				else if (map[y][x] == LAVA)
//					animate_tile(map, color_map, LAVA);
			}
			else if (TCOD_map_is_in_fov(fov_map, x, y) == false && discovered[y][x] == 1) {
				TCOD_console_put_char_ex(NULL, x, y + 5, (int)map[y][x],
						TCOD_color_multiply(color_map[y][x], darken), TCOD_black);
			}
			else
				TCOD_console_put_char_ex(NULL, x, y + 5, BLANK, TCOD_black, TCOD_black);
		}
	}
}

void draw_hud(object_t player) {
        TCOD_color_t hud = {120, 120, 255};
        int i;

        TCOD_console_set_color_control(TCOD_COLCTRL_1, TCOD_dark_grey, TCOD_black);

	//Note: This is used just to color the players current HP depending on how much HP they have left.
	if (player.currentHP < (player.maxHP / 2))
		TCOD_console_set_color_control(TCOD_COLCTRL_2, TCOD_red, TCOD_black);
	else if (player.currentHP < (player.maxHP / 1.25))
		TCOD_console_set_color_control(TCOD_COLCTRL_2, TCOD_yellow, TCOD_black);
	else
		TCOD_console_set_color_control(TCOD_COLCTRL_2, TCOD_white, TCOD_black);

	//Note: This section looks a little weird now that I've changed things, but believe me, changing
	//      it over to a system where print_left prints the values separately from the HUD layout is better
	//      since it can size itself independently without breaking the layout.
	//TODO: Convert everything over to real stats instead of these static numbers.
	TCOD_console_print_left(NULL, 0, 46, TCOD_BKGND_NONE,
		"  \"The Debugger\"    HP: [%c----------%c]           Str: 4  Dex: 5  Con: 3",
	       	TCOD_COLCTRL_1, TCOD_COLCTRL_STOP);
	TCOD_console_print_left(NULL, 0, 47, TCOD_BKGND_NONE,
		"                    MP: [%c----------%c]           Int: 8  Wis: 7  Lck: 5",
	       	TCOD_COLCTRL_1, TCOD_COLCTRL_STOP);
	TCOD_console_print_left(NULL, 0, 48, TCOD_BKGND_NONE,
		"  Dlvl:             Turns:                     Lvl: 1          Exp: 1/4");

	TCOD_console_print_left(NULL, 37, 46, TCOD_BKGND_NONE, "%c%i%c/%i",
		TCOD_COLCTRL_2, player.currentHP, TCOD_COLCTRL_STOP, player.maxHP);
	TCOD_console_print_left(NULL, 37, 47, TCOD_BKGND_NONE, "%i/%i", player.currentMP, player.maxMP);
	TCOD_console_print_left(NULL, 8, 48, TCOD_BKGND_NONE, "%i", player.dlvl);
	TCOD_console_print_left(NULL, 27, 48, TCOD_BKGND_NONE, "%i", player.turns);
	
	for (i = 25; i < 25 + player.hpPct; i++) {
		TCOD_console_put_char_ex(NULL, i, 46, 178, TCOD_light_red, TCOD_black);
	}
	for (i = 25; i < 25 + player.mpPct; i++) {
		TCOD_console_put_char_ex(NULL, i, 47, 178, TCOD_light_blue, TCOD_black);
	}

	//Set up borders, this is done here because of drawing order.
	for (i = 0; i < SCREEN_WIDTH; i++) {
		if (i != 0 && i != SCREEN_WIDTH - 1) {
			TCOD_console_put_char_ex(NULL, i, 45, TCOD_CHAR_HLINE, hud, TCOD_black);
			TCOD_console_put_char_ex(NULL, i, 49, TCOD_CHAR_HLINE, hud, TCOD_black);
		}
                else if (i == 0) {
			TCOD_console_put_char_ex(NULL, i, 45, TCOD_CHAR_NW, hud, TCOD_black);
			TCOD_console_put_char_ex(NULL, i, 49, TCOD_CHAR_SW, hud, TCOD_black);

			TCOD_console_put_char_ex(NULL, i, 46, TCOD_CHAR_VLINE, hud, TCOD_black);
			TCOD_console_put_char_ex(NULL, i, 47, TCOD_CHAR_VLINE, hud, TCOD_black);
			TCOD_console_put_char_ex(NULL, i, 48, TCOD_CHAR_VLINE, hud, TCOD_black);
		}
		else if (i == SCREEN_WIDTH - 1){
			TCOD_console_put_char_ex(NULL, i, 45, TCOD_CHAR_NE, hud, TCOD_black);
			TCOD_console_put_char_ex(NULL, i, 49, TCOD_CHAR_SE, hud, TCOD_black);

			TCOD_console_put_char_ex(NULL, i, 46, TCOD_CHAR_VLINE, hud, TCOD_black);
			TCOD_console_put_char_ex(NULL, i, 47, TCOD_CHAR_VLINE, hud, TCOD_black);
			TCOD_console_put_char_ex(NULL, i, 48, TCOD_CHAR_VLINE, hud, TCOD_black);
		}
	}
}

void draw_help() {
	TCOD_console_print_frame(NULL, 2, 6, 45, 18, true, TCOD_BKGND_NONE, "Help");
	TCOD_console_print_left(NULL, 3, 7, TCOD_BKGND_NONE, "TestRL Basic controls:");
	TCOD_console_print_left(NULL, 3, 8, TCOD_BKGND_NONE, "Saving can only be done on downstairs.");

	TCOD_console_print_left(NULL, 3, 10, TCOD_BKGND_NONE, "UP/DOWN/LEFT/RIGHT - Move player.");
	TCOD_console_print_left(NULL, 3, 11, TCOD_BKGND_NONE, "g                  - lights torch.");
	TCOD_console_print_left(NULL, 3, 12, TCOD_BKGND_NONE, "i                  - displays inventory.");
	TCOD_console_print_left(NULL, 3, 13, TCOD_BKGND_NONE, "q                  - quaff potion.");
	TCOD_console_print_left(NULL, 3, 14, TCOD_BKGND_NONE, "r                  - read scroll/book.");
	TCOD_console_print_left(NULL, 3, 15, TCOD_BKGND_NONE, "u                  - unequip armor.");
	TCOD_console_print_left(NULL, 3, 16, TCOD_BKGND_NONE, "w                  - wear armor.");
	TCOD_console_print_left(NULL, 3, 18, TCOD_BKGND_NONE, "S                  - saves game.");
	TCOD_console_print_left(NULL, 3, 19, TCOD_BKGND_NONE, "W                  - wield weapon.");
	TCOD_console_print_left(NULL, 3, 21, TCOD_BKGND_NONE, ">                  - go down stairs.");
	TCOD_console_print_left(NULL, 3, 22, TCOD_BKGND_NONE, "?                  - displays this screen.");
	TCOD_console_flush();
}

void draw_items(item_t loot[MAX_RANDOM_ITEMS], int loot_count, TCOD_map_t tcod_map) {
	TCOD_color_t darken = {127, 127, 127};

	int i;

	for (i = 0; i < loot_count; i++) {
		if (loot[i].x == 99)
			continue;
		else if (TCOD_map_is_in_fov(tcod_map, loot[i].x, loot[i].y) == true) {
			loot[i].discovered = true;

			switch (loot[i].type) {
				case WEAPON:
					TCOD_console_put_char_ex(NULL, loot[i].x, loot[i].y + 5, 40,
							TCOD_light_blue, TCOD_black);
					break;
				case ARMOR:
					TCOD_console_put_char_ex(NULL, loot[i].x, loot[i].y + 5, 91,
							TCOD_dark_yellow, TCOD_black);
					break;
				case SCROLL:
					TCOD_console_put_char_ex(NULL, loot[i].x, loot[i].y + 5, 63,
							TCOD_white, TCOD_black);
					break;
				case BOOK:
					TCOD_console_put_char_ex(NULL, loot[i].x, loot[i].y + 5, 43,
							TCOD_desaturated_green, TCOD_black);
					break;
				case POTION:
					TCOD_console_put_char_ex(NULL, loot[i].x, loot[i].y + 5, 33,
							TCOD_desaturated_blue, TCOD_black);
					break;
				case AMMO:
					TCOD_console_put_char_ex(NULL, loot[i].x, loot[i].y + 5, 42,
							TCOD_white, TCOD_black);
					break;
				default:
					//???????
					printf("[Console] ERROR: Unknown item type, what the hell did you do?!?!?!\n");
					break;
			}
		}
		else if (TCOD_map_is_in_fov(tcod_map, loot[i].x, loot[i].y) == false && loot[i].discovered == true) {
			switch (loot[i].type) {
				case WEAPON:
					TCOD_console_put_char_ex(NULL, loot[i].x, loot[i].y + 5, 40,
							TCOD_color_multiply(TCOD_light_blue, darken), TCOD_black);
					break;
				case ARMOR:
					TCOD_console_put_char_ex(NULL, loot[i].x, loot[i].y + 5, 91,
							TCOD_color_multiply(TCOD_dark_yellow, darken), TCOD_black);
					break;
				case SCROLL:
					TCOD_console_put_char_ex(NULL, loot[i].x, loot[i].y + 5, 63,
							TCOD_color_multiply(TCOD_white, darken), TCOD_black);
					break;
				case BOOK:
					TCOD_console_put_char_ex(NULL, loot[i].x, loot[i].y + 5, 43,
							TCOD_color_multiply(TCOD_desaturated_green, darken), TCOD_black);
					break;
				case POTION:
					TCOD_console_put_char_ex(NULL, loot[i].x, loot[i].y + 5, 33,
							TCOD_color_multiply(TCOD_desaturated_blue, darken), TCOD_black);
					break;
				case AMMO:
					TCOD_console_put_char_ex(NULL, loot[i].x, loot[i].y + 5, 42,
							TCOD_color_multiply(TCOD_white, darken), TCOD_black);
					break;
				default:
					//???????
					printf("[Console] ERROR: Unknown item type, what the hell did you do?!?!?!\n");
					break;
			}
		}

	}
}
