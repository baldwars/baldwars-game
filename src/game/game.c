#include "game.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include "../json_file_management/jsonActionLog.h"
#include "../utils/utils.h"

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
int **map;


const int TABSIZES[3] = {smallTABSIZE, mediumTABSIZE, largeTABSIZE};
const int OBSTACLES[3] = {smallOBSTACLE, mediumOBSTACLE, largeOBSTACLE};

void initMap (int index) {
    map = (int**)malloc(TABSIZES[index] * sizeof(int));
    printf("non \n");
    for (int i = 0; i < TABSIZES[index]; i++) {
        printf("oui");
        map [i] = (int*) malloc(TABSIZES[index] * sizeof(int));
    }
}

void deleteMap (int index) {
    for (int i = 0; i < TABSIZES[index]; i++) {
        free (map[i]);
    }
    free(map);
}

void setupMap (int index) {

    for (int i = 0; i < TABSIZES[index]; i++) {
        for (int j = 0; j < TABSIZES[index]; j++) {
            map[i][j] = 0;
        }
    }

    int i, j;
    float nbObsCalc = 1 / 5 * (TABSIZES[index] * TABSIZES[index]);
    int nbObs = (int) nbObsCalc;

    int pose2 = TABSIZES[index] - nbObs;
    map[nbObs][nbObs] = 1;
    map[pose2][pose2] = 1;

    srand(NULL);
    for (int k = 0; k < OBSTACLES[index]; k++) {
        do {
            i = rand() % 25;
            j = rand() % 25;
        } while (map[i][j] != 0);
        map[i][j] = -1;
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
            if (map[currentPlayer->xPosition][j] == 1) {
                return true;
            }
        }
    }
    else if (a > b) {
        for (int j = b; j < a; j++) {
            if (map[currentPlayer->xPosition][j] == 1) {
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
            if (map[i][currentPlayer->yPosition] == 1) {
                return true;
            }
        }
    }

    else {
        for (int i = b; i < a; i++) {
            if (map[i][currentPlayer->yPosition] == 1) {
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
                        if (map[i][j] == 1) {
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
                    if (map[currentPlayer->xPosition][j] == 1) {
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
        logAttack(actions, damage, *target);
    }
}

void defend () {
    currentPlayer->armor = 5;
    logDefence(actions, currentPlayer);
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

Cell *cell_init(int x, int y)
{
    Cell *cell = malloc(sizeof(Cell));
    *cell = (Cell){
        .x = x,
        .y = y
    };

    return cell;
}

Warrior *warrior_init(unsigned short id,const char *name, unsigned int level,
                          unsigned int health, unsigned int moves, unsigned int action)
{
    Warrior *warrior = malloc(sizeof(Warrior));
    *warrior = (Warrior){
        .id = id,
        .name = name,
        .level = level,
        .health = health,
        .actions = action,
        .moves = moves,
        .weapon = NULL
    };

    return warrior;
}

Warrior **load_warriors(size_t *warriors_length)
{
    char *buffer = get_file_content("./warriors.json");

    cJSON *parsed = cJSON_Parse(buffer);
    cJSON *item, *id, *name, *level, *health, *moves, *actions;

    *warriors_length = cJSON_GetArraySize(parsed);
    Warrior **warriors = malloc(sizeof(Warrior *) * (*warriors_length));

    for (int i = 0; i < *warriors_length; ++i)
    {
        item = cJSON_GetArrayItem(parsed, i);
        id = cJSON_GetObjectItemCaseSensitive(item, "id");
        name = cJSON_GetObjectItemCaseSensitive(item, "name");
        level = cJSON_GetObjectItemCaseSensitive(item, "level");
        health = cJSON_GetObjectItemCaseSensitive(item, "health");
        moves = cJSON_GetObjectItemCaseSensitive(item, "moves");
        actions = cJSON_GetObjectItemCaseSensitive(item, "actions");

        char *name_val = cJSON_GetStringValue(name);
        unsigned int id_val = (unsigned int)cJSON_GetNumberValue(id);
        unsigned int level_val = (unsigned int)cJSON_GetNumberValue(level);
        unsigned int health_val = (unsigned int)cJSON_GetNumberValue(health);
        unsigned int actions_val = (unsigned int)cJSON_GetNumberValue(actions);
        unsigned int moves_val = (unsigned int)cJSON_GetNumberValue(moves);

        warriors[i] = warrior_init(id_val, name_val, level_val, health_val, moves_val, actions_val);
    }
    return warriors;
}

void game_start()
{
    size_t warriors_number = 0;
    Warrior **warriors = load_warriors(&warriors_number);
    int **map = generate_random_map();
    locate_warriors_on_map(&map, warriors, warriors_number);
    print_map(map);

    unsigned int round = 1;
    unsigned short fight_is_over = 0;

    while (!fight_is_over) {
        printf("Round %u:\n", round);
        print_warriors(warriors, warriors_number);

        if (++round > 5) {
            fight_is_over = 1;
        }
    }
}

int **map_init()
{
    int **map = malloc(sizeof(int*) * MAP_SIZE);
    for (int i = 0; i < MAP_SIZE; ++i) {
        map[i] = malloc(sizeof(int) * MAP_SIZE);
    }

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
    size_t min = MAP_SIZE * MAP_SIZE * 0.2;
    size_t max = MAP_SIZE * MAP_SIZE * 0.4;
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
    printf("  actions: %u\n\n", warrior->actions);
}

void print_warriors(Warrior **warriors, size_t length)
{
    for (int i = 0; i < length; ++i) {
        print_warrior(warriors[i]);
    }
}
