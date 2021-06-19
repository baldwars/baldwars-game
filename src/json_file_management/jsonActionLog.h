#ifndef BALDWARS_JSONACTIONLOG_H
#define BALDWARS_JSONACTIONLOG_H
#include "../game/game_struct.h"
#include "cJSON.h"

cJSON *actions_;
void logMovement (cJSON *, Character);
void logAttack (cJSON *, int, Character);
void logDefence (cJSON *, Character *);

#endif //BALDWARS_JSONACTIONLOG_H
