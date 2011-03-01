#include "../include/trl-main.h"

void check_tile(object_t *object) {
	//We use this to check if a tile will hurt us.
	//FIXME: Current system really doesn't allow this easily, change to 2D arra
	if (TCOD_console_get_char(NULL, object->x, object->y) == LAVA)
		object->currentHP = object->currentHP - (object->maxHP * 0.10);
}

void check_attack(object_t *player, object_t *monster, int damage, int attacker) {
	if (attacker == ATTACKER_PLAYER)
		monster->currentHP -= damage;
	else if (attacker == ATTACKER_MONSTER)
		player->currentHP -= damage;
}
