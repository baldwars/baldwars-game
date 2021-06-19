#ifndef BALDWARS_JSONACTIONLOG_H
#define BALDWARS_JSONACTIONLOG_H
#include "../game/game_struct.h"
#include "cJSON.h"

cJSON *actions;
void logMovement (cJSON *actions, Character player);
void logAttack (cJSON *actions, int damage, Character target);
void logDefence (cJSON *actions, Character *player);

#endif //BALDWARS_JSONACTIONLOG_H
