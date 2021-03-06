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

Main.h - General header file.
*/

#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>
#include "libtcod.h"

/*
 * This section of code is for all the definitions we need for the game
 * including the minimum/maximum number of dungeon levels, map/screen
 * height/width, etc
 *
 * I use #define for this purpose since it's simple and doesn't add
 * more bloat by defining a ton of new integers.
*/

#define MAP_WIDTH 80
#define MAP_HEIGHT 40
#define SCREEN_WIDTH 80
#define SCREEN_HEIGHT 50

#define MAX_MONSTERS 32
#define MAX_DLVL 100
#define MAX_INVENTORY 24
#define MAX_RANDOM_ITEMS 5

#define MIN_MONSTERS 15
#define MIN_DLVL 60

#define UP 0
#define RIGHT 1
#define DOWN 2
#define LEFT 3

/*
 * Tile definitions (ASCII)
 *
 * We need these to determine which tile is which, much
 * easier to read than a number.
*/
#define BLANK 32
#define WALL 35
#define LAVA 39
#define DOOR 43
#define GRASS 44
#define OPEN_DOOR 45
#define GROUND 46
#define STAIRS 62
#define HERO 64
#define TREE 94
#define BLOOD 96
#define WATER 126

/*
 * Type definitions.
 *
 * These just distinguish between the different types of items.
*/

#define EMPTY_SLOT 0
#define WEAPON 1
#define ARMOR 2
#define SCROLL 3
#define BOOK 4
#define POTION 5
#define AMMO 6

/*
 * Weapon sub-type definitions.
*/

#define DAGGER 0
#define SWORD 1
#define AXE 2
#define MACE 3
#define STAFF 4
#define BOW 5
//...more to come maybe

/*
 * Armor sub-type definitions.
*/

#define CLOTH 0
#define LEATHER 1
#define STUDDED_LEATHER 2
#define CHAIN_MAIL 3
#define PLATE_MAIL 4
//...maybe add a few more here, like studded leather and another type of metal.

/*
 * Scroll sub-type definitions.
*/

#define S_MAPPING 0
#define S_LIGHT 1
#define S_SLEEP 2
#define S_ARMOR 3
#define S_WEAP 4
#define S_IDENT 5
//Definitely going to be more here.
//...the rest of the item sub-types to come!

/*
 * Monster state definitions
 *
 * We use these to determine AI used.and the monsters current thoughts
*/
#define STATE_SLEEP 		1
#define STATE_AGGRESSIVE 	2
#define STATE_NON_AGGRESSIVE	3
#define STATE_FRIGHTENED	4

#define ATTACKER_PLAYER		1
#define ATTACKER_MONSTER	2

/*
 * Type definitions.
*/

int distance(int, int, int, int);
void game_loop(bool);

typedef struct item_s {
	uint8_t x, y;

	uint8_t type;
	uint8_t sub_type;
	uint8_t quantity;

	uint8_t effect;

	bool throwable;

	uint8_t min_dmg;
	uint8_t max_dmg;
	uint8_t plus_hit;
	uint8_t plus_dmg;

	uint8_t special_dmg;

	uint8_t ac;

	bool currently_equipped;
	bool discovered;
} item_t;

typedef struct object_s {
	uint8_t x;
	uint8_t y;

	uint8_t radius;
	uint8_t torch_life;
	bool torch_lit;

	uint8_t currentHP;
	uint8_t currentMP;

	uint8_t maxHP;
	uint8_t maxMP;

	float hpPct;
	float mpPct;

	uint8_t color[3];
	uint8_t symbol;

	uint8_t dlvl;

	uint8_t state;

	uint32_t turns;

	item_t inventory[MAX_INVENTORY];
} object_t;

typedef struct tile_s {
	int index;

	uint8_t x;
	uint8_t y;
} tile_t;


