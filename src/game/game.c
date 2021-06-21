#include "game.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include "../json_file_management/jsonActionLog.h"
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
Character **players;
Character *currentPlayer;

int **map_;
Warrior *current_warrior_;
Warrior **warriors_;

const int TABSIZES[3] = {smallTABSIZE, mediumTABSIZE, largeTABSIZE};
const int OBSTACLES[3] = {smallOBSTACLE, mediumOBSTACLE, largeOBSTACLE};

void initMap (int index) {
    map_ = (int**)malloc(TABSIZES[index] * sizeof(int));
    printf("non \n");
    for (int i = 0; i < TABSIZES[index]; i++) {
        printf("oui");
        map_ [i] = (int*) malloc(TABSIZES[index] * sizeof(int));
    }
}

void deleteMap (int index) {
    for (int i = 0; i < TABSIZES[index]; i++) {
        free (map_[i]);
    }
    free(map_);
}

void setupMap (int index) {

    for (int i = 0; i < TABSIZES[index]; i++) {
        for (int j = 0; j < TABSIZES[index]; j++) {
            map_[i][j] = 0;
        }
    }

    int i, j;
    float nbObsCalc = 1 / 5 * (TABSIZES[index] * TABSIZES[index]);
    int nbObs = (int) nbObsCalc;

    int pose2 = TABSIZES[index] - nbObs;
    map_[nbObs][nbObs] = 1;
    map_[pose2][pose2] = 1;

    for (int k = 0; k < OBSTACLES[index]; k++) {
        do {
            i = rand() % 25;
            j = rand() % 25;
        } while (map_[i][j] != 0);
        map_[i][j] = -1;
    }
}

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

void run () {
    currentPlayer = players[turn % nbPlayers];

    turn++;
}



bool inRange (Character *target) {
    int x, y;
    Weapon w = currentPlayer->weapon;
    x = abs(currentPlayer->xPosition - target->xPosition);
    y = abs(currentPlayer->yPosition - target->yPosition);
    if (x + y >= w.minRange && x + y <= w.maxRange) {
        printf("in range");
        return true;
    } else {
        printf("not in range");
        return false;
    }
}

bool linearXObstacleCheck (int a, int b) {
    if (a < b) {
        for (int j = a; j < b; j++) {
            if (map_[currentPlayer->xPosition][j] == 1) {
                return true;
            }
        }
    }
    else if (a > b) {
        for (int j = b; j < a; j++) {
            if (map_[currentPlayer->xPosition][j] == 1) {
                return true;
            }
        }
    }
    else {
        printf("you can't step on the other player");
        return false;
    }
}

bool linearYObstacleCheck (int a, int b) {
    if (a < b) {
        for (int i = a; i < b; i++) {
            if (map_[i][currentPlayer->yPosition] == 1) {
                return true;
            }
        }
    }

    else {
        for (int i = b; i < a; i++) {
            if (map_[i][currentPlayer->yPosition] == 1) {
                return true;
            }
        }
    }
}

bool obstacleInSight (Character *target) {

    if (currentPlayer->xPosition == target->xPosition) {
        linearXObstacleCheck(currentPlayer->yPosition, target->yPosition);
    }

    else if (currentPlayer->yPosition == target->yPosition) {
        linearYObstacleCheck(currentPlayer->xPosition, target->xPosition);
    }

    else {
        int absX = abs(currentPlayer->xPosition - target->xPosition);
        int absY = abs(currentPlayer->yPosition - target->yPosition);
        if (currentPlayer->xPosition < target->xPosition) {
            if (currentPlayer->yPosition < target->yPosition) {
                if (absX == absY) {
                    int j = currentPlayer->yPosition;
                    for (int i = currentPlayer->xPosition; i < target->xPosition; i++) {
                        if (map_[i][j] == 1) {
                            return true;
                        }
                        j++;
                    }
                    return false;
                }
                else {
                    /*if () {
                        return true;
                    }
                    else {
                        return false;
                    }*/
                }
            }

            else  {
                for (int j = target->yPosition; j < currentPlayer->yPosition; j++) {
                    if (map_[currentPlayer->xPosition][j] == 1) {
                        return true;
                    }
                }
            }

        }
    }
}

bool canAttack (Character *target) {
    bool range = inRange(target);
    bool obs = obstacleInSight(target);
    if (range && !obs) {
        printf("oui");
        return true;
    }
    else {
        printf("target can't be hit");
        return false;
    }
}

void attack (Character *target) {
    Weapon weapon = currentPlayer->weapon;
    if (!canAttack(target) || currentPlayer->actionPoint < weapon.cost) {
        printf("target can't be hit");
    }
    else {
        int damage = weapon.damage - target->armor;
        target->hp = target->hp - (damage);
        printf("remaining hp %d", target->hp);
        logAttack(actions_, damage, *target);
    }
}

void defend () {
    currentPlayer->armor = 5;
    logDefence(actions_, currentPlayer);
}

void moveToEnemy (Character target) {


}

void moveFromEnemy (Character target) {

}

void writeJSON (char *string) {
    FILE *jsonFile;
    jsonFile = fopen("../test.json", "w");
    if (jsonFile == NULL) {
        printf("error");
        exit(1);
    }
    fprintf(jsonFile, string);
    fclose(jsonFile);
}

Cell *cell_init(size_t x, size_t y)
{
    Cell *cell = malloc(sizeof(Cell));
    *cell = (Cell){
        .x = x,
        .y = y
    };

    return cell;
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

void game_start()
{
    size_t warriors_number = 0;
    Warrior **warriors = load_warriors(&warriors_number);

    map_ = generate_map(warriors, warriors_number);
    print_map(map_);

    size_t round = 1;
    unsigned short fight_is_over = 0;

    current_warrior_ = warriors[0];
    move_toward(warriors[1]->cell);
    current_warrior_->moves = 3;
    move_toward(warriors[1]->cell);
    cJSON *json_warrior = log_warrior(current_warrior_->name);
    log_warriors(json_warrior);

//    current_warrior_ = warriors[1];
//    move_toward(warriors[0]->cell);
//    json_warrior = log_warrior(current_warrior_->name);
//    log_warriors(json_warrior);

    char *json = cJSON_Print(json_warriors_);
    printf("json: %s\n", json);

//    while (!fight_is_over) {
//        printf("Round %u:\n", round);
//        print_warriors(warriors, warriors_number);
//
//        if (++round > 5) {
//            fight_is_over = 1;
//        }
//    }
}

int **map_init()
{
    int **map = malloc(sizeof(int*) * MAP_SIZE);
    for (int i = 0; i < MAP_SIZE; ++i) {
        map[i] = malloc(sizeof(int) * MAP_SIZE);
    }

    return map;
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
cJSON *log_movement(Cell *cell)
{
    cJSON *json_cell = cJSON_CreateObject();
    cJSON_AddNumberToObject(json_cell, "x", cell->x);
    cJSON_AddNumberToObject(json_cell, "y", cell->y);

    return json_cell;
}

cJSON *log_movement_action(cJSON *json_path)
{
    cJSON *json_action = cJSON_CreateObject();
    cJSON_AddStringToObject(json_action, "type", "move");
    cJSON_AddItemToObject(json_action, "path", json_path);

    return json_action;
}

cJSON *log_attack_action(size_t weapon_id)
{
    cJSON *json_action = cJSON_CreateObject();
    cJSON_AddStringToObject(json_action, "type", "attack");
    cJSON_AddNumberToObject(json_action, "path", weapon_id);

    return json_action;
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

Warrior **get_warriors()
{
    return warriors_;
}