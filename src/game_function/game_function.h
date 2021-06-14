//
// Created by Latif on 07/06/2021.
//

#include <stdbool.h>
#include "game_struct.h"
#ifndef BALDWARS_GAME_GAME_FUNCTION_H
#define BALDWARS_GAME_GAME_FUNCTION_H

void initMap (int index);
void deleteMap (int index);
void setupMap (int index);
void loadScript();
void run ();
bool inRange (Character *target);

#endif //BALDWARS_GAME_GAME_FUNCTION_H
