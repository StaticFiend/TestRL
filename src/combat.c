#include "../include/trl-main.h"

void check_tile(object_t *object, char map[][MAP_WIDTH]) {
	//We use this to check if a tile will hurt us, or inflict some status effect.
	//Also make sure to subtract 5 from the object's y value, as we need to translate this to
	//how the map system works.
	if (map[object->y - 5][object->x] == LAVA)
		//10% health taken per step on lava.
		object->currentHP = ceil(object->maxHP * 0.10);
//	else if (map[object->y - 5][object->x] == BLOOD)
//	TODO: Think about something for blood, maybe other tiles too.
}

void check_attack(object_t *player, object_t *monster, int damage, int attacker) {
	if (attacker == ATTACKER_PLAYER)
		monster->currentHP -= damage;
	else if (attacker == ATTACKER_MONSTER)
		player->currentHP -= damage;
}
