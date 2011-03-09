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

cave-walk.c - this handles the random walk routines for generating caves.
*/

#include "../include/trl-main.h"
#include "../include/trl-map.h"

void cave_walk(int dir, char map[][MAP_WIDTH], tile_t *walker) {
	switch(dir) {
		case 1:
			if ((walker->y - 1) > 0)
				walker->y -= 1;
			 break;
		case 2:
			 if ((walker->y + 1) < (MAP_HEIGHT - 1))
				walker->y += 1;
			 break;
		case 3:
		case 4:
		case 5:
			 if ((walker->x - 1) > 0)
				 walker->x -= 1;
			 break;
		case 6:
		case 7:
		case 8:
			 if ((walker->x + 1) < (MAP_WIDTH - 1))
				 walker->x += 1;
			 break;
		default:
			 printf("[Console] Invalid direction, how'd you manage this?\n");
			 printf("[Console] walker.x: %i  walker.y: %i  direction: %i\n", walker->x, walker->y, dir);
			 break;
	}

	if ((walker->y > 0) && (walker->y < MAP_HEIGHT - 1) && (walker->x > 0) && (walker->x < MAP_WIDTH - 1))
		map[walker->y][walker->x] = GROUND;
}

void cave_place_player(char map[][MAP_WIDTH], object_t *player) {
	int player_placed = 0;

	while (player_placed = 0) {
		player->x = TCOD_random_get_int(NULL, 0, MAP_WIDTH - 1);
		player->y = TCOD_random_get_int(NULL, 0, MAP_HEIGHT - 1);

		if (map[player->y][player->x] == GROUND)
			player_placed = 1;
	}

	player->y += 5;
}

void cave_place_stairs(char map[][MAP_WIDTH], object_t player, tile_t *stairs) {
	int stairs_placed = 0;
	int largest_x, largest_y;
	int cur_dist, largest_dist = 0;
	int times_run = 0;

	while (stairs_placed = 0) {
		stairs->x = TCOD_random_get_int(NULL, 0, MAP_WIDTH - 1);
		stairs->y = TCOD_random_get_int(NULL, 0, MAP_HEIGHT - 1);

		cur_dist = distance(player.x, player.y, stairs->x, stairs->y);
		if (cur_dist > largest_dist) {
			largest_x = stairs->x;
			largest_y = stairs->y;
			largest_dist = cur_dist;
		}

		if (times_run == 20) {
			stairs->x = largest_x;
			stairs->y = largest_y;
			map[largest_y][largest_x] = STAIRS;
		}
	}
}

void generate_cave(char map[][MAP_WIDTH], TCOD_map_t *fov_map, TCOD_color_t color_map[][MAP_WIDTH],
		uint8_t discovered[][MAP_WIDTH], object_t *player, tile_t *stairs) {

	int count = 0, limit, direction = 0;

	tile_t walker;

	walker.x = TCOD_random_get_int(NULL, 0, MAP_WIDTH - 1);
	walker.y = TCOD_random_get_int(NULL, 0, MAP_HEIGHT - 1);

	map[walker.y][walker.x] = GROUND;

	limit = TCOD_random_get_int(NULL, 4000, 6000);

	while (count < limit) {
		direction = TCOD_random_get_int(NULL, 1, 8);
		random_walk(dir, map, &walker);
		count++;
	}

	create_tcod_map(map, discovered);
	cave_place_player(map, player);
	cave_place_stairs(map, *player, stairs);
	random_get_colors(map, color_map);

	//FIXME: Finish this.
}
