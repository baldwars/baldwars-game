#ifndef BALDWARS_GAME_GAME_FUNCTION_H
#define BALDWARS_GAME_GAME_FUNCTION_H

#include <stdbool.h>
#include <stdlib.h>
#include "game_struct.h"

#define MAP_SIZE 20

typedef struct cell_t Cell;
typedef struct weapon_t Weapon_;
typedef struct warrior_t Warrior;

struct cell_t {
    unsigned int x;
    unsigned int y;
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
    unsigned int level;
    unsigned int health;
    unsigned int moves;
    unsigned int actions;
    Weapon_ *weapon;
};

// INIT
Cell *cell_init(int, int);
Weapon_ *weapon_init();
Warrior *warrior_init(unsigned short, const char *, unsigned int, unsigned int, unsigned int, unsigned int);
int **map_init();

void game_start();
int **generate_random_map();
void generate_walls_on_map(int ***);
void locate_warriors_on_map(int ***, Warrior **, size_t);
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


#endif //BALDWARS_GAME_GAME_FUNCTION_H
