#ifndef BALDWARS_GAME_TOOLS_H
#define BALDWARS_GAME_TOOLS_H

#include "../game/game.h"

// CELLS
Cell *get_cell();
Cell *get_cell_of(Warrior *);

// MOVEMENTS
size_t move_toward_enemy(Cell *);
size_t move_toward_enemy_with_moves(Cell *, size_t);
size_t move_away_from_enemy(Cell *);
size_t move_away_from_enemy_with_moves(Cell *, size_t);

#endif //BALDWARS_GAME_TOOLS_H
