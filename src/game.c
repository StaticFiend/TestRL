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

game.c - Main game loop and misc game related functions.
*/

#include <time.h>
#include "trl-main.h"
#include "trl-map.h"
#include "trl-cave.h"
#include "trl-draw.h"
#include "trl-monster.h"

//FIXME: Make this better (class system, stats, etc)
void initPlayer(object_t *player) {
	int i;

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

	for (i = 0; i < MAX_INVENTORY; i++) {
		player->inventory[i].type = EMPTY_SLOT;
	}
}

//TODO: Separate this into maybe item.c
void init_items(item_t items[MAX_RANDOM_ITEMS], int item_count, TCOD_map_t tcod_map) {
	int i, random_type;

	for (i = 0; i < item_count; i++) {
		do {
			items[i].x = TCOD_random_get_int(NULL, 0, MAP_WIDTH);
			items[i].y = TCOD_random_get_int(NULL, 0, MAP_HEIGHT);
		} while (TCOD_map_is_walkable(tcod_map, items[i].x, items[i].y) == false);
		//I've got bigger ideas for this, here's my plan:
		//	Make it so certain items appear more often than others
		//	Certain subtypes are more likely.
		//	Possibly more to come from this.
//		items[i].type = TCOD_random_get_int(NULL, 1, 6);
		random_type = TCOD_random_get_int(NULL, 1, 15);

		//These numbers may be tweaked at some point, since a 6% (nearly 7%) chance of a book seems low.
		if (random_type >= 1 && random_type <= 2)
			items[i].type = WEAPON;
		else if (random_type >= 3 && random_type <= 4)
			items[i].type = ARMOR;
		else if (random_type >= 5 && random_type <= 8)
			items[i].type = SCROLL;
		else if (random_type == 9)
			items[i].type = BOOK;
		else if (random_type >= 10 && random_type <= 13)
			items[i].type = POTION;
		else
			items[i].type = AMMO;

		items[i].discovered = false;
#ifdef DEBUG
		printf("[Debug] item #%i type: %i\n", i, items[i].type);
#endif
	}
}

//THIS WORKS!!!!!!!
void pickup_item(item_t *item, object_t *player) {
	int i, open_slot = 666;

	for (i = 0; i < MAX_INVENTORY; i++) {
		if (player->inventory[i].type == EMPTY_SLOT) {
			open_slot = i;
			break;
		}
	}

	if (open_slot == 666) {
		printf("[Console] No room in inventory.\n");
		TCOD_console_print_left(NULL, 0, 0, TCOD_BKGND_NONE, "Your pack is completely full!");
	}
	else {
		player->inventory[open_slot] = *item; //I believe this should be fine, since there's no pointers involved.
		item->x = 99;
	}
}

void game_loop(bool save_detected) {
	TCOD_key_t key;
	TCOD_map_t tcod_map = TCOD_map_new(MAP_WIDTH, MAP_HEIGHT);
	TCOD_color_t map_colors[MAP_HEIGHT][MAP_WIDTH];
	
	FILE *save_file;

	uint8_t discovered[MAP_HEIGHT][MAP_WIDTH];
	uint8_t fov_formula;
	uint8_t lit_cave;

	uint8_t loot_count;

	object_t player;
//	object_t *monster;
	tile_t *door;
	tile_t stairs;
	item_t random_loot[MAX_RANDOM_ITEMS];

	char map[MAP_HEIGHT][MAP_WIDTH];
//	char *saveChar;
	bool done = false;
	char map_file[256] = "", map_config[256] = "";

	int door_count = 0;
	int i;
	
	//	mon_num = TCOD_random_get_int(NULL, 0, MAX_MONSTERS);

//#ifdef DEBUG
//	printf("[DEBUG] mon_num is now set to : %i\n", mon_num);
//#endif

//	monster = malloc(sizeof(object_t) * mon_num);

	initPlayer(&player);

	if (save_detected == false) {
		strcat(map_file, "data/maps/start.map");
		strcat(map_config, "data/maps/start.cfg");
		door_count = count_doors(map_file);
		if (door_count == -1) {
			printf("[Console] FATAL ERROR: Unable to find specified map: %s\n", map_file);
			return;
		}
	}
	else {
		strcat(map_file, "data/maps/doom.map");
		strcat(map_config, "data/maps/doom.cfg");
		door_count = count_doors(map_file);
		if (door_count == -1) {
			printf("[Console] FATAL ERROR: Unable to find specified map: %s\n", map_file);
			return;
		}
	}

	loot_count = TCOD_random_get_int(NULL, 1, MAX_RANDOM_ITEMS);

#ifdef DEBUG
	printf("[Debug] Loot count: %i\n", loot_count);
#endif

	door = malloc(sizeof(tile_t) * door_count);
	load_map_from_file(map_file, map, map_colors, &player, &stairs, door);
	load_map_config(map_config, &player.radius, &fov_formula);
	tcod_map = create_tcod_map(map, discovered);

//	createMonster(monster, mon_num, tcod_map);
	init_items(random_loot, loot_count, tcod_map);

	while (done == false) {
		//player.y - 5 is necessary here because the drawn map is shifted 5 units down,
		//so we need to compensate for that.
		TCOD_map_compute_fov(tcod_map, player.x, player.y - 5, player.radius, true, fov_formula);

		draw_map(map, map_colors, tcod_map, discovered);
		draw_hud(player);
		//FIXME: This is kind of a hack to clear the top area, only an issue because there's no messaging system.
		TCOD_console_print_left(NULL, 0, 0, TCOD_BKGND_NONE, "                                    ");
//		draw_monster(monster, mon_num, tcod_map);
		draw_items(random_loot, loot_count, tcod_map);
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
					TCOD_BKGND_NONE, "using %i turn(s).", player.turns);
			TCOD_console_print_center(NULL, SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2 + 4,
					TCOD_BKGND_NONE, "Press space to quit.");
			TCOD_console_flush();

			while (dead_quit == false) {
				key = TCOD_console_wait_for_keypress(TCOD_KEY_PRESSED);

				if (key.vk == TCODK_SPACE)
					dead_quit = true;
			}
			//Possibly write an obituray file here, put it in data/obituary/DATE - TIME.txt
			char obitFileName[256];
//			FILE *obitFile;

			struct tm *cur_time;
			time_t temp_time;

			temp_time = time(NULL);
			cur_time = localtime(&temp_time);

			strftime(obitFileName, 256, "%Y%d%m_%I%M-obituary.txt", cur_time);
			printf("[Debug] Testing obituary filename: %s.\n", obitFileName);

			//Save a file here with said filename and information about the player, maybe the map and all that.
			//Otherwise this WORKS!!!

			break; //Get out of main game loop, and wrap up.
		}

		//Apr 13, 2011 - this is a big change, because this means the game no longer waits purely for a keypress
		//               and instead just keeps going, this is where a turn system needs to be implemented before
		//               I add monsters back.

		//Apr 14, 2011 - tried check_for_keypress for a while but it slows the game down considerably, I'll either
		//               need to figure out how to make the entire game much faster or simply just keep wait
		//               which works fine as is.
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
		//TODO: This area should be changed slightly, basically I want it to allow you to pick up what items
		//	you want if there are multiple on the same square.
		if (key.c == ',') {
			for (i = 0; i < loot_count; i++) {
				if ((player.y - 5 == random_loot[i].y) && (player.x == random_loot[i].x)) {
					pickup_item(&random_loot[i], &player);
					printf("[Console] Player picked up an item!\n");
					break;
				}
			}
		}
		if (key.c == 'i') {
			//Just doing a check to see if picking up items worked.
			for (i = 0; i < MAX_INVENTORY; i++) {
				printf("[Console] Inventory Slot %i item ID number %i\n", i + 1, player.inventory[i].type);
			}
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

			save_file = fopen("current_game", "w");
			fprintf(save_file, "<player_currentHP = %i>\n<player_currentMP = %i>\n<player_maxHP = %i>\n<player_maxMP = %i>\n<player_dlvl = %i>\n<special = %i>\n", player.currentHP, player.currentMP, player.maxHP, player.maxMP, player.dlvl, special);

			printf("[Console] Game saved, quitting...\n");
			
			TCOD_console_print_left(NULL, 0, 0, TCOD_BKGND_NONE, "Game saved, press any key to quit...");
			TCOD_console_flush();
			key = TCOD_console_wait_for_keypress(TCOD_KEY_PRESSED);

			fclose(save_file);
			TCOD_map_delete(tcod_map);

			return;
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

				//Add new items.
				loot_count = TCOD_random_get_int(NULL, 0, MAX_RANDOM_ITEMS);
				init_items(random_loot, loot_count, tcod_map);

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

		//Pressing the button enough will drain your torch, perhaps change this?
		if (player.torch_lit == true)
			player.torch_life--;
		if (player.torch_lit == true && player.torch_life == 0) {
			player.torch_lit = false;
			player.radius /= 2;
		}

//		ai_move(monster, mon_num, tcod_map, player); //Run all monster related events after the player has moved.
	}

	TCOD_map_delete(tcod_map);
}
