#ifndef BALDWARS_GAME_TOOLS_H
#define BALDWARS_GAME_TOOLS_H

#include "../game/game.h"

// CELLS
Cell *get_cell();
Cell *get_cell_of(size_t);
size_t get_distance_between(Cell *, Cell *);

// MOVEMENTS
size_t move_toward(size_t);
size_t move_toward_with_moves(Cell *, size_t);
size_t move_away_from(size_t);
size_t move_away_from_with_moves(Cell *, size_t);


#endif //BALDWARS_GAME_TOOLS_H
