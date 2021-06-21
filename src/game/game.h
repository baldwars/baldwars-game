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

// CELL
Cell *cell_init(size_t, size_t);

// WEAPON
Weapon_ *weapon_init();

// WARRIOR
Warrior *warrior_init(unsigned short, const char *, size_t, size_t, size_t, size_t);
Warrior *load_warrior(cJSON *);
Warrior **load_warriors(size_t *);

// MAP
int **map_init();
int **generate_map(Warrior **, size_t);
int **generate_random_map();
void generate_walls_on_map(int ***);
void locate_warriors_on_map(int ***, Warrior **, size_t);
unsigned short map_is_valid(int **);
void free_map(int **);

// GAME
void game_start();

// DISPLAY
void print_map(int **);
void print_warrior(Warrior *);
void print_warriors(Warrior **, size_t);

// JSON
cJSON *log_movement(Cell *);
cJSON *log_movements_action(cJSON *);
cJSON *log_attack(size_t);
cJSON *log_attacks_action(cJSON *);
cJSON *log_fight(cJSON *);
cJSON *log_round(cJSON *);
void log_warriors(cJSON *);
cJSON *log_warrior(const char *);
void log_warrior_action(cJSON *);

// ACCESSORS
int **get_map();
Warrior **get_warriors();
Warrior *get_current_warrior();
size_t get_current_round();


void initMap (int index);
void deleteMap (int index);
void setupMap (int index);
void loadScript();
void run ();
bool inRange (Character *target);


#endif //BALDWARS_GAME_GAME_FUNCTION_H
