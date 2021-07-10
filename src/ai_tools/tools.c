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

    Cell *corner = get_opposite_corner_from(enemy->cell);
    Node *target = node_init(corner->x, corner->y, cell_is_obstacle(corner), cell_is_entity(corner));
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

size_t move_away_from_with_moves(size_t id, size_t moves)
{
    Warrior *current_warrior = get_current_warrior();
    Warrior *enemy = get_warrior_by_id(id);
    int **map = get_map();

    Cell *corner = get_opposite_corner_from(enemy->cell);
    Node *target = node_init(corner->x, corner->y, cell_is_obstacle(corner), cell_is_entity(corner));
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

    warrior->weapon = weapon;
}

unsigned short can_use_weapon(size_t id)
{
    Warrior *current_warrior = get_current_warrior();
    if (!current_warrior->weapon) {
        return 0;
    }

    Warrior *enemy = get_warrior_by_id(id);
    if (!is_in_weapon_range(current_warrior, enemy->cell)) {
        return 0;
    }

    if (cells_are_aligned(current_warrior->cell, enemy->cell) && is_wall_between()) {
        return 0;
    }
    else {
        Area *area = get_area_limits_between(current_warrior->cell, enemy->cell);
        Cells *walls = get_wall_in_area(area);

        size_t distance = INT_MAX;
        Cell *wall = NULL;
        size_t sum = 0;
        unsigned short can_shoot = 1;

        for (int i = 0; i < walls->length; ++i) {
            wall = walls->items[i];
            if (get_distance_between(current_warrior->cell, wall) < get_distance_between(wall, enemy->cell)) {
                sum += !((abs((int)wall->x - (int)enemy->cell->x) < 2)
                    || (abs((int)wall->y - (int)enemy->cell->y) < 2));
            }

            if (get_distance_between(enemy->cell, wall) < get_distance_between(wall, current_warrior->cell)) {
                sum += !((abs((int)wall->x - (int)current_warrior->cell->x) < 2)
                        || (abs((int)wall->y - (int)current_warrior->cell->y) < 2));
            }

            can_shoot = (sum % 2 == 0) ? 0 : 1;
        }

        return can_shoot;
    }


    unsigned short current_has_as_wall_neighbor = has_wall_as_neighbor(current_warrior->cell, get_map());
    unsigned short enemy_has_wall_as_neighbor = has_wall_as_neighbor(enemy->cell, get_map());

    Nodes *current_warrior_walls = get_walls_of(current_warrior->cell, get_map());
    Nodes *enemy_walls = get_walls_of(enemy->cell, get_map());

    unsigned short wall_is_between = 0;
    if (enemy_walls->length > 0) {
        wall_is_between += is_wall_between(enemy_walls, current_warrior->cell, enemy->cell);
    }
    if (current_warrior_walls->length > 0) {
        wall_is_between += is_wall_between(current_warrior_walls, current_warrior->cell, enemy->cell);
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
