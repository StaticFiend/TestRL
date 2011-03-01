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

2d-map.c - Handles map related functions (using a 2D Array)
*/

#include "../include/trl-main.h"

/*
 * void load_map_from_file()

 * Loads a .map file for use as a special level.

 * Parameters:
 * filename - the file to load
 * map[][] - the map array we use for all the normal characters
 * map_colors[][] - the array of colors used on each tile
 * *player - the player object, we use a pointer here because we want to set the x/y when found
 * *stair_x, *stair_y - this may eventually be moved to a struct, but we want to know the x/y of the stairs
 * *door - the tile struct for doors, we want to know their positions as well

 * Function works by simply reading in the file using standard C fopen, then we move into reading the
 * .map using fgetc and checking what this character is via a switch, this way we can color tiles
 * appropriately and accurately.  If this tile happens to be something we don't expect we can just
 * simply make it into a wall or whatever ever tile.  If the tile in question when read from the .map
 * data is not respresented by the same symbol in the game, then it is changed here as well.
*/
void load_map_from_file(const char *filename, char map[][MAP_WIDTH],
	       	TCOD_color_t map_colors[][MAP_WIDTH], object_t *player, int *stair_x, int *stair_y,
		tile_t *door) {

	static int stairs_found = 0, player_found = 0;
	FILE *map_file;
	int i = 0, x = 0, y = 0;
	char temp;

	map_file = fopen(filename, "r");

	if (map_file == NULL) {
		printf("[Console] FATAL ERROR: Could not load map %s, please verify install.\n", filename);
		return;
	}

	while (!feof(map_file)) {
		temp = fgetc(map_file);

		if (temp == '\n' && x == 79) {
			y++;
			x = 0;
			continue;
		}
		else if (temp == '\n' && y == 39)
			break;
		else {
			if (temp < 0)
				temp = WALL;

			map[y][x] = temp;
			switch((int)map[y][x]) {
				case WALL:
					map_colors[y][x].r = 127;
					map_colors[y][x].g = 127;
					map_colors[y][x].b = 127;
					break;
				case LAVA:
					//*map[y][x] = WATER; //Use water tiles for lava.
					map_colors[y][x].r = TCOD_random_get_int(NULL, 180, 200);
					map_colors[y][x].g = TCOD_random_get_int(NULL, 32, 128);
					map_colors[y][x].b = 16;
					break;
				case DOOR:
					door[i].x = x;
					door[i].y = y;

					printf("Door %i at (%i, %i)\n", i, x, y);

					i++;

					map_colors[y][x].r = 200;
					map_colors[y][x].g = 200;
					map_colors[y][x].b = 0;
					break;
				case GROUND:
				case STAIRS:
					if (stairs_found == 1 && map[y][x] == STAIRS)
						map[y][x] = GROUND;
					else if (stairs_found == 0 && map[y][x] == STAIRS) {
						*stair_x = x;
						*stair_y = y;
						stairs_found = 1;
					}

					map_colors[y][x].r = 255;
					map_colors[y][x].g = 255;
					map_colors[y][x].b = 255;
					break;
				case HERO:
					if (player_found == 0) {
						player->x = x;
						player->y = y + 5;
						player_found = 1;
					}

					map[y][x] = GROUND;
					map_colors[y][x].r = 255;
					map_colors[y][x].g = 255;
					map_colors[y][x].b = 255;
					break;
				case TREE:
					map_colors[y][x].r = 40;
					map_colors[y][x].g = TCOD_random_get_int(NULL, 180, 230);
					map_colors[y][x].b = 40;
					break;
				case BLOOD:
					map[y][x] = GROUND;
					map_colors[y][x].r = TCOD_random_get_int(NULL, 130, 200);
					map_colors[y][x].g = 20;
					map_colors[y][x].b = 20;
					break;
				case GRASS:
					map_colors[y][x].r = 0;
					map_colors[y][x].g = TCOD_random_get_int(NULL, 100, 170);
					map_colors[y][x].b = 0;
					break;
				case WATER:
					map_colors[y][x].r = 0;
					map_colors[y][x].g = 60;
					map_colors[y][x].b = TCOD_random_get_int(NULL, 150, 250);
					break;
				default:
					map[y][x] = WALL;
					map_colors[y][x].r = 127;
					map_colors[y][x].g = 127;
					map_colors[y][x].b = 127;
					break;
			}
		}

		x++;
	}

	fclose(map_file);
	printf("\n");
}

int count_doors(const char *filename) {
	int count = 0;
	char temp;

	FILE *map;

	map = fopen(filename, "r");

	if (map == NULL) {
		printf("[Console] FATAL ERROR: Unable to load map %s.  Please check game installation.\n", filename);
		return -1;
	}

	while (!feof(map)) {
		temp = fgetc(map);

		if (temp == DOOR)
			count++;
	}

	return count;
}

void load_map_config(const char *filename, uint8_t *radius, uint8_t *fov_formula) {
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

TCOD_map_t create_tcod_map(char map[MAP_HEIGHT][MAP_WIDTH], uint8_t discovered[][MAP_WIDTH]) {
	TCOD_map_t tc_map = TCOD_map_new(MAP_WIDTH, MAP_HEIGHT);
	
	int x, y;
	
	for (y = 0; y < MAP_HEIGHT; y++) {
		for (x = 0; x < MAP_WIDTH; x++) {

			discovered[y][x] = 0;

			switch((int)map[y][x]) {
				case BLANK:
				case DOOR:
				case WALL:
					TCOD_map_set_properties(tc_map, x, y, false, false);
					break;
				case TREE:
					TCOD_map_set_properties(tc_map, x, y, false, true);
					break;
				default:
					TCOD_map_set_properties(tc_map, x, y, true, true);
					break;
			}
		}
	}
	
	return tc_map;
}

void use_door(int direction, object_t player, tile_t *door, TCOD_map_t *fov_map,
		char map[MAP_HEIGHT][MAP_WIDTH], int door_count, int open) {

	int i;

	if (direction == UP) {
		for (i = 0; i < door_count; i++) {
			if (door[i].x == player.x && door[i].y == player.y - 1 - 5) {
				if (open == 1) {
					map[door[i].y][door[i].x] = OPEN_DOOR;
					TCOD_map_set_properties(*fov_map, player.x, player.y - 6, true, true);
				}
				else if (open == 0) {
					map[door[i].y][door[i].x] = DOOR;
					TCOD_map_set_properties(*fov_map, player.x, player.y - 6, false, false);
				}
			}
		}
	}
	else if (direction == DOWN) {
		for (i = 0; i < door_count; i++) {
			if (door[i].x == player.x && door[i].y == player.y + 1 - 5) {
				if (open == 1) {
					map[door[i].y][door[i].x] = OPEN_DOOR;
					TCOD_map_set_properties(*fov_map, player.x, player.y - 4, true, true);
				}
				else if (open == 0) {
					map[door[i].y][door[i].x] = DOOR;
					TCOD_map_set_properties(*fov_map, player.x, player.y - 4, false, false);
				}
			}
		}
	}
	else if (direction == RIGHT) {
		for (i = 0; i < door_count; i++) {
			if (door[i].x == player.x + 1 && door[i].y == player.y - 5) {
				if (open == 1) {
					map[door[i].y][door[i].x] = OPEN_DOOR;
					TCOD_map_set_properties(*fov_map, player.x + 1, player.y - 5, true, true);
				}
				else if (open == 0) {
					map[door[i].y][door[i].x] = DOOR;
					TCOD_map_set_properties(*fov_map, player.x + 1, player.y - 5, false, false);
				}
			}
		}
	}
	else if (direction == LEFT) {
		for (i = 0; i < door_count; i++) {
			if (door[i].x == player.x - 1 && door[i].y == player.y - 5) {
				if (open == 1) {
					map[door[i].y][door[i].x] = OPEN_DOOR;
					TCOD_map_set_properties(*fov_map, player.x - 1, player.y - 5, true, true);
				}
				else if (open == 0) {
					map[door[i].y][door[i].x] = DOOR;
					TCOD_map_set_properties(*fov_map, player.x - 1, player.y - 5, false, false);
				}
			}
		}
	}
}
