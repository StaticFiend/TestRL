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

draw.c - Handles all the drawing routines
*/

#include "../include/trl-main.h"
//From QuakeRL, maybe consider changing this.
/*
void hud_message(const char *text, TCOD_console_t text_con) {
	static int current_line = 0;

	if (current_line > 4) {
		TCOD_console_t temp = TCOD_console_new(SCREEN_WIDTH, 4);

		TCOD_console_blit(text_con, 0, 1, SCREEN_WIDTH, 5, temp, 0, 0, 1, 1);
		TCOD_console_clear(text_con);
		TCOD_console_blit(temp, 0, 0, SCREEN_WIDTH, 4, text_con, 0, 0, 1, 1);

		current_line = 4;
	}

	TCOD_console_print_left(text_con, 0, currentLine, TCOD_BKGND_NONE, text);

	current_line++;
}		
*/

//Use me later when the map system is switched to a 2D array.
void fill_map(char map[MAP_HEIGHT][MAP_WIDTH], const char fill) {
	int x, y;

	for (y = 0; y < MAP_HEIGHT; y++) {
		for (x = 0; x < MAP_WIDTH; x++) {
			map[y][x] = fill;
		}
	}
}

void drawPlayer(object_t player) {
	TCOD_color_t player_color;

	player_color.r = player.color[0];
	player_color.g = player.color[1];
	player_color.b = player.color[2];

	TCOD_console_put_char_ex(NULL, player.x, player.y, player.symbol, player_color, TCOD_black);
}

void drawMonsters(object_t *monster, int mon_num, TCOD_map_t fov_map) {
	TCOD_color_t monster_color;

	int i;

	for (i = 0; i < mon_num; i++) {
		monster_color.r = monster[i].color[0];
		monster_color.g = monster[i].color[1];
		monster_color.b = monster[i].color[2];

		if (TCOD_map_is_in_fov(fov_map, monster[i].x, monster[i].y - 5) == true)
			TCOD_console_put_char_ex(NULL, monster[i].x, monster[i].y, monster[i].symbol, monster_color, TCOD_black);
	}
}

//FIXME: Function is safe guarded against passing an array_size that's impossible, but
//FIXME: think of a way to eliminate the array_size altogether and have it dynamically
//FIXME: figure this out.
void drawMonster(object_t monster[], int array_size, TCOD_map_t map) {
	TCOD_color_t monster_color;
	int i;

	if (array_size < 1) {
		printf("[Console] Error: array_size passted to drawMonster is less than 1.\n");
		return;
	}
	else {
		for (i = 0; i < array_size; i++) {
			if (TCOD_map_is_in_fov(map, monster[i].x, monster[i].y) == true) {
				monster_color.r = monster[i].color[0];
				monster_color.g = monster[i].color[1];
				monster_color.b = monster[i].color[2];

				TCOD_console_put_char_ex(NULL, monster[i].x, monster[i].y, monster[i].symbol, monster_color, TCOD_black);
			}
		}
	}		
}

//FIXME: Clean this function up, it's kind of getting a bit out of hand.
//FIXME: Actually this function might be totally fine as far as cleanliness is concerned.
void drawMap(char map[MAP_WIDTH * MAP_HEIGHT], TCOD_color_t map_colors[MAP_WIDTH * MAP_HEIGHT], TCOD_map_t fov_map, uint8_t *discovered) {
	TCOD_color_t darken = {127, 127, 127};
	TCOD_color_t light = {255, 249, 239};

	int i, y = 0;

	TCOD_console_clear(NULL);

	for (i = 0; i < MAP_WIDTH * MAP_HEIGHT; i++) {
		if ((i % 80) == 0 && y < 39 && i != 0)
			y++;
	
		if (TCOD_map_is_in_fov(fov_map, i % 80, y) == true && discovered[i] == 0) {
			TCOD_console_put_char_ex(NULL, i % 80, y + 5, map[i], TCOD_color_multiply(map_colors[i], light), TCOD_black);
			discovered[i] = 1;
		}
		else if (TCOD_map_is_in_fov(fov_map, i % 80, y) == true && discovered[i] == 1)
			TCOD_console_put_char_ex(NULL, i % 80, y + 5, map[i], TCOD_color_multiply(map_colors[i], light), TCOD_black);
		else if (TCOD_map_is_in_fov(fov_map, i % 80, y) == false && discovered[i] == 1)
			TCOD_console_put_char_ex(NULL, i % 80, y + 5, map[i], TCOD_color_multiply(map_colors[i], darken), TCOD_black);
		else
			TCOD_console_put_char_ex(NULL, i % 80, y + 5, 32, TCOD_black, TCOD_black);

		if ((i % 80) == 79 && y == 39)
			break;
	}
}

void drawHud(object_t player) {
	TCOD_color_t hud = {120, 120, 255};
	int i;

	TCOD_console_set_color_control(TCOD_COLCTRL_1, TCOD_dark_grey, TCOD_black);
	
	if (player.currentHP < (player.maxHP / 2))
		TCOD_console_set_color_control(TCOD_COLCTRL_2, TCOD_red, TCOD_black);
	else if (player.currentHP < (player.maxHP / 1.25))
		TCOD_console_set_color_control(TCOD_COLCTRL_2, TCOD_yellow, TCOD_black);
	else
		TCOD_console_set_color_control(TCOD_COLCTRL_2, TCOD_white, TCOD_black);

	TCOD_console_print_left(NULL, 0, 46, TCOD_BKGND_NONE, "  \"The Debugger\"    HP: [%c----------%c] %c%i%c/%i     Str: 4  Dex: 5  Con: 3", TCOD_COLCTRL_1, TCOD_COLCTRL_STOP, TCOD_COLCTRL_2, player.currentHP, TCOD_COLCTRL_STOP, player.maxHP);
	TCOD_console_print_left(NULL, 0, 47, TCOD_BKGND_NONE, "                    MP: [%c----------%c] %i/%i     Int: 8  Wis: 7  Lck: 5", TCOD_COLCTRL_1, TCOD_COLCTRL_STOP, player.currentMP, player.maxMP);
	TCOD_console_print_left(NULL, 0, 48, TCOD_BKGND_NONE, "  Dlvl: %i                                      Lvl: 1          Exp: 1/4", player.dlvl);
	for (i = 25; i < 25 + player.hpPct; i++) {
		TCOD_console_put_char_ex(NULL, i, 46, 178, TCOD_light_red, TCOD_black);
	}
	for (i = 25; i < 25 + player.mpPct; i++) {
		TCOD_console_put_char_ex(NULL, i, 47, 178, TCOD_light_blue, TCOD_black);
	}
	//Set up borders, this is done here because of drawing order.
	for (i = 0; i < SCREEN_WIDTH; i++) {
		if (i != 0 && i != SCREEN_WIDTH - 1){
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

