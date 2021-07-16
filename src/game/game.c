#include <stdio.h>
#include <stdlib.h>
#include "game.h"
#include "../scripts/scripts.h"
#include "../utils/path_finding.h"


int **map_;
Warrior *current_warrior_;
Warriors *warriors_;
Weapons *weapons_;

cJSON *json_rounds_;
cJSON *json_warriors_;
cJSON *json_current_warrior_actions_;

// AREA
Area *area_init(size_t min_x, size_t max_x, size_t min_y, size_t max_y)
{
    Area *area = malloc(sizeof(Area));
    *area = (Area){
        .min_x = min_x,
        .max_x = max_x,
        .min_y = min_y,
        .max_y = max_y
    };

    return area;
}

Area *get_area_limits_between(Cell *a, Cell *b)
{
    size_t min_x = (a->x < b->x) ? a->x : b->x;
    size_t max_x = (a->x < b->x) ? b->x : a->x;

    size_t min_y = (a->y < b->y) ? a->y : b->y;
    size_t max_y = (a->y < b->y) ? b->y : a->y;

    return area_init(min_x, max_x, min_y, max_y);
}

Cells *get_wall_in_area(Area *area)
{
    Cell *cell = NULL;
    Cells *walls = cells_init();
    for (size_t i = area->min_x; i <= area->max_x; ++i) {
        for (size_t j = area->min_y; j <= area->max_y; ++j) {
            cell = cell_init(i, j);
            if (cell_is_obstacle(cell)) {
                cells_push_back(walls, cell);
            }
        }
    }

    return walls;
}

// CELL
Cell *cell_init(size_t x, size_t y)
{
    Cell *cell = malloc(sizeof(Cell));
    *cell = (Cell){
        .x = x,
        .y = y
    };

    return cell;
}

Cells *cells_init()
{
    return cells_init_alloc(1);
}

Cells *cells_init_alloc(size_t capacity)
{
    Cells *cells = malloc(sizeof(Cells));
    *cells = (Cells){
            .length = 0,
            .capacity = capacity,
            .items = malloc(sizeof(Cell *) * capacity),
    };

    return cells;
}

void cells_check_alloc(Cells *cells)
{
    if (cells->length >= cells->capacity)
    {
        cells-> capacity += (cells->capacity < CAPACITY_LIMIT) ? cells->capacity : CAPACITY_LIMIT;
        cells->items = realloc(cells->items, (sizeof(Cell *) * cells->capacity));
    }
}

void cells_push_back(Cells *cells, Cell *value)
{
    cells_check_alloc(cells);
    cells->items[cells->length++] = value;
}

Cell *get_corner(size_t corner)
{
    size_t last_index = MAP_SIZE - 1;

    switch (corner) {
        case TOP_LEFT:
            return cell_init(0, 0);
        case TOP_RIGHT:
            return cell_init(0, last_index);
        case DOWN_LEFT:
            return cell_init(last_index, 0);
        default:
            return cell_init(last_index, last_index);
    }
}

Cell *get_opposite_corner_from(Cell *source)
{
    size_t middle = MAP_SIZE / 2;
    size_t last_index = MAP_SIZE - 1;

    if (source->x >= middle && source->y >= middle) {
        return cell_init(0, 0);
    }
    else if (source->x < middle && source->y >= middle) {
        return cell_init(last_index, 0);
    }
    else if (source->x >= middle && source->y < middle) {
        return cell_init(0, last_index);
    }
    else {
        return cell_init(last_index, last_index);
    }
}

unsigned short cell_is_obstacle(Cell *target)
{
    int value = map_[target->x][target->y];
    return value == -1;
}

unsigned short cell_is_entity(Cell *target)
{
    int value = map_[target->x][target->y];
    return value > 0;
}

Cell *get_direction_between(Cell *a, Cell *b)
{
    if (a->x < b->x && a->y > b->y) {
        return cell_init(-1, 1);
    }
    else if (a->x < b->x && a->y < b->y) {
        return cell_init(-1, -1);
    }
    else if (a->x > b->x && a->y > b->y) {
        return cell_init(1, 1);
    }
    else if (a->x > b->x && a->y < b->y) {
        return cell_init(1, -1);
    }
    else if (a->x < b->x && a->y == b->y) {
        return cell_init(-1, 0);
    }
    else if (a->x == b->x && a->y < b->y) {
        return cell_init(0, -1);
    }
    else if (a->x > b->x && a->y == b->y) {
        return cell_init(1, 0);
    }
    else {
        return cell_init(0, 1);
    }
}

size_t get_distance_between(Cell *a, Cell *b)
{
    return abs((int) a->x - (int) b->x) + abs((int) a->y - (int) b->y);
}

void print_cells(Cells *cells)
{
    printf("[ ");
    for (size_t i = 0; i < cells->length; ++i) {
        Cell *cell = cells->items[i];
        if (i == cells->length - 1) {
            printf("{ %zu ; %zu }", cell->x, cell->y);
        }
        else {
            printf("{ %zu ; %zu }, ", cell->x, cell->y);
        }
    }
    printf(" ]\n");
}

// WEAPON
Weapon *weapon_init(size_t id, const char *name, size_t damage, size_t level,
                    size_t cost, size_t min_range, size_t max_range)
{
    Weapon *weapon = malloc(sizeof(Weapon));
    *weapon = (Weapon){
        .id = id,
        .name = name,
        .damage = damage,
        .level = level,
        .cost = cost,
        .min_range = min_range,
        .max_range = max_range
    };

    return weapon;
}

Weapon *get_weapon_by_id(size_t weapon_id)
{
    for (size_t i = 0; i < weapons_->length; ++i) {
        Weapon *weapon = weapons_->items[i];
        if (weapon->id == weapon_id) {
            return weapon;
        }
    }

    return NULL;
}

Weapon *load_weapon(cJSON *weapon)
{
    cJSON *id, *name, *damage, *level, *cost, *min_range, *max_range;

    id = cJSON_GetObjectItemCaseSensitive(weapon, "id");
    name = cJSON_GetObjectItemCaseSensitive(weapon, "name");
    damage = cJSON_GetObjectItemCaseSensitive(weapon, "damage");
    level = cJSON_GetObjectItemCaseSensitive(weapon, "level");
    cost = cJSON_GetObjectItemCaseSensitive(weapon, "cost");
    min_range = cJSON_GetObjectItemCaseSensitive(weapon, "min_range");
    max_range = cJSON_GetObjectItemCaseSensitive(weapon, "max_range");

    size_t id_val = (size_t)cJSON_GetNumberValue(id);
    char *name_val = cJSON_GetStringValue(name);
    size_t damage_val = (size_t)cJSON_GetNumberValue(damage);
    size_t level_val = (size_t)cJSON_GetNumberValue(level);
    size_t cost_val = (size_t)cJSON_GetNumberValue(cost);
    size_t min_range_val = (size_t)cJSON_GetNumberValue(min_range);
    size_t max_range_val = (size_t)cJSON_GetNumberValue(max_range);

    return weapon_init(id_val, name_val, damage_val, level_val, cost_val, min_range_val, max_range_val);
}

Weapons *load_weapons()
{
    char *buffer = get_file_content("../src/weapons.json");

    cJSON *item;
    cJSON *parsed = cJSON_Parse(buffer);
    size_t weapons_length = cJSON_GetArraySize(parsed);

    Weapons *weapons = weapons_init();

    for (size_t i = 0; i < weapons_length; ++i)
    {
        item = cJSON_GetArrayItem(parsed, (int)i);
        weapons_push_back(weapons, load_weapon(item));
    }

    return weapons;
}

Weapons *weapons_init()
{
    return weapons_init_alloc(1);
}

Weapons *weapons_init_alloc(size_t capacity)
{
    Weapons *weapons = malloc(sizeof(Weapons));
    *weapons = (Weapons){
            .length = 0,
            .capacity = capacity,
            .items = malloc(sizeof(Weapon *) * capacity),
    };

    return weapons;
}

void weapons_check_alloc(Weapons *weapons)
{
    if (weapons->length >= weapons->capacity)
    {
        weapons-> capacity += (weapons->capacity < CAPACITY_LIMIT) ? weapons->capacity : CAPACITY_LIMIT;
        weapons->items = realloc(weapons->items, (sizeof(Weapon *) * weapons->capacity));
    }
}

void weapons_push_back(Weapons *weapons, Weapon *value)
{
    weapons_check_alloc(weapons);
    weapons->items[weapons->length++] = value;
}

unsigned short is_in_weapon_range(Warrior *warrior, Cell *target)
{
    size_t distance = get_distance_between(warrior->cell, target);

    if (distance < warrior->weapon->min_range || distance > warrior->weapon->max_range) {
        return 0;
    }

    return 1;
}

Warrior *warrior_init(size_t id, const char *name, size_t level, int health, size_t moves, size_t action)
{
    Warrior *warrior = malloc(sizeof(Warrior));
    *warrior = (Warrior){
        .id = id,
        .name = name,
        .level = level,
        .max_health = health,
        .health = health,
        .actions = action,
        .moves = moves,
        .weapon = NULL,
        .cell = NULL
    };

    return warrior;
}

Warrior *load_warrior(cJSON *warrior)
{
    cJSON *id, *name, *level, *health, *moves, *actions;

    id = cJSON_GetObjectItemCaseSensitive(warrior, "id");
    name = cJSON_GetObjectItemCaseSensitive(warrior, "name");
    level = cJSON_GetObjectItemCaseSensitive(warrior, "level");
    health = cJSON_GetObjectItemCaseSensitive(warrior, "health");
    moves = cJSON_GetObjectItemCaseSensitive(warrior, "moves");
    actions = cJSON_GetObjectItemCaseSensitive(warrior, "actions");

    size_t id_val = (size_t)cJSON_GetNumberValue(id);
    char *name_val = cJSON_GetStringValue(name);
    size_t level_val = (size_t)cJSON_GetNumberValue(level);
    int health_val = (int)cJSON_GetNumberValue(health);
    size_t actions_val = (size_t)cJSON_GetNumberValue(actions);
    size_t moves_val = (size_t)cJSON_GetNumberValue(moves);

    return warrior_init(id_val, name_val, level_val, health_val, moves_val, actions_val);
}

Warriors *load_warriors()
{
    char *buffer = get_file_content("../src/warriors.json");

    cJSON *parsed = cJSON_Parse(buffer);
    int warriors_length = cJSON_GetArraySize(parsed);
    Warriors *warriors = warriors_init();

    for (int i = 0; i < warriors_length; ++i)
    {
        cJSON *item = cJSON_GetArrayItem(parsed, i);
        warriors_push_back(warriors, load_warrior(item));
    }

    return warriors;
}

Warriors *warriors_init()
{
    return warriors_init_alloc(1);
}

Warriors *warriors_init_alloc(size_t capacity)
{
    Warriors *warriors = malloc(sizeof(Warriors));
    *warriors = (Warriors){
            .length = 0,
            .capacity = capacity,
            .items = malloc(sizeof(Warrior *) * capacity),
    };

    return warriors;
}

void warriors_check_alloc(Warriors *warriors)
{
    if (warriors->length >= warriors->capacity)
    {
        warriors-> capacity += (warriors->capacity < CAPACITY_LIMIT) ? warriors->capacity : CAPACITY_LIMIT;
        warriors->items = realloc(warriors->items, (sizeof(Warrior *) * warriors->capacity));
    }
}

void warriors_push_back(Warriors *warriors, Warrior *value)
{
    warriors_check_alloc(warriors);
    warriors->items[warriors->length++] = value;
}

void reset_warrior_action_stats(Warrior *warrior, size_t moves, size_t actions)
{
    warrior->moves = moves;
    warrior->actions = actions;
}

Warrior *get_winner(Warriors *warriors)
{
    Warrior *warrior1 = warriors->items[0];
    Warrior *warrior2 = warriors->items[1];
    Warrior *survivor;

    if ((warrior1->health == (int)warrior1->max_health && warrior2->health == (int)warrior2->max_health)
            ||
        (warrior1->health == warrior2->health))
    {
        survivor = NULL;
    }
    else if (warrior1->health > warrior2->health)
    {
        survivor = warrior1;
    }
    else
    {
        survivor = warrior2;
    }

    return survivor;
}

unsigned short map_is_valid(int **map)
{
    Nodes *nodes = nodes_init();

    for (int i = 0; i < MAP_SIZE; ++i) {
        for (int j = 0; j < MAP_SIZE; ++j) {
            if (map[i][j] > 0) {
                nodes_push_back(nodes, node_init(i, j, 0, 1));
            }
        }
    }

    HashTable *came_from = hash_table_init();
    HashTable *cost_so_far = hash_table_init();
    Nodes *graph = convert_grid_to_nodes(map, MAP_SIZE, MAP_SIZE);

    a_star_search(graph, nodes->items[0], nodes->items[1], came_from, cost_so_far);

    return reconstruct_path(came_from, nodes->items[0], nodes->items[1]) ? 1 : 0;
}

cJSON *game_start()
{
    warriors_ = load_warriors();
    weapons_ = load_weapons();
    map_ = generate_map(warriors_);
    
    size_t current_round = 1;
    unsigned short fight_is_over = 0;
    unsigned short enemy_is_dead = 0;

    while (!fight_is_over) {
        json_warriors_ = NULL;
        for (size_t i = 0; i < warriors_->length; ++i) {
            json_current_warrior_actions_ = NULL;

            current_warrior_ = warriors_->items[i];
            
            size_t moves = current_warrior_->moves;
            size_t actions = current_warrior_->actions;

            if (i == 0) {
                run_script_user1();    
            }
            else {
                run_script_user2();
            }

            log_warrior(current_warrior_->name);
            reset_warrior_action_stats(current_warrior_, moves, actions);

            Warrior *enemy = (i == 0) ? get_warrior_by_id(warriors_->items[1]->id) : get_warrior_by_id(warriors_->items[0]->id);
            if (enemy->health == 0) {
                enemy_is_dead = 1;
                break;
            }
        }

        log_round(current_round);

        if (enemy_is_dead || (++current_round > MAX_ROUND)) {
            fight_is_over = 1;
        }
    }

    Warrior *winner = get_winner(warriors_);
    
    return log_fight(winner);
}

int **map_init()
{
    int **map = malloc(sizeof(int*) * MAP_SIZE);
    for (int i = 0; i < MAP_SIZE; ++i) {
        map[i] = malloc(sizeof(int) * MAP_SIZE);
    }

    return map;
}

void update_map(int **map, Warrior *warrior, Node *node)
{
    map[warrior->cell->x][warrior->cell->y] = 0;
    map[node->x][node->y] = warrior->id;
    warrior->cell = cell_init(node->x, node->y);
    warrior->moves--;
}

unsigned short has_wall_as_neighbor(Cell *cell, int **map)
{
    Node *current = node_init(cell->x, cell->y, cell_is_obstacle(cell), cell_is_entity(cell));
    Nodes *graph = convert_grid_to_nodes(map, MAP_SIZE, MAP_SIZE);
    Nodes *neighbors = neighbors_of(current, graph, DIRECTION_WITH_DIAGONALS);

    for (int i = 0; i < WITH_DIAGONALS_NEIGHBORS_LENGTH; ++i) {
        if (neighbors->items[i]->is_obstacle) {
            return 1;
        }
    }

    return 0;
}

Nodes *get_walls_of(Cell *cell, int **map)
{
    Node *current = node_init(cell->x, cell->y, cell_is_obstacle(cell), cell_is_entity(cell));
    Nodes *graph = convert_grid_to_nodes(map, MAP_SIZE, MAP_SIZE);
    Nodes *neighbors = neighbors_of(current, graph, DIRECTION_WITH_DIAGONALS);

    Nodes *walls = nodes_init();

    for (int i = 0; i < WITH_DIAGONALS_NEIGHBORS_LENGTH; ++i) {
        if (neighbors->items[i]->is_obstacle) {
            nodes_push_back(walls, neighbors->items[i]);
        }
    }

    return walls;
}

unsigned short is_wall_between(Cell *wall, Cell *a, Cell *b)
{
    if (is_wall_horizontally_between(wall, a, b)
        ||
        is_wall_horizontally_between(wall, b, a)
        ||
        is_wall_vertically_between(wall, a, b)
        ||
        is_wall_vertically_between(wall, b, a)
        ||
        is_wall_diagonally_between(wall, a, b)
        ||
        are_symmetrical_from(wall, a, b))
    {
        return 1;
    }

    return 0;
}

unsigned short are_horizontally_aligned(Cell *a, Cell *b)
{
    return a->x == b->x;
}

unsigned short are_vertically_aligned(Cell *a, Cell *b)
{
    return a->y == b->y;
}

unsigned short is_wall_vertically_between(Cell *wall, Cell *a, Cell *b)
{
    if (are_vertically_aligned(a, wall)) {
        if ((are_vertically_aligned(wall, b))
            ||
        (abs((int)wall->y - (int)b->y) == 1 && get_distance_between(a, wall) < get_distance_between(wall, b))
            ||
        (get_distance_between(a, wall) == 1 && are_diagonally_aligned(wall, b)))
        {
            return 1;
        }
    }

    return 0;
}

unsigned short is_wall_horizontally_between(Cell *wall, Cell *a, Cell *b)
{
    if (are_horizontally_aligned(a, wall)) {
        if ((are_horizontally_aligned(wall, b))
            ||
        (abs((int)wall->x - (int)b->x) == 1 && get_distance_between(a, wall) > get_distance_between(wall, b))
            ||
        (get_distance_between(a, wall) == 1 && are_diagonally_aligned(wall, b)))
        {
            return 1;
        }
    }

    return 0;
}

Cells *get_diagonal_cells_from(Cell *from, Cell *direction)
{
    Cells *cells = cells_init();
    Cell *current = cell_init(from->x, from->y);

    while (current->x > 0 && current->x < MAP_SIZE && current->y > 0 && current->y < MAP_SIZE) {
        current->x += direction->x;
        current->y += direction->y;

        cells_push_back(cells, current);
    }

    return cells;
}

unsigned short cells_contains(Cells *cells, Cell *cell)
{
    if (cells == NULL || cell == NULL) {
        return 0;
    }

    Cell *current;

    for (size_t i = 0; i < cells->length; ++i) {
        current = cells->items[i];

        if (current->x == cell->x && current->y == cell->y) {
            return 1;
        }
    }

    return 0;
}

unsigned short are_diagonally_aligned(Cell *a, Cell *b)
{
    Cell *direction = get_direction_between(a, b);
    Cells *diagonal = get_diagonal_cells_from(a, direction);

    if (cells_contains(diagonal, b)) {
        return 1;
    }

    return 0;
}

unsigned short are_symmetrical_from(Cell *wall, Cell *a, Cell *b)
{
    return ((abs((int)a->x - (int)wall->x) == abs((int)wall->x - (int)b->x)
            && !are_vertically_aligned(b, wall))
        || (abs((int)a->y - (int)wall->y) == abs((int)wall->y - (int)b->y)
            && !are_horizontally_aligned(b, wall)));
}

unsigned short is_wall_diagonally_between(Cell *wall, Cell *a, Cell *b)
{
    Cell *direction = get_direction_between(a, b);
    Cells *diagonal = get_diagonal_cells_from(a, direction);

    if (cells_contains(diagonal, b) && cells_contains(diagonal, wall)) {
        if ((a->x < wall->x && b->x > wall->x) || (a->x > wall->x && b->x < wall->x)) {
            return 1;
        }
    }

    return 0;
}

void free_map(int **map)
{
    for (int i = 0; i < MAP_SIZE; ++i) {
        free(map[i]);
    }

    free(map);
}

int **generate_map(Warriors *warriors)
{
    int **map = NULL;
    unsigned short is_valid_map;

    do {
        map = generate_random_map();
        locate_warriors_on_map(&map, warriors);

        is_valid_map = map_is_valid(map);

        if (!is_valid_map) {
            free_map(map);
            map = NULL;
        }

    } while (!is_valid_map);

    return map;
}

int **generate_random_map()
{
    int **map = map_init();

    for (int i = 0; i < MAP_SIZE; ++i) {
        for (int j = 0; j < MAP_SIZE; ++j) {
            map[i][j] = 0;
        }
    }

    generate_walls_on_map(&map);
    no_walls_in_corners(&map);

    return map;
}

void no_walls_in_corners(int ***map)
{
    (*map)[0][0] = 0;
    (*map)[0][MAP_SIZE - 1] = 0;
    (*map)[MAP_SIZE - 1][0] = 0;
    (*map)[MAP_SIZE - 1][MAP_SIZE - 1] = 0;
}

void generate_walls_on_map(int ***map)
{
    size_t min = (size_t)(MAP_SIZE * MAP_SIZE * WALLS_MIN_RATIO);
    size_t max = (size_t)(MAP_SIZE * MAP_SIZE * WALLS_MAX_RATIO);
    size_t walls_number = rand() % (max - min + 1) + min;

    for (size_t i = 0; i < walls_number; ++i) {
        size_t random_x, random_y;

        do {
            random_x = rand() % MAP_SIZE;
            random_y = rand() % MAP_SIZE;
        } while ((*map)[random_x][random_y] == -1);

        (*map)[random_x][random_y] = -1;
    }
}

void locate_warriors_on_map(int *** map, Warriors *warriors)
{
    size_t random_x, random_y;

    for (size_t i = 0; i < warriors->length; ++i) {
        do {
            if (i % 2 == 0) {
                random_y = rand() % MAP_SIZE / 2;
            }
            else {
                size_t min = MAP_SIZE / 2;
                size_t max = MAP_SIZE;
                random_y = rand() % (max - min) + min;
            }
            random_x = rand() % MAP_SIZE;
        } while ((*map)[random_x][random_y] == -1);

        (*map)[random_x][random_y] = warriors->items[i]->id;
        warriors->items[i]->cell = cell_init(random_x, random_y);
    }
}

void print_map(int **map)
{
    printf("Map:\n");
    for (int i = 0; i < MAP_SIZE; ++i) {
        for (int j = 0; j < MAP_SIZE; ++j) {
            printf("%d ", map[i][j]);
        }
        printf("\n");
    }
    printf("\n");
}

void print_warrior(Warrior *warrior)
{
    printf("%s:\n", warrior->name);
    printf("  id: %zu\n", warrior->id);
    printf("  level: %zu\n", warrior->level);
    printf("  health: %d\n", warrior->health);
    printf("  moves: %zu\n", warrior->moves);
    printf("  actions: %zu\n", warrior->actions);
    printf("  weapon:\n");
    printf("    id: %zu\n", warrior->weapon->id);
    printf("    level: %zu\n", warrior->weapon->level);
    printf("    cost: %zu\n", warrior->weapon->cost);
    printf("    damage: %zu\n", warrior->weapon->damage);
}

void print_warriors(Warriors *warriors)
{
    for (size_t i = 0; i < warriors->length; ++i) {
        print_warrior(warriors->items[i]);
        printf("\n");
    }
}

// JSON
void log_movement(Cell *cell, cJSON **json_path)
{
    cJSON *json_cell = cJSON_CreateObject();
    cJSON_AddNumberToObject(json_cell, "x", cell->x);
    cJSON_AddNumberToObject(json_cell, "y", cell->y);

    if (!(*json_path))
    {
        *json_path = cJSON_CreateArray();
    }

    cJSON_AddItemToArray(*json_path, json_cell);
}

void log_movements_action(cJSON *json_path)
{
    cJSON *json_action = cJSON_CreateObject();
    cJSON_AddStringToObject(json_action, "type", "move");
    cJSON *path = (!json_path) ? cJSON_CreateArray() : json_path;

    cJSON_AddItemToObject(json_action, "path", path);

    log_warrior_action(json_action);
}

void log_attack_action(size_t weapon_id, size_t cost)
{
    cJSON *json_action = cJSON_CreateObject();
    cJSON_AddStringToObject(json_action, "type", "attack");
    cJSON *json_attack_result = cJSON_CreateObject();
    cJSON_AddNumberToObject(json_attack_result, "id", weapon_id);
    cJSON *weapon_cost = cJSON_CreateNumber(cost);
    cJSON_AddItemToObject(json_attack_result, "cost", weapon_cost);

    cJSON_AddItemToObject(json_action, "weapon", json_attack_result);

    log_warrior_action(json_action);
}

void log_equip_weapon_action(size_t weapon_id)
{
    cJSON *json_action = cJSON_CreateObject();
    cJSON_AddStringToObject(json_action, "type", "equip");
    cJSON_AddNumberToObject(json_action, "weapon", weapon_id);

    log_warrior_action(json_action);
}

void log_warrior_action(cJSON *warrior_action)
{
    if (!json_current_warrior_actions_) {
        json_current_warrior_actions_ = cJSON_CreateArray();
    }

    cJSON_AddItemToArray(json_current_warrior_actions_, warrior_action);
}

void log_warriors(cJSON *json_warrior)
{
    if (!json_warriors_) {
        json_warriors_ = cJSON_CreateArray();
    }

    cJSON_AddItemToArray(json_warriors_, json_warrior);
}

void log_warrior(const char *warrior_name)
{
    cJSON *json_warrior = cJSON_CreateObject();
    cJSON_AddStringToObject(json_warrior, "name", warrior_name);

    cJSON *actions = (!json_current_warrior_actions_) ? cJSON_CreateArray() : json_current_warrior_actions_;
    cJSON_AddItemToObject(json_warrior, "actions", actions);

    log_warriors(json_warrior);
}

void log_round(size_t round)
{
    cJSON *json_round = cJSON_CreateObject();
    cJSON_AddNumberToObject(json_round, "round", round);
    cJSON *warriors = (!json_warriors_) ? cJSON_CreateArray() : json_warriors_;

    cJSON_AddItemToObject(json_round, "warriors", warriors);

    log_rounds(json_round);
}

void log_rounds(cJSON *json_round)
{
    if (!json_rounds_) {
        json_rounds_ = cJSON_CreateArray();
    }

    cJSON_AddItemToArray(json_rounds_, json_round);
}

void log_winner(Warrior *winner, cJSON *fight)
{
    if (winner == NULL) {
        cJSON_AddNullToObject(fight, "winner");
    } else {
        cJSON_AddNumberToObject(fight, "winner", winner->id);
    }
}

cJSON *log_fight(Warrior *winner)
{
    cJSON *json_fight = cJSON_CreateObject();
    log_winner(winner, json_fight);
    cJSON *rounds = (!json_rounds_) ? cJSON_CreateArray() : json_rounds_;

    cJSON_AddItemToObject(json_fight, "fight", rounds);

    return json_fight;
}

// ACCESSORS
int **get_map()
{
    return map_;
}

Warrior *get_current_warrior()
{
    return current_warrior_;
}

Warrior *get_warrior_by_id(size_t id)
{
    for (size_t i = 0; i < warriors_->length; ++i) {
        Warrior *warrior = warriors_->items[i];
        if (warrior->id == id) {
            return warrior;
        }
    }

    return NULL;
}

Warriors *get_warriors()
{
    return warriors_;
}

// SEARCH
Nodes *a_star_algorithm(int **map, Warrior *current, Node *target)
{
    Nodes *graph = convert_grid_to_nodes(map, MAP_SIZE, MAP_SIZE);

    HashTable *came_from = hash_table_init();
    HashTable *cost_so_far = hash_table_init();

    Node *start = node_init(current->cell->x, current->cell->y, 0, 1);

    a_star_search(graph, start, target, came_from, cost_so_far);

    return reconstruct_path(came_from, start, target);
}
