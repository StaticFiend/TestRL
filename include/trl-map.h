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

map.h - map.c function prototypes.
*/

void load_map_from_file(const char *filename, char map[][MAP_WIDTH],
		TCOD_color_t map_colors[][MAP_WIDTH], object_t *player,
		int *stair_x, int *stair_y, tile_t *door);
int count_doors(const char *filename);
void load_map_config(const char *filename, uint8_t *radius, uint8_t *fov_formula);

TCOD_map_t create_tcod_map(char map[MAP_HEIGHT][MAP_WIDTH], uint8_t discovered[][MAP_WIDTH]);

void use_door(int direction, object_t player, tile_t *door, TCOD_map_t *fov_map, 
		char map[MAP_HEIGHT][MAP_WIDTH], int door_count, int open);
void close_door(int direction, object_t player, tile_t *door, TCOD_map_t *fov_map,
		char map[MAP_HEIGHT][MAP_WIDTH], int door_count);

//int countDoors(const char *filename);
//void getMapFromFile(const char *filename, char map[MAP_HEIGHT * MAP_WIDTH], TCOD_color_t map_colors[MAP_HEIGHT * MAP_WIDTH], object_t *player, int *stair_x, int *stair_y, tile_t *door, int count);
//TCOD_map_t createTCODmap(char map[MAP_HEIGHT * MAP_WIDTH], uint8_t discovered[MAP_HEIGHT * MAP_WIDTH]);
//void loadMapConfig(const char *filename, uint8_t *radius, uint8_t *fov_formula);
//void open_door(int direction, object_t player, tile_t *door, TCOD_map_t *fov_map, char *map, int door_count);
//void close_door(int direction, object_t player, tile_t *door, TCOD_map_t *fov_map, char *map, int door_count);
