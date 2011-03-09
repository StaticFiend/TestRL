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

monster.c - Handles all monster related events and stats.
*/

#include "trl-main.h"

//Do some AI stuff here, herp a derp.
void monster_change_state(object_t *monster, int index, uint8_t new_state) {
	monster[index].state = new_state;
}

void ai_move(object_t *monster, int mon_num, TCOD_map_t fov_map, object_t player) {
//	FIXME: Pathing code from this is bugged, so leave this commented.
//	TCOD_path_t mon_path = TCOD_path_new_using_map(fov_map, 0.0f);

	int i;
	int rand_dir = 0;
	static int last_dir;

	for (i = 0; i < mon_num; i++) {
		switch(monster[i].state) {
			case STATE_SLEEP:
				break; //Do nothing, it's asleep.
			case STATE_NON_AGGRESSIVE:
				do {
				rand_dir = TCOD_random_get_int(NULL, 0, 3);
				} while (rand_dir == last_dir);
				
				if (rand_dir == UP && TCOD_map_is_walkable(fov_map, monster[i].x, monster[i].y - 6)) {
					if (monster[i].y - 1 != player.y && monster[i].x != player.x)
						monster[i].y -= 1;
				}
				if (rand_dir == RIGHT && TCOD_map_is_walkable(fov_map, monster[i].x + 1, monster[i].y - 5)) {
					if (monster[i].x + 1 != player.x && monster[i].y != player.y)
						monster[i].x += 1;
				}
				if (rand_dir == DOWN && TCOD_map_is_walkable(fov_map, monster[i].x, monster[i].y - 4)) {
					if (monster[i].y + 1 != player.y && monster[i].x != player.x)
						monster[i].y += 1;
				}
				if (rand_dir == LEFT && TCOD_map_is_walkable(fov_map, monster[i].x - 1, monster[i].y - 5)) {
					if (monster[i].x - 1 != player.x && monster[i].y != player.y)
						monster[i].x -= 1;
				}
				last_dir = rand_dir;
				break;
				//FIXME: Code below is insanely buggy, figure this out later.
			case STATE_AGGRESSIVE:
				//do TCOD_create_path here and make it follow that path one step at a time.
//				TCOD_path_compute(mon_path, monster[i].x, monster[i].y - 5, player.x, player.y - 5);

//				TCOD_path_walk(mon_path, &monster[i].x, &monster[i].y, true);
				break;
			case STATE_FRIGHTENED:
				//run awaaaay
				break;
			default:
				printf("[Console] ERROR: Unknown monster state, dunno how you managed this!\n");
				break;
		}
	}
}
