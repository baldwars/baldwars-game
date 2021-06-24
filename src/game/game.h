#ifndef BALDWARS_GAME_GAME_FUNCTION_H
#define BALDWARS_GAME_GAME_FUNCTION_H

#include <stdbool.h>
#include <stdlib.h>
#include <cJSON.h>
#include <utils.h>
#include "game_struct.h"

#define MAP_SIZE 20
#define WALLS_MIN_RATIO 0.2
#define WALLS_MAX_RATIO 0.4

typedef struct cell_t Cell;
typedef struct weapon_t Weapon;
typedef struct warrior_t Warrior;

struct cell_t {
    size_t x;
    size_t y;
};

struct weapon_t {
    size_t id;
    const char *name;
    size_t damage;
    size_t cost;
    size_t min_range;
    size_t max_range;
};

struct warrior_t {
    unsigned short id;
    const char *name;
    size_t level;
    size_t health;
    size_t moves;
    size_t actions;
    Weapon *weapon;
    Cell *cell;
};

// CELL
Cell *cell_init(size_t, size_t);
Cell *get_opposite_corner_from(Cell *);
unsigned short cell_is_obstacle(Cell *);
unsigned short cell_is_entity(Cell *);

// WEAPON
Weapon *weapon_init(size_t, const char *, size_t, size_t, size_t, size_t);
Weapon *get_weapon_by_id(size_t);
Weapon *load_weapon(cJSON *);
Weapon **load_weapons(size_t *);

// WARRIOR
Warrior *warrior_init(unsigned short, const char *, size_t, size_t, size_t, size_t);
Warrior *load_warrior(cJSON *);
Warrior **load_warriors(size_t *);
void reset_warrior_action_stats(Warrior *, size_t, size_t);

// MAP
int **map_init();
int **generate_map(Warrior **, size_t);
int **generate_random_map();
void generate_walls_on_map(int ***);
void locate_warriors_on_map(int ***, Warrior **, size_t);
unsigned short map_is_valid(int **);
void free_map(int **);
void update_map(int **, Warrior *, Node *);

// GAME
void game_start();

// DISPLAY
void print_map(int **);
void print_warrior(Warrior *);
void print_warriors(Warrior **, size_t);

// JSON
void log_movement(Cell *, cJSON **);
void log_movements_action(cJSON *);
cJSON *log_attack(size_t);
cJSON *log_attacks_action(cJSON *);
cJSON *log_fight();
void log_rounds(cJSON *);
void log_round(size_t);
void log_warriors(cJSON *);
void log_warrior(const char *);
void log_warrior_action(cJSON *);

// ACCESSORS
int **get_map();
Warrior **get_warriors();
size_t get_warriors_number();
Warrior *get_current_warrior();
Warrior *get_warrior_by_id(size_t);
size_t get_current_round();

// SEARCH
Nodes *a_star_algorithm(int **, Warrior *, Node *);


void initMap (int index);
void deleteMap (int index);
void setupMap (int index);
void loadScript();
void run ();
bool inRange (Character *target);


#endif //BALDWARS_GAME_GAME_FUNCTION_H
