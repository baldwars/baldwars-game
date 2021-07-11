#ifndef BALDWARS_GAME_GAME_FUNCTION_H
#define BALDWARS_GAME_GAME_FUNCTION_H

#include <stdbool.h>
#include <stdlib.h>
#include <cJSON.h>
#include <utils.h>

#define MAP_SIZE 20
#define WALLS_MIN_RATIO 0.2
#define WALLS_MAX_RATIO 0.4

typedef struct cell_t Cell;
typedef struct cells_t Cells;
typedef struct weapon_t Weapon;
typedef struct warrior_t Warrior;
typedef struct area_t Area;

struct cell_t {
    size_t x;
    size_t y;
};

struct cells_t {
    Cell **items;
    size_t length;
    size_t capacity;
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

struct area_t {
    size_t min_x;
    size_t max_x;
    size_t min_y;
    size_t max_y;
};

// CELL
Cell *cell_init(size_t, size_t);
Cell *get_opposite_corner_from(Cell *);
unsigned short cell_is_obstacle(Cell *);
unsigned short cell_is_entity(Cell *);
Cells *cells_init();
Cells *cells_init_alloc(size_t);
void cells_check_alloc(Cells *);
void cells_push_back(Cells *, Cell *);
unsigned short cells_contains(Cells *, Cell *);

// WEAPON
Weapon *weapon_init(size_t, const char *, size_t , size_t , size_t , size_t );

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
cJSON *game_start();

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
void log_attack_action1(size_t, size_t);
void log_weapon_drawing(size_t);

// ACCESSORS
int **get_map();
Warrior **get_warriors();
size_t get_warriors_number();
Warrior *get_current_warrior();
Warrior *get_warrior_by_id(size_t);
size_t get_current_round();
Weapon **get_weapons();
size_t get_weapons_number();

// SEARCH
Nodes *a_star_algorithm(int **, Warrior *, Node *);


void initMap (int index);
void deleteMap (int index);
void setupMap (int index);
void loadScript();
void run ();

// AREA
Area *area_init(size_t, size_t, size_t, size_t);
Area *get_area_limits_between(Cell *, Cell *);
Cells *get_wall_in_area(Area *);

// ATTACK
size_t inRange (Warrior *current, Warrior *enemy);
size_t obstacleInSight (Warrior *current, Warrior *enemy, int** m);

void set_weapon (Weapon *w);

#endif //BALDWARS_GAME_GAME_FUNCTION_H
