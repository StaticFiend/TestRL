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

map.c - Handles map related functions
*/

#include "../include/trl-main.h"

int countDoors(const char *filename) {
	int door_count = 0;
	char temp;

	FILE *map;

	map = fopen(filename, "r");

	if (map == NULL)
		return -1;

	while (!feof(map)) {
		temp = fgetc(map);

		if (temp == DOOR)
			door_count++;
	}

	return door_count;
}

void getMapFromFile(const char *filename, char *map, TCOD_color_t *map_colors, object_t *player, int *stair_x, int *stair_y, tile_t *door) {
	static int stair_found = 0, player_found = 0;
	FILE *mapFile;
	int i, y = 0, d = 0;

	mapFile = fopen(filename, "r");

	if (!mapFile) {
		printf("[Console] Unable to open file %s, please make sure this file exists and try again.\n", filename);
		return;
	}

	while (!feof(mapFile)) {
		for (i = 0; i < MAP_WIDTH * MAP_HEIGHT; i++) {
			map[i] = fgetc(mapFile);

			if (map[i] == -1 || map[i] == 10)
				map[i] = WALL;
			if ((i % 80) == 0 && y < 39 && i != 0)
				y++;

			switch((int)map[i]) {
				case WALL:
					if (player->dlvl > 0) {
						map_colors[i].r = 88;
						map_colors[i].g = 102;
						map_colors[i].b = 112;
					}
					else {
						map_colors[i].r = 127;
						map_colors[i].g = 127;
						map_colors[i].b = 127;
					}
					break;
				case LAVA:
					//map[i] = WATER; //Nice visual effect but harder to test tiles like this.
					map_colors[i].r = TCOD_random_get_int(NULL, 180, 207);
					map_colors[i].g = TCOD_random_get_int(NULL, 32, 128);
					map_colors[i].b = 16;
					break;
				case DOOR:
#ifdef DEBUG
					printf("[DEBUG] Door found at cell %i\n", i);
#endif

					door[d].index = i;
					door[d].x = i % 80;
					door[d].y = y + 5;

					d++;

					map_colors[i].r = 200;
					map_colors[i].g = 200;
					map_colors[i].b = 0;
					break;
				case OPEN_DOOR:
					map_colors[i].r = 200;
					map_colors[i].g = 200;
					map_colors[i].b = 0;
					break;
				case GROUND:
				case STAIRS:
					//Figured this should be necessary incase multiple stairs are added.
					if (stair_found == 1)
						map[i] = GROUND;
					if (map[i] == STAIRS && stair_found == 0) {
						*stair_x = i % 80;
						*stair_y = y;
						stair_found = 1;
					}

					map_colors[i].r = 255;
					map_colors[i].g = 255;
					map_colors[i].b = 255;
					break;
				case HERO:
					if (player_found == 0) {
						player->x = i % 80;
						player->y = y + 5;

						player_found = 1;
					}
						map[i] = GROUND;
						map_colors[i].r = 255;
						map_colors[i].g = 255;
						map_colors[i].b = 255;
					break;
				case TREE:
					map_colors[i].r = 40;
					map_colors[i].g = TCOD_random_get_int(NULL, 180, 230);
					map_colors[i].b = 40;
					break;
				case BLOOD:
					map[i] = GROUND;
					map_colors[i].r = TCOD_random_get_int(NULL, 130, 200);
					map_colors[i].g = 20;
					map_colors[i].b = 20;
					break;
				case GRASS:
					map_colors[i].r = 0;
					map_colors[i].g = TCOD_random_get_int(NULL, 100, 150);
					map_colors[i].b = 0;
					break;
				case WATER:
					map_colors[i].r = 0;
					map_colors[i].g = 60;
					map_colors[i].b = TCOD_random_get_int(NULL, 150, 250);
					break;
					}
				
			if (i == 79 && y == 39)
				break;
		}
	}

	fclose(mapFile);

	return;
}

void loadMapConfig(const char *filename, uint8_t *radius, uint8_t *fov_formula) {
	FILE *map_config;
	char temp[1];

	map_config = fopen(filename, "r");

	if (map_config == NULL) {
		printf("[Console] Unable to load map config file, using default values...\n");

		*radius = 0;
		*fov_formula = FOV_RESTRICTIVE;
		return;
	}

	temp[0] = fgetc(map_config);
	*radius = atoi(temp);
	if (*radius == 0)
		*fov_formula = FOV_RESTRICTIVE;
	else
		*fov_formula = FOV_SHADOW;

	fclose(map_config);
}

TCOD_map_t createTCODmap(char *map, uint8_t *discovered) {
	TCOD_map_t tc_map = TCOD_map_new(MAP_WIDTH, MAP_HEIGHT);

	int i, y = 0;

	TCOD_map_clear(tc_map);

	for (i = 0; i < MAP_WIDTH * MAP_HEIGHT; i++) {

		discovered[i] = 0;

		if ((i % 80) == 0 && y < 39 && i != 0)
			y++;

		//map, x, y, is_transparent, is_walkable (non-transparent things will block light)
		switch ((int)map[i]) {
			case 32:
			case DOOR:
			case WALL:
				TCOD_map_set_properties(tc_map, i % 80, y, false, false);
				break;
			case TREE:
				TCOD_map_set_properties(tc_map, i % 80, y, false, true);
				break;
			default:
				TCOD_map_set_properties(tc_map, i % 80, y, true, true);
				break;
		}

		if ((i % 80) == 79 && y == 39)
			break;
	}

	return tc_map;
}

//FIXME: Think about a way to make this more effecient maybe.
void open_door(int direction, object_t player, tile_t *door, TCOD_map_t *fov_map, char *map, int door_count) {
	int i;

	if (direction == UP) {
		TCOD_map_set_properties(*fov_map, player.x, player.y - 5 - 1, true, true);

		for (i = 0; i < door_count; i++) {
			if (door[i].x == player.x && door[i].y == player.y - 1)
				map[door[i].index] = OPEN_DOOR;
		}
	}
	else if (direction == RIGHT) {
		TCOD_map_set_properties(*fov_map, player.x + 1, player.y - 5, true, true);

		for (i = 0; i < door_count; i++) {
			if (door[i].x == player.x + 1 && door[i].y == player.y)
				map[door[i].index] = OPEN_DOOR;
		}
	}
	else if (direction == DOWN) {
		TCOD_map_set_properties(*fov_map, player.x, player.y - 5 + 1, true, true);

		for (i = 0; i < door_count; i++) {
			if (door[i].x == player.x && door[i].y == player.y + 1)
				map[door[i].index] = OPEN_DOOR;
		}
	}
	else if (direction == LEFT) {
		TCOD_map_set_properties(*fov_map, player.x - 1, player.y - 5, true, true);

		for (i = 0; i < door_count; i++) {
			if (door[i].x == player.x - 1 && door[i].y == player.y)
				map[door[i].index] = OPEN_DOOR;
		}
	}
}

void close_door(int direction, object_t player, tile_t *door, TCOD_map_t *fov_map, char *map, int door_count) {
	int i;

	if (direction == UP) {
		TCOD_map_set_properties(*fov_map, player.x, player.y - 5 - 1, false, false);

		for (i = 0; i < door_count; i++) {
			if (door[i].x == player.x && door[i].y == player.y - 1)
				map[door[i].index] = DOOR;
		}
	}
	else if (direction == RIGHT) {
		TCOD_map_set_properties(*fov_map, player.x + 1, player.y - 5, false, false);

		for (i = 0; i < door_count; i++) {
			if (door[i].x == player.x + 1 && door[i].y == player.y)
				map[door[i].index] = DOOR;
		}
	}
	else if (direction == DOWN) {
		TCOD_map_set_properties(*fov_map, player.x, player.y - 5 + 1, false, false);

		for (i = 0; i < door_count; i++) {
			if (door[i].x == player.x && door[i].y == player.y + 1)
				map[door[i].index] = DOOR;
		}
	}
	else if (direction == LEFT) {
		TCOD_map_set_properties(*fov_map, player.x - 1, player.y - 5, false, false);

		for (i = 0; i < door_count; i++) {
			if (door[i].x == player.x - 1 && door[i].y == player.y)
				map[door[i].index] = DOOR;
		}
	}
}

