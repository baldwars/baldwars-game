#include "tools.h"

// CELLS
Cell *get_cell()
{
    Warrior *warrior = get_current_warrior();
    return warrior->cell;
}

Cell *get_cell_of(Warrior *warrior)
{
    return warrior->cell;
}