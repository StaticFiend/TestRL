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

Main.c - main function and some other generalized stuff
*/

//Implementation of the new map system from the psuedo code file.

/*
TODO:
* Try implementing a save/load system.
IDEAS:
	* Save should save all current player data except x/y.

* Try implementing a monster, doesn't have to do anything in particular, just wander around or decide to stand still.
*/

#include "trl-main.h"
#include "trl-map.h"
#include "trl-cave.h"
#include "trl-draw.h"
#include "trl-monster.h"

//FIXME: Rewrite this to use arrays for object_t
/*void mon_AI(object_t *monster, TCOD_map_t map) {
	int direction;

	direction = TCOD_random_get_int(NULL, 1, 5);

	if (monster->currentHP == 0)
		return;

	switch(direction) {
		case 1: //up
			if (TCOD_map_is_walkable(map, monster->x, (monster->y - 5) - 1))
				monster->y--;
			break;
		case 2: //right
			if (TCOD_map_is_walkable(map, monster->x + 1, monster->y - 5))
				monster->x++;
			break;
		case 3: //down
			if (TCOD_map_is_walkable(map, monster->x, (monster->y - 5) + 1))
				monster->y++;
			break;
		case 4: //left
			if (TCOD_map_is_walkable(map, monster->x - 1, monster->y - 5))
				monster->x--;
			break;
		default: //stand still
			break;
	}
}*/

//Algebra formula, distance between 2 points.
int distance(int x1, int y1, int x2, int y2) {
	return sqrt(pow(x2 - x1, 2) + pow(y2 - y1, 2));
}
/*
int trl_random(int low, int high) {
	static TCOD_random_t rnd = TCOD_random_new_from_seed(TCOD_RNG_CMWC, seed);

	return TCOD_random_get_int(rnd, low, high);
}
*/
//FIXME: Make this better (class system, stats, etc)
void initPlayer(object_t *player) {
	player->symbol = 64;
	player->x = 0;
	player->y = 0;

	player->maxHP = 25;
	player->maxMP = 50;

	player->currentHP = 25;
	player->currentMP = 50;

	player->hpPct = ((float)player->currentHP / (float)player->maxHP) * 10;
	player->mpPct = ((float)player->currentMP / (float)player->maxMP) * 10;

	player->dlvl = 1; //This signifies start.map

	player->color[0] = 150;
	player->color[1] = 150;
	player->color[2] = 255;
	
	player->torch_lit = false;
	player->turns = 0;
	player->torch_life = 80;
}

//FIXME: Make this better.
void createMonster(object_t *monster, int mon_num, TCOD_map_t map) {
//	monster->x = ;
//	monster->y = 20;
	int i;

	//printf("[DEBUG] Called createMonster...\n");

	for (i = 0; i < mon_num; i++) {
		//printf("[DEBUG] In for loop...\n");
		do {
			monster[i].x = TCOD_random_get_int(NULL, 0, 79);
			monster[i].y = TCOD_random_get_int(NULL, 0, 39);
		} while (TCOD_map_is_walkable(map, monster[i].x, monster[i].y) == false);

		if (TCOD_map_is_walkable(map, monster[i].x, monster[i].y) == false) {
			printf("[Console] Monster spawned out of bounds.\n");
		}

		monster[i].y += 5;

#ifdef DEBUG
		printf("[Console] Current monster x: %i   y: %i\n", monster[i].x, monster[i].y);
#endif

		monster[i].maxHP = 30;
		monster[i].currentHP = 30;

		monster[i].color[0] = 200;
		monster[i].color[1] = 20;
		monster[i].color[2] = 20;

		monster[i].state = STATE_NON_AGGRESSIVE;
		monster[i].symbol = 107;
	}
	//printf("[DEBUG] Exiting function...\n");
}

//FIXME: Might need to implement this at some point for better string parsing.
/*
void inspectString (char *test) {
	//Take a string, analyze it, 
}
*/

//TODO: Move all of this code into its own file labelled game.c
//TODO: Also make a title screen and all that good stuff.
int main(int argc, char **argv) {
	TCOD_key_t key;
	TCOD_map_t tcod_map = TCOD_map_new(MAP_WIDTH, MAP_HEIGHT);
	TCOD_color_t map_colors[MAP_HEIGHT][MAP_WIDTH];

	FILE *saveFile;

	uint8_t discovered[MAP_HEIGHT][MAP_WIDTH];
	uint8_t fov_formula;
	uint8_t lit_cave;

	object_t player;
//	object_t *monster;
	tile_t *door;
	tile_t stairs;

	char map[MAP_HEIGHT][MAP_WIDTH];
	char map_file[256] = "", map_config[256] = "";
	char *saveChar;
	bool done = false;

	//TODO: stair_x and stair_y should be replaced entirely by the tile_t stairs;
	int size, door_count = 0;
//	int mon_num = 0;

	TCOD_console_set_custom_font("data/font/testrl-font1.png", TCOD_FONT_LAYOUT_TCOD|TCOD_FONT_TYPE_GREYSCALE, 0, 0);
	TCOD_console_init_root(SCREEN_WIDTH, SCREEN_HEIGHT, "Map System Testing for TestRL", false);

//	mon_num = TCOD_random_get_int(NULL, 0, MAX_MONSTERS);

#ifdef DEBUG
	printf("[DEBUG] mon_num is now set to : %i\n", mon_num);
#endif

//	monster = malloc(sizeof(object_t) * mon_num);

	initPlayer(&player);

	if (argc > 1) {
		strcat(map_file, "./data/maps/");
		strcat(map_file, argv[1]);
		strcat(map_file, ".map\0");
		strcat(map_config, ",/data/maps/");
		strcat(map_config, argv[1]);
		strcat(map_config, ".cfg\0");
	}
	else {
		strcat(map_file, "data/maps/start.map");
		strcat(map_config, "data/maps/start.cfg");
	}

#ifdef DEBUG
	printf("[DEBUG] Testing a couple of variable sizes: %li bytes map_file, %li bytes map_config.\n", sizeof(map_file), sizeof(map_config));
#endif

	door_count = count_doors(map_file);
	if (door_count == -1) {
		printf("[Console] FATAL ERROR: Unable to find specified map: %s\n", map_file);
		return 0;
	}

	door = malloc(sizeof(tile_t) * door_count);
	load_map_from_file(map_file, map, map_colors, &player, &stairs, door);
	load_map_config(map_config, &player.radius, &fov_formula);
	tcod_map = create_tcod_map(map, discovered);
//	createMonster(monster, mon_num, tcod_map);

	saveFile = fopen("current_game", "r");

	if (saveFile != NULL) {
		printf("[Console] Saved data found, continuing game...\n");
		//do read stuff, tokenize, etc
		fseek(saveFile, 0, SEEK_END);
		size = ftell(saveFile);
		fseek(saveFile, 0, SEEK_SET);

#ifdef DEBUG
		printf("[DEBUG] size: %i\n", size);
#endif
		saveChar = (char *)malloc((sizeof(char) * size) + 1);

		fread(saveChar, 1, size, saveFile);
#ifdef DEBUG
		printf("[DEBUG] saveChar: %s\n", saveChar);
#endif

		//tokenized stuff beep boop beep
		//FIXME: Figure out how to load this stuff properly, strtok wants everything
		//FIXME: in an array rather than how it's laid out already.
		fclose(saveFile);
		free(saveChar);
		printf("[Console] Deleting saved game...\n");
		remove("current_game");
	}

	while (done == false) {
		//player.y - 5 is necessary here because the drawn map is shifted 5 units down,
		//so we need to compensate for that.
		TCOD_map_compute_fov(tcod_map, player.x, player.y - 5, player.radius, true, fov_formula);

		draw_map(map, map_colors, tcod_map, discovered);
		draw_hud(player);
		//Kind of a temporary bandaid because there's no message system, thus "open in what direction?"
		//sticks on the screen at all times.
		TCOD_console_print_left(NULL, 0, 0, TCOD_BKGND_NONE, "                                    ");
//		draw_monster(monster, mon_num, tcod_map);
		draw_player(player);

		TCOD_console_flush();

		if (player.currentHP == 0) {
			int fade;
			bool dead_quit = false;

			for (fade = 255; fade >= 0; fade--) {
				TCOD_console_set_fade(fade, TCOD_darker_red);
				TCOD_console_flush();
			}

			TCOD_console_set_background_color(NULL, TCOD_darker_red);
			TCOD_console_clear(NULL);
			TCOD_console_set_fade(255, TCOD_darker_red);

			TCOD_console_print_center(NULL, SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2, 
					TCOD_BKGND_NONE, "You have died.");
			TCOD_console_print_center(NULL, SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2 + 1,
					TCOD_BKGND_NONE, "Survived until dungeon level %i", player.dlvl);
			TCOD_console_print_center(NULL, SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2 + 2,
					TCOD_BKGND_NONE, "in %i turn(s).", player.turns);
			TCOD_console_print_center(NULL, SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2 + 4,
					TCOD_BKGND_NONE, "Press space to quit.");
			TCOD_console_flush();

			while (dead_quit == false) {
				key = TCOD_console_wait_for_keypress(TCOD_KEY_PRESSED);

				if (key.vk == TCODK_SPACE)
					dead_quit = true;
			}
			//Possibly write an obituray file here, put it in data/obituary/ __DATE__ __TIME__ .txt
			break; //Get out of main game loop, and wrap up.
		}

		key = TCOD_console_wait_for_keypress(TCOD_KEY_PRESSED);

		if (key.vk == TCODK_ESCAPE)
			break;
		//Using TCOD_map_is_walkable here just because we have this information
		//stored, might as well use it.  It also makes it so that we don't have to check individual
		//tiles for their code, we just know if they're walkable or not.
		//FIXME: This all needs to be modified for combat purposes!
		//TODO: Implement a proper turn system, right now this is just mostly fluff and is only on certain
		//      actions.
		if (key.vk == TCODK_UP && TCOD_map_is_walkable(tcod_map, player.x, player.y - 6)) {
			player.y--;
			player.turns++;
		}
		if (key.vk == TCODK_DOWN && TCOD_map_is_walkable(tcod_map, player.x, player.y - 4)) {
			player.y++;
			player.turns++;
		}
		if (key.vk == TCODK_LEFT && TCOD_map_is_walkable(tcod_map, player.x - 1, player.y - 5)) {
			player.x--;
			player.turns++;
		}
		if (key.vk == TCODK_RIGHT && TCOD_map_is_walkable(tcod_map, player.x + 1, player.y - 5)) {
			player.x++;
			player.turns++;
		}
		if (key.c == 'h' && player.currentHP > 0) {
			player.currentHP--;
			player.hpPct = ((float)player.currentHP / (float)player.maxHP) * 10;
		}
		//Door opening code!
		//TODO: Need to add a proper message system at some point, that way this won't stick on the screen.
		if (key.c == 'o') {
			TCOD_console_print_left(NULL, 0, 0, TCOD_BKGND_NONE, "Open in what direction?");
			TCOD_console_flush();

			key = TCOD_console_wait_for_keypress(TCOD_KEY_PRESSED);

			if (key.vk == TCODK_UP && TCOD_console_get_char(NULL, player.x, player.y - 1) == DOOR)
				use_door(UP, player, door, &tcod_map, map, door_count, 1);
			if (key.vk == TCODK_DOWN && TCOD_console_get_char(NULL, player.x, player.y + 1) == DOOR)
				use_door(DOWN, player, door, &tcod_map, map, door_count, 1);
			if (key.vk == TCODK_RIGHT && TCOD_console_get_char(NULL, player.x +1, player.y) == DOOR)
				use_door(RIGHT, player, door, &tcod_map, map, door_count, 1);
			if (key.vk == TCODK_LEFT && TCOD_console_get_char(NULL, player.x - 1, player.y) == DOOR)
				use_door(LEFT, player, door, &tcod_map, map, door_count, 1);
			
			player.turns++;
		}
		if (key.c == 'c') {
			TCOD_console_print_left(NULL, 0, 0, TCOD_BKGND_NONE, "Close in what direction?");
			TCOD_console_flush();

			key = TCOD_console_wait_for_keypress(TCOD_KEY_PRESSED);

			if (key.vk == TCODK_UP && TCOD_console_get_char(NULL, player.x, player.y - 1) == OPEN_DOOR)
				use_door(UP, player, door, &tcod_map, map, door_count, 0);
			if (key.vk == TCODK_DOWN && TCOD_console_get_char(NULL, player.x, player.y + 1) == OPEN_DOOR)
				use_door(DOWN, player, door, &tcod_map, map, door_count, 0);
			if (key.vk == TCODK_RIGHT && TCOD_console_get_char(NULL, player.x + 1, player.y) == OPEN_DOOR)
				use_door(RIGHT, player, door, &tcod_map, map, door_count, 0);
			if (key.vk == TCODK_LEFT && TCOD_console_get_char(NULL, player.x - 1, player.y) == OPEN_DOOR)
				use_door(LEFT, player, door, &tcod_map, map, door_count, 0);
			
			player.turns++;
		}
		if (key.vk == TCODK_BACKSPACE)
			TCOD_sys_save_screenshot(NULL);
		//TODO: Do this later, full on help system ingame.
		if (key.c == '?') {
			draw_help();
			key = TCOD_console_wait_for_keypress(TCOD_KEY_PRESSED);
		}

		//Change this to DoomRL style saving.  You can only save on downstairs, and it will auto-descend.
		//This way we don't have to store as much information, we can ignore saving discovered[] and just save
		//player data, and if the next map will be special (ie: from file) or not.
		if (key.c == 'S' && distance(player.x, player.y, stairs.x, stairs.y + 5) == 0) {
			//Increment dlvl, keep current inventory/spells/hp/mp, don't save x/y, run random check for special
			//(dlvl * 2.5)% of next level being special
			int special = TCOD_random_get_int(NULL, 1, 100);

#ifdef DEBUG
			printf("[Console] Testing the current value of \"special\": %i\n", special);
#endif

			if (special <= (player.dlvl * 2.5)) //on start.map (or any new game) this will never occur
				special = 1;
			else
				special = 0;

			player.dlvl++;
			player.turns++;

			saveFile = fopen("current_game", "w");
			fprintf(saveFile, "<player_currentHP = %i>\n<player_currentMP = %i>\n<player_maxHP = %i>\n<player_maxMP = %i>\n<player_dlvl = %i>\n<special = %i>\n", player.currentHP, player.currentMP, player.maxHP, player.maxMP, player.dlvl, special);

			printf("[Console] Game saved, quitting...\n");
			
			TCOD_console_print_left(NULL, 0, 0, TCOD_BKGND_NONE, "Game saved, press any key to quit...");
			TCOD_console_flush();
			key = TCOD_console_wait_for_keypress(TCOD_KEY_PRESSED);

			fclose(saveFile);
			TCOD_map_delete(tcod_map);

			return 0;
		}
		if (key.c == 'g' && player.radius > 0 && player.torch_life > 0) { //Torches!
			player.torch_lit = !player.torch_lit;

			if (player.torch_lit == true)
				player.radius *= 2;
			else if (player.torch_lit == false)
				player.radius /= 2;
		}
		if (key.c == '>' && distance(player.x, player.y, stairs.x, stairs.y + 5) == 0) {
				player.dlvl++;
				//There will be 2 random number checks when descending stairs, the first random number
				//will determine if the next floor is going to be special (ie: loaded froma file), or
				//if it'll be generated.  If it's to be generated, then another number will be randomized
				//to determine whether this should be a cave or a dungeon.
				//Perhaps caves should be higher chance the lower you go, while dungeon is higher if you're
				//just starting.
				generate_cave(map, map_colors, &player, &stairs);
				tcod_map = create_tcod_map(map, discovered);

				//Maybe think of different odds, perhaps base this off of dungeon depth, lower
				//you go the higher the chance of a dark floor.
				//idea for formula: (dlvl * 1.5) + 1 - upper limit
				//if lit_cave < (dlvl / 1.5)
				lit_cave = TCOD_random_get_int(NULL, 1, 3);
				player.turns++;

				if (lit_cave == 1) {
					player.radius = 0;
					fov_formula = FOV_RESTRICTIVE;
					player.torch_lit = false;
				}
				else {
					if (player.torch_lit == true)
						player.radius = 10;
					else
						player.radius = 5;

					fov_formula = FOV_SHADOW;
				}
		}

		if (player.torch_lit == true)
			player.torch_life--;
		if (player.torch_lit == true && player.torch_life == 0) {
			player.torch_lit = false;
			player.radius /= 2;
		}

//		ai_move(monster, mon_num, tcod_map, player); //Run all monster related events after the player has moved.
	}

	TCOD_map_delete(tcod_map);

	return 0;
}
