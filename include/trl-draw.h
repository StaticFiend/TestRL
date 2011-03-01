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

draw.h - draw.c prototypes
*/

void fill_map(char map[MAP_HEIGHT][MAP_WIDTH], const char fill);
void draw_player(object_t player);
void draw_monster(object_t *monster, int num_of_mon, TCOD_map_t fov_map);
void draw_map(char map[][MAP_WIDTH], TCOD_color_t color_map[][MAP_WIDTH],
		TCOD_map_t fov_map, uint8_t discovered[][MAP_WIDTH]);
void draw_hud(object_t player);

/*
void drawPlayer(object_t player);
//void drawMonster(object_t monster[], int array_size);
void drawMonsters(object_t *monster, int mon_num, TCOD_map_t fov_map);
void drawMap(char map[MAP_HEIGHT * MAP_WIDTH], TCOD_color_t map_colors[MAP_HEIGHT * MAP_WIDTH], TCOD_map_t fov_map, uint8_t discovered[MAP_HEIGHT * MAP_WIDTH]);
void drawHud(object_t player);*/
