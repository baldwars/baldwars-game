#include "game.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include "../utils/utils.h"
#include "../path_finding/path_finding.h"
#include "../ai_tools/tools.h"

const int smallTABSIZE = 25;
const int mediumTABSIZE = 50;
const int largeTABSIZE = 100;
const int smallOBSTACLE = 125;
const int mediumOBSTACLE = 500;
const int largeOBSTACLE = 2000;

const long blockSize = 24;
long count = 20;
//char buffer[blockSize];
int turn = 0;
int nbPlayers = 2;

int **map_;
Warrior *current_warrior_;
Warrior **warriors_;
size_t *warriors_number_;
Weapon **weapons_;
size_t *weapons_number_;

cJSON *json_rounds_;
cJSON *json_warriors_;
cJSON *json_current_warrior_actions_;

void loadScript() {
    FILE *scriptFile;
    long read;
    char *path = "../script0.c";
    path[9] = (char) turn % nbPlayers;
    scriptFile = fopen(path, "r");
    if (scriptFile == NULL) {
        printf("error");
        exit(1);
    }
    //fread(buffer, blockSize, count, scriptFile);
    fclose(scriptFile);
}





size_t inRange (Warrior *current, Warrior *enemy) {
    int x, y;
    Cell *current_cell = current->cell;
    Cell *enemy_cell = enemy->cell;
    Weapon *w = current->weapon;
    x = abs(current_cell->x - enemy_cell->x);
    y = abs(current_cell->y - enemy_cell->y);
    printf("o");
    if (x + y >= w->min_range && x + y <= w->max_range) {
        printf("in range");
        return 1;
    } else {
        printf("not in range");
        return 0;
    }
}

size_t linearXObstacleCheck (size_t a, size_t b, size_t x, int** m) {
    if (a < b) {
        for (int j = a; j < b; j++) {
            if (m[x][j] == -1) {
                return 1;
            }
        }
        return 0;
    }
    else {
        for (int j = b; j < a; j++) {
            if (m[x][j] == -1) {
                return 1;
            }
        }
        return 0;
    }
}

size_t linearYObstacleCheck (size_t a, size_t b, size_t y, int** m) {
    if (a < b) {
        for (int i = a; i < b; i++) {
            if (m[i][y] == -1) {
                return 1;
            }
        }
        return 0;
    }

    else {
        for (int i = b; i < a; i++) {
            if (m[i][y] == -1) {
                return 1;
            }
        }
        return 0;
    }
}

size_t diagonalObstacleCheck (size_t x1, size_t x2, size_t y, int** m) {
    int j = y;
    for (int i = x1; i < x2; i++) {
        if (m[i][j] == -1) {
            return 1;
        }
        j++;
    }
    return 0;
}

size_t fullDiagonalObstacleCheck (size_t x1, size_t x2, size_t y1, size_t y2, int** m) {
    if (x1 < x2) {
        if (y1 < y2) {
            if (m[x1+1][y1] == -1 && m[x1][y1+1] == -1) {
                return 1;
            }
            return diagonalObstacleCheck(x1, x2, y1, m);
        }
        else {
            if (m[x1+1][y1] == -1 && m[x1][y1-1] == -1) {
                return 1;
            }
            return diagonalObstacleCheck(x1, x2, y2, m);
        }
    }
    else {
        if (y1 < y2) {
            if (m[x1-1][y1] == -1 && m[x1][y1+1] == -1) {
                return 1;
            }
            return diagonalObstacleCheck(x2, x1, y1, m);
        }
        else {
            if (m[x1-1][y1] == -1 && m[x1][y1-1] == -1) {
                return 1;
            }
            return diagonalObstacleCheck(x2, x1, y2, m);
        }
    }
}

size_t lastObstacleCheckX (size_t x1, size_t x2, size_t y1, size_t y2, int** m) {
    for (int i = x1+1; i <= x2; i++) {
        for (int j = y1+1; j < y2; j++) {
            if (m[i][j] == -1) {
                return 1;
            }
        }
    }
    return 0;
}

size_t lastObstacleCheckXFull (size_t x1, size_t x2, size_t y1, size_t y2, int** m) {
    if (x1 < x2) {
        if (y1 < y2) {
            if (m[x1][y1+1] == -1) {
                return 1;
            }
            return lastObstacleCheckX(x1, x2, y1, y2, m);
        }
        else {
            if (m[x1][y1-1] == -1) {
                return 1;
            }
            return lastObstacleCheckX(x1, x2, y2, y1, m);
        }
    }
    else {
        if (y1 < y2) {
            if (m[x1][y1+1] == -1) {
                return 1;
            }
            return lastObstacleCheckX(x2, x1, y1, y2, m);
        }
        else {
            if (m[x1][y1-1] == -1) {
                return 1;
            }
            return lastObstacleCheckX(x2, x1, y2, y1, m);
        }
    }
}

size_t lastObstacleCheckY (size_t x1, size_t x2, size_t y1, size_t y2, int** m) {
    for (int i = x1+1; i < x2; i++) {
        for (int j = y1+1; j <= y2; j++) {
            if (m[i][j] == -1) {
                return 1;
            }
        }
    }
    return 0;
}

size_t lastObstacleCheckYFull (size_t x1, size_t x2, size_t y1, size_t y2, int** m) {
    if (x1 < x2) {
        if (y1 < y2) {
            if (m[x1+1][y1] == -1) {
                return 1;
            }
            return lastObstacleCheckY (x1, x2, y1, y2, m);
        }
        else {
            if (m[x1-1][y1] == -1) {
                return 1;
            }
            return lastObstacleCheckY (x1, x2, y2, y1, m);
        }
    }
    else {
        if (y1 < y2) {
            if (m[x1+1][y1] == -1) {
                return 1;
            }
            return lastObstacleCheckY (x2, x1, y1, y2, m);
        }
        else {
            if (m[x1-1][y1] == -1) {
                return 1;
            }
            return lastObstacleCheckY (x2, x1, y2, y1, m);
        }
    }
}

size_t obstacleInSight (Warrior *current, Warrior *enemy, int** m) {
    Cell *current_cell = current->cell;
    Cell *enemy_cell = enemy->cell;
    if (current_cell->x == enemy_cell->x) {
        linearXObstacleCheck(current_cell->y, enemy_cell->y, current_cell->x, m);
    }

    else if (current_cell->y == enemy_cell->y) {
        linearYObstacleCheck(current_cell->x, enemy_cell->x, current_cell->y, m);
    }

    else {
        int absX = abs(current_cell->x - enemy_cell->x);
        int absY = abs(current_cell->y - enemy_cell->y);
        if (absX == absY) {
            return fullDiagonalObstacleCheck(current_cell->x, enemy_cell->x, current_cell->y, enemy_cell->y, m);
        }
        else {
            if (absX < absY) {
                lastObstacleCheckXFull (current_cell->x, enemy_cell->x, current_cell->y, enemy_cell->y, m);
            }
            else {
                lastObstacleCheckYFull (current_cell->x, enemy_cell->x, current_cell->y, enemy_cell->y, m);
            }
        }
    }
}

/*int canAttack (Character *target) {
    int range = inRange(target);
    int obs = obstacleInSight(target);
    Weapon w = currentPlayer->weapon;
    if (range == 1 && obs == 0 &&  currentPlayer->actionPoint >= w.cost) {
        return 1;
    }
    else {
        return 0;
    }
}

void attack (Character *target) {
    Weapon weapon = currentPlayer->weapon;
    if (canAttack(target) == 0) {
        printf("target can't be hit");
    }
    else {
        int damage = weapon.damage - target->armor;
        target->hp = target->hp - (damage);
        printf("remaining hp %d", target->hp);
        printf("\n %d %d %d", weapon.id, damage, weapon.cost);
        logAttack(actions, weapon.id, damage, weapon.cost);
    }
}*/




void writeJSON (char *string) {
    FILE *jsonFile;
    jsonFile = fopen("./fight.json", "w");
    if (jsonFile == NULL) {
        printf("error");
        exit(1);
    }
    fprintf(jsonFile, string);
    fclose(jsonFile);
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

unsigned short cells_contains(Cells *cells, Cell *cell)
{
    if (cells == NULL || cell == NULL) {
        return 0;
    }

    Cell *current;
    for (int i = 0; i < cells->length; ++i) {
        current = cells->items[i];
        if (current->x == cell->x && current->y == cell->y) {
            return 1;
        }
    }

    return 0;
}

Warrior *warrior_init(unsigned short id, const char *name, size_t level, size_t health, size_t moves, size_t action)
{
    Warrior *warrior = malloc(sizeof(Warrior));
    *warrior = (Warrior){
        .id = id,
        .name = name,
        .level = level,
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
    size_t health_val = (size_t)cJSON_GetNumberValue(health);
    size_t actions_val = (size_t)cJSON_GetNumberValue(actions);
    size_t moves_val = (size_t)cJSON_GetNumberValue(moves);

    return warrior_init(id_val, name_val, level_val, health_val, moves_val, actions_val);
}

Warrior **load_warriors(size_t *warriors_length)
{
    char *buffer = get_file_content("./warriors.json");

    cJSON *item;
    cJSON *parsed = cJSON_Parse(buffer);
    *warriors_length = cJSON_GetArraySize(parsed);
    Warrior **warriors = malloc(sizeof(Warrior *) * (*warriors_length));

    for (int i = 0; i < *warriors_length; ++i)
    {
        item = cJSON_GetArrayItem(parsed, i);
        warriors[i] = load_warrior(item);
    }

    return warriors;
}

Weapon *weapon_init(size_t id, const char *name, size_t damage, size_t cost, size_t min_range, size_t max_range)
{
    Weapon *weapon = malloc(sizeof(Weapon));
    *weapon = (Weapon){
            .id = id,
            .name = name,
            .damage = damage,
            .cost = cost,
            .min_range = min_range,
            .max_range = max_range
    };

    return weapon;
}

Weapon *load_weapon(cJSON *weapon)
{
    cJSON *id, *name, *damage, *cost, *min_range, *max_range;

    id = cJSON_GetObjectItemCaseSensitive(weapon, "id");
    name = cJSON_GetObjectItemCaseSensitive(weapon, "name");
    damage = cJSON_GetObjectItemCaseSensitive(weapon, "damage");
    cost = cJSON_GetObjectItemCaseSensitive(weapon, "cost");
    min_range = cJSON_GetObjectItemCaseSensitive(weapon, "min_range");
    max_range = cJSON_GetObjectItemCaseSensitive(weapon, "max_range");

    size_t id_val = (size_t)cJSON_GetNumberValue(id);
    char *name_val = cJSON_GetStringValue(name);
    size_t damage_val = (size_t)cJSON_GetNumberValue(damage);
    size_t cost_val = (size_t)cJSON_GetNumberValue(cost);
    size_t min_range_val = (size_t)cJSON_GetNumberValue(min_range);
    size_t max_range_val = (size_t)cJSON_GetNumberValue(max_range);

    return weapon_init(id_val, name_val, damage_val, cost_val, min_range_val, max_range_val);
}

Weapon **load_weapons(size_t *weapons_length)
{
    char *buffer = get_file_content("./weapons.json");

    cJSON *item;
    cJSON *parsed = cJSON_Parse(buffer);
    *weapons_length = cJSON_GetArraySize(parsed);
    Weapon **weapons = malloc(sizeof(Weapon *) * (*weapons_length));

    for (int i = 0; i < *weapons_length; ++i)
    {
        item = cJSON_GetArrayItem(parsed, i);
        weapons[i] = load_weapon(item);
    }

    return weapons;
}

void reset_warrior_action_stats(Warrior *warrior, size_t moves, size_t actions)
{
    warrior->moves = moves;
    warrior->actions = actions;
    warrior->weapon = NULL;
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
    warriors_number_ = malloc(sizeof(size_t));
    warriors_ = load_warriors(warriors_number_);
    weapons_number_ = malloc(sizeof(size_t));
    weapons_ = load_weapons(weapons_number_);
    printf("\n %s \n", weapons_[0]->name);

    map_ = generate_map(warriors_, *warriors_number_);
    print_map(map_);

    size_t current_round = 1;
    size_t round_limit = 10;
    unsigned short fight_is_over = 0;

    while (!fight_is_over) {
        json_warriors_ = NULL;
        printf("i");
        for (int i = 0; i < *warriors_number_; ++i) {
            json_current_warrior_actions_ = NULL;
            current_warrior_ = warriors_[i];

            size_t moves = current_warrior_->moves;
            size_t actions = current_warrior_->actions;

            size_t id = get_nearest_enemy(); // to delete when user script ready
            printf("\nNearest enemy: %zu\n", id);
            Warrior *enemy = (i == 0) ? warriors_[1] : warriors_[0]; // to delete when user script ready
            size_t distance = get_distance_between(current_warrior_->cell, enemy->cell); // to delete when user script ready
            // to delete when user script ready
            if (distance <= 3) {
                printf("a");
                equip_weapon(1);
                attack(enemy->id);
                //printf("\n enemy health %zu \n", enemy->health);
                move_away_from(enemy->id);
            }
            else {
                printf("b");
                printf("\nN\n");
                move_toward(enemy->id);

            }
            print_map(map_);
            // end of block to remove

            log_warrior(current_warrior_->name);

            reset_warrior_action_stats(current_warrior_, moves, actions);
        }

        log_round(current_round);

        if (++current_round > round_limit) {
            fight_is_over = 1;
        }
    }

   return log_fight();

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

void free_map(int **map)
{
    for (int i = 0; i < MAP_SIZE; ++i) {
        free(map[i]);
    }

    free(map);
}

int **generate_map(Warrior **warriors, size_t warriors_number)
{
    int **map = NULL;
    unsigned short is_valid_map;

    do {
        map = generate_random_map();
        locate_warriors_on_map(&map, warriors, warriors_number);

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

    return map;
}

void generate_walls_on_map(int ***map)
{
    size_t min = MAP_SIZE * MAP_SIZE * WALLS_MIN_RATIO;
    size_t max = MAP_SIZE * MAP_SIZE * WALLS_MAX_RATIO;
    size_t walls_number = rand() % (max - min + 1) + min;

    for (int i = 0; i < walls_number; ++i) {
        size_t random_x, random_y;

        do {
            random_x = rand() % MAP_SIZE;
            random_y = rand() % MAP_SIZE;
        } while ((*map)[random_x][random_y] == -1);

        (*map)[random_x][random_y] = -1;
    }
}

void locate_warriors_on_map(int *** map, Warrior **warriors, size_t warriors_number)
{
    size_t random_x, random_y;

    for (int i = 0; i < warriors_number; ++i) {
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

        (*map)[random_x][random_y] = warriors[i]->id;
        warriors[i]->cell = cell_init(random_x, random_y);
    }
}

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
    for (int i = area->min_x; i < area->max_x; ++i) {
        for (int j = area->min_y; j < area->max_y; ++j) {
            cell = cell_init(i, j);
            if (cell_is_obstacle(cell)) {
                cells_push_back(walls, cell);
            }
        }
    }

    return walls;
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
    printf("  id: %u\n", warrior->id);
    printf("  level: %u\n", warrior->level);
    printf("  health: %u\n", warrior->health);
    printf("  moves: %u\n", warrior->moves);
    printf("  actions: %u\n", warrior->actions);
}

void print_warriors(Warrior **warriors, size_t length)
{
    for (int i = 0; i < length; ++i) {
        print_warrior(warriors[i]);
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

cJSON *log_attack_action(size_t weapon_id)
{
    cJSON *json_action = cJSON_CreateObject();
    cJSON_AddStringToObject(json_action, "type", "attack");
    cJSON_AddNumberToObject(json_action, "weapon", weapon_id);

    return json_action;
}

void log_attack_action1(size_t weapon_id, size_t cost)
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

void log_weapon_drawing(size_t weapon_id)
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

cJSON *log_fight()
{
    cJSON *json_fight = cJSON_CreateObject();
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
    for (int i = 0; i < *warriors_number_; ++i) {
        Warrior *warrior = warriors_[i];
        if (warrior->id == id) {
            return warriors_[i];
        }
    }

    return NULL;
}

size_t get_warriors_number()
{
    return *warriors_number_;
}

Warrior **get_warriors()
{
    return warriors_;
}

Weapon **get_weapons()
{
    return weapons_;
}

void set_weapon (Weapon *w)
{
    current_warrior_->weapon = w;
}

size_t get_weapons_number()
{
    return *weapons_number_;
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