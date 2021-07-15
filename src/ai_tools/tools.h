#ifndef BALDWARS_GAME_TOOLS_H
#define BALDWARS_GAME_TOOLS_H

#include "../game/game.h"

#define WEAPON_PISTOL 1
#define WEAPON_SHOTGUN 2
#define WEAPON_KATANA 3
#define WEAPON_BLASTER 4

// CELLS
Cell *get_cell();
Cell *get_cell_of_warrior(size_t);
size_t get_distance(size_t, size_t);

// MOVEMENTS
size_t move_toward(size_t);
size_t move_toward_with_moves(size_t, size_t);
size_t move_away_from(size_t);
size_t move_away_from_with_moves(size_t, size_t);

// WEAPON
size_t get_weapon();
void set_weapon(size_t);
unsigned short can_use_weapon(size_t);
void use_weapon(size_t);

// SEARCH
size_t get_nearest_enemy();


#endif //BALDWARS_GAME_TOOLS_H
