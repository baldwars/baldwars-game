#include <utils.h>
#include <stdio.h>
#include "tools.h"

// CELLS
Cell *get_cell()
{
    Warrior *warrior = get_current_warrior();
    return warrior->cell;
}

Cell *get_cell_of_warrior(size_t id)
{
    Warrior *warrior = get_warrior_by_id(id);
    return warrior->cell;
}

size_t get_distance(size_t from, size_t to)
{
    Cell *a = get_cell_of_warrior(from);
    Cell *b = get_cell_of_warrior(to);

    return get_distance_between(a, b);
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

size_t move_toward_with_moves(size_t id, size_t moves)
{
    Warrior *current_warrior = get_current_warrior();
    Warrior *enemy = get_warrior_by_id(id);
    int **map = get_map();
    Node *target = node_init(enemy->cell->x, enemy->cell->y, 0, 1);
    Nodes *path = a_star_algorithm(map, current_warrior, target);
    cJSON *json_path = NULL;

    if (moves > current_warrior->moves) {
        moves = current_warrior->moves;
    }

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

size_t move_away_from(size_t id)
{
    Warrior *current_warrior = get_current_warrior();
    Warrior *enemy = get_warrior_by_id(id);
    int **map = get_map();

    Cell *corner;
    Node *target;
    Nodes *path;
    Node *node_enemy;

    for (int i = 0; i < CORNERS_NUMBER; ++i) {
        corner = get_corner(i);
        target = node_init(corner->x, corner->y, cell_is_obstacle(corner), cell_is_entity(corner));
        path = a_star_algorithm(map, current_warrior, target);
        node_enemy = node_init(enemy->cell->x, enemy->cell->y, 0, 1);

        if (!nodes_includes(path, node_enemy)) {
            break;
        }
    }

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

size_t move_away_from_with_moves(size_t id, size_t moves)
{
    Warrior *current_warrior = get_current_warrior();
    Warrior *enemy = get_warrior_by_id(id);
    int **map = get_map();

    Cell *corner;
    Node *target;
    Nodes *path;
    Node *node_enemy;

    for (int i = 0; i < CORNERS_NUMBER; ++i) {
        corner = get_corner(i);
        target = node_init(corner->x, corner->y, cell_is_obstacle(corner), cell_is_entity(corner));
        path = a_star_algorithm(map, current_warrior, target);
        node_enemy = node_init(enemy->cell->x, enemy->cell->y, 0, 1);

        if (!nodes_includes(path, node_enemy)) {
            break;
        }
    }

    cJSON *json_path = NULL;

    if (moves > current_warrior->moves) {
        moves = current_warrior->moves;
    }

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

// WEAPON
size_t get_weapon()
{
    Warrior *warrior = get_current_warrior();
    return (warrior->weapon) ? warrior->weapon->id : 0;
}

void set_weapon(size_t weapon_id)
{
    Warrior *warrior = get_current_warrior();
    Weapon *weapon = get_weapon_by_id(weapon_id);

    if (weapon->level <= warrior->level)
    {
        warrior->weapon = weapon;
        warrior->actions -= 1;
    }
}

unsigned short can_use_weapon(size_t target_id)
{
    Warrior *current_warrior = get_current_warrior();
    if (!current_warrior->weapon) {
        return 0;
    }

    Warrior *enemy = get_warrior_by_id(target_id);
    if (!is_in_weapon_range(current_warrior, enemy->cell)) {
        return 0;
    }

    if (current_warrior->actions < current_warrior->weapon->cost) {
        return 0;
    }

    Area *area = get_area_limits_between(current_warrior->cell, enemy->cell);
    Cells *walls = get_wall_in_area(area);

    print_cells(walls);

    for (size_t i = 0; i < walls->length; ++i)
    {
        Cell *wall = walls->items[i];

        if (is_wall_between(wall, current_warrior->cell, enemy->cell)) {
            return 0;
        }
    }

    return 1;
}

// SEARCH
size_t get_nearest_enemy()
{
    Warrior *current_warrior = get_current_warrior();
    Warrior **warriors = get_warriors();
    size_t length = get_warriors_number();
    size_t min_distance = INT_MAX;
    size_t nearest_enemy = 0;
    size_t distance;

    for (int i = 0; i < length; ++i) {
        if (warriors[i]->id != current_warrior->id) {
            distance = get_distance_between(warriors[i]->cell, current_warrior->cell);

            if (min_distance > distance) {
                min_distance = distance;
                nearest_enemy = warriors[i]->id;
            }
        }
    }

    return nearest_enemy;
}
