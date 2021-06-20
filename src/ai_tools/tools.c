#include <utils.h>
#include <stdio.h>
#include "tools.h"
#include "../path_finding/path_finding.h"

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
    Nodes *graph = convert_grid_to_nodes(map, MAP_SIZE, MAP_SIZE);

    HashTable *came_from = hash_table_init();
    HashTable *cost_so_far = hash_table_init();

    Node *start = node_init(current_warrior->cell->x, current_warrior->cell->y, 0, 1);
    Node *end = node_init(target->x, target->y, 0, 1);

    a_star_search(graph, start, end, came_from, cost_so_far);
    Nodes *path = reconstruct_path(came_from, start, end);

    print_nodes(path);

    cJSON *json_path = cJSON_CreateArray();

    size_t moves = current_warrior->moves;

    for (int i = 0; i < moves; ++i) {
        Node *node = nodes_dequeue(path);

        if (nodes_are_equals(node, end)) {
            break;
        }
        current_warrior->cell = cell_init(node->x, node->y);
        current_warrior->moves--;

        cJSON *json_cell = log_movement(current_warrior->cell);
        cJSON_AddItemToArray(json_path, json_cell);
    }

    size_t json_path_length = cJSON_GetArraySize(json_path);

    if (json_path_length > 0) {
        cJSON *json_action = log_movement_action(json_path);

    }

    return current_warrior->moves;
}

