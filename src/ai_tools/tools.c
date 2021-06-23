#include <utils.h>
#include <stdio.h>
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

// MOVEMENTS
size_t move_toward(Cell *target)
{
    Warrior *current_warrior = get_current_warrior();
    int **map = get_map();
    Node *end = node_init(target->x, target->y, 0, 1);
    Nodes *path = a_star_algorithm(map, current_warrior, target);
    cJSON *json_path = NULL;
    size_t moves = current_warrior->moves;

    for (int i = 0; i < moves; ++i) {
        Node *node = nodes_dequeue(path);

        if (!node || nodes_are_equals(node, end)) {
            break;
        }

        update_map(map, current_warrior, node);
        log_movement(current_warrior->cell, &json_path);
    }

    size_t json_path_length = cJSON_GetArraySize(json_path);

    if (json_path_length > 0) {
        log_movements_action(json_path);
    }

    return current_warrior->moves;
}

