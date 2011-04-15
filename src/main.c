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
//FIXME: Maybe move this to game.c
int distance(int x1, int y1, int x2, int y2) {
	return sqrt(pow(x2 - x1, 2) + pow(y2 - y1, 2));
}

//FIXME: Make this better.
//FIXME: Move this to game.c or somewhere else.
void createMonster(object_t *monster, int mon_num, TCOD_map_t map) {
	int i;

	for (i = 0; i < mon_num; i++) {
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
}

//TODO: Make a title screen and all that good stuff.
int main(/*int argc, char **argv*/) {
	FILE *save_file;
	bool save_detected = false;

	TCOD_console_set_custom_font("data/font/testrl-font1.png", TCOD_FONT_LAYOUT_TCOD|TCOD_FONT_TYPE_GREYSCALE, 0, 0);
	TCOD_console_init_root(SCREEN_WIDTH, SCREEN_HEIGHT, "TestRL - Pre-alpha", false);
//	TCOD_sys_set_fps(25);
//	Possibly make this a command-line switch or ingame option at some point.
//	TCOD_console_disable_keyboard_repeat();

	save_file = fopen("current_game", "r");

	if (save_file != NULL) {
		int size = 0;
		
		save_detected = true;

		printf("[Console] Saved data found, continuing game...\n");
		//do read stuff, tokenize, etc
		fseek(save_file, 0, SEEK_END);
		size = ftell(save_file);
		fseek(save_file, 0, SEEK_SET);

#ifdef DEBUG
		printf("[DEBUG] size: %i\n", size);
#endif
//		saveChar = (char *)malloc((sizeof(char) * size) + 1);

//		fread(saveChar, 1, size, save_file);
//#ifdef DEBUG
//		printf("[DEBUG] saveChar: %s\n", saveChar);
//#endif

		//tokenized stuff beep boop beep
		//FIXME: Figure out how to load this stuff properly, strtok wants everything
		//FIXME: in an array rather than how it's laid out already.
		fclose(save_file);
//		free(saveChar);
		printf("[Console] Deleting saved game...\n");
		remove("current_game");
	}
	
	game_loop(save_detected);

	return 0;
}
