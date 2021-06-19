#ifndef BALDWARS_GAME_GAME_FUNCTION_H
#define BALDWARS_GAME_GAME_FUNCTION_H

#include <stdbool.h>
#include <stdlib.h>
#include <cJSON.h>
#include "game_struct.h"

#define MAP_SIZE 20
#define WALLS_MIN_RATIO 0.2
#define WALLS_MAX_RATIO 0.4

typedef struct cell_t Cell;
typedef struct weapon_t Weapon_;
typedef struct warrior_t Warrior;

struct cell_t {
    size_t x;
    size_t y;
};

struct weapon_t {
    char *name;
    int damage;
    int cost;
    int minRange;
    int maxRange;
};

struct warrior_t {
    unsigned short id;
    const char *name;
    size_t level;
    size_t health;
    size_t moves;
    size_t actions;
    Weapon_ *weapon;
    Cell *cell;
};

// INIT
Cell *cell_init(size_t, size_t);
Weapon_ *weapon_init();
Warrior *warrior_init(unsigned short, const char *, size_t, size_t, size_t, size_t);
int **map_init();

void game_start();
int **generate_random_map();
void generate_walls_on_map(int ***);
void locate_warriors_on_map(int ***, Warrior **, size_t);
Warrior *load_warrior(cJSON *);
Warrior **load_warriors(size_t *);

void initMap (int index);
void deleteMap (int index);
void setupMap (int index);
void loadScript();
void run ();
bool inRange (Character *target);

// DISPLAY
void print_map(int **);
void print_warrior(Warrior *);
void print_warriors(Warrior **, size_t);

// ACCESSORS
int **get_map();
Warrior **get_warriors();
Warrior *get_current_warrior();
size_t get_current_round();


#endif //BALDWARS_GAME_GAME_FUNCTION_H
