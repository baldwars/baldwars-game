#include <utils.h>
#include <stdio.h>
#include "tools.h"

// CELLS
Cell *get_cell()
{
    Warrior *warrior = get_current_warrior();
    return warrior->cell;
}

Cell *get_cell_of(size_t id)
{
    Warrior *warrior = get_warrior_by_id(id);
    return warrior->cell;
}

size_t get_distance_between(Cell *a, Cell *b)
{
    return abs((int) a->x - (int) b->x) + abs((int) a->y - (int) b->y);
}

// MOVEMENTS
size_t move_toward(size_t id)
{
    Warrior *current_warrior = get_current_warrior();
    Warrior *enemy = get_warrior_by_id(id);
    int **map = get_map();
    Node *target = node_init(enemy->cell->x, enemy->cell->y, 0, 1);
    Nodes *path = a_star_algorithm(map, current_warrior, target);
    cJSON *json_path = NULL;
    size_t moves = current_warrior->moves;

    for (int i = 0; i < moves; ++i) {
        Node *node = nodes_dequeue(path);

        if (!node || nodes_are_equals(node, target)) {
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

