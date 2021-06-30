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

size_t canAttack (size_t id) {
    Warrior *current_warrior = get_current_warrior();
    Warrior *enemy = get_warrior_by_id(id);
    int** map_ = get_map();
    int range = inRange(current_warrior, enemy);
    int obs = obstacleInSight(current_warrior, enemy, map_);
    Weapon_ *w = current_warrior->weapon;
    if (range == 1 && obs == 0 &&  current_warrior->actions >= w->cost) {
        return 1;
    }
    else {
        return 0;
    }
}

size_t attack (size_t id) {
    printf("o");
    if (canAttack(id) == 0) {
        printf("target can't be hit");
        return  0;
    }
    else {
        Warrior *current_warrior = get_current_warrior();
        Weapon *weapon = current_warrior->weapon;
        Warrior *enemy = get_warrior_by_id(id);
        int damage = weapon->damage;
        enemy->health = enemy->health - (damage);
        printf("remaining hp %d", enemy->health);
        printf("\n %c %d %d", weapon->name, damage, weapon->cost);
        //logAttack(actions, weapon->id, damage, weapon.cost);
        return current_warrior->actions;
    }
}