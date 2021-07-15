#ifndef BALDWARS_GAME_GAME_FUNCTION_H
#define BALDWARS_GAME_GAME_FUNCTION_H

#include <stdbool.h>
#include <stdlib.h>
#include <cJSON.h>
#include <utils.h>

#define MAP_SIZE 20
#define WALLS_MIN_RATIO 0.2
#define WALLS_MAX_RATIO 0.3

#define CORNERS_NUMBER 4
#define TOP_LEFT 0
#define TOP_RIGHT 1
#define DOWN_LEFT 2

typedef struct cell_t Cell;
typedef struct cells_t Cells;
typedef struct weapon_t Weapon;
typedef struct weapons_t Weapons;
typedef struct warrior_t Warrior;
typedef struct warriors_t Warriors;
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
    size_t level;
    size_t cost;
    size_t min_range;
    size_t max_range;
};

struct weapons_t {
    Weapon **items;
    size_t length;
    size_t capacity;
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

struct warriors_t {
    Warrior **items;
    size_t length;
    size_t capacity;
};

struct area_t {
    size_t min_x;
    size_t max_x;
    size_t min_y;
    size_t max_y;
};

// AREA
Area *area_init(size_t, size_t, size_t, size_t);
Area *get_area_limits_between(Cell *, Cell *);
Cells *get_wall_in_area(Area *);

// CELL
Cell *cell_init(size_t, size_t);
Cells *cells_init();
Cells *cells_init_alloc(size_t);
void cells_check_alloc(Cells *);
void cells_push_back(Cells *, Cell *);
unsigned short cells_contains(Cells *, Cell *);
Cells *get_diagonal_cells_from(Cell *, Cell *);
Cell *get_opposite_corner_from(Cell *);
Cell *get_corner(size_t);
unsigned short cell_is_obstacle(Cell *);
unsigned short cell_is_entity(Cell *);
Cell *get_direction_between(Cell *, Cell *);
unsigned short are_horizontally_aligned(Cell *, Cell *);
unsigned short are_vertically_aligned(Cell *, Cell *);
unsigned short are_diagonally_aligned(Cell *, Cell *);
unsigned short are_symmetrical_from(Cell *, Cell *, Cell *);
size_t get_distance_between(Cell *, Cell *);

// WEAPON
Weapon *weapon_init(size_t, const char *, size_t, size_t, size_t, size_t, size_t);
Weapon *get_weapon_by_id(size_t);
Weapon *load_weapon(cJSON *);
Weapons *load_weapons();
Weapons *weapons_init();
Weapons *weapons_init_alloc(size_t);
void weapons_check_alloc(Weapons *);
void weapons_push_back(Weapons *, Weapon *);
unsigned short is_in_weapon_range(Warrior *, Cell *);

// WARRIOR
Warrior *warrior_init(unsigned short, const char *, size_t, size_t, size_t, size_t);
Warrior *load_warrior(cJSON *);
Warriors *load_warriors();
Warriors *warriors_init();
Warriors *warriors_init_alloc(size_t);
void warriors_check_alloc(Warriors *);
void warriors_push_back(Warriors *, Warrior *);
void reset_warrior_action_stats(Warrior *, size_t, size_t);
Warrior *get_winner(Warriors *);

// MAP
int **map_init();
int **generate_map(Warriors *);
int **generate_random_map();
void generate_walls_on_map(int ***);
void no_walls_in_corners(int ***);
void locate_warriors_on_map(int ***, Warriors *);
unsigned short map_is_valid(int **);
void free_map(int **);
void update_map(int **, Warrior *, Node *);

// WALLS
unsigned short has_wall_as_neighbor(Cell *, int **);
Nodes *get_walls_of(Cell *, int **);
unsigned short is_wall_between(Cell *, Cell *, Cell *);
unsigned short is_wall_vertically_between(Cell *,Cell *, Cell *);
unsigned short is_wall_horizontally_between(Cell *,Cell *, Cell *);
unsigned short is_wall_diagonally_between(Cell *,Cell *, Cell *);

// GAME
cJSON *game_start();

// DISPLAY
void print_map(int **);
void print_warrior(Warrior *);
void print_warriors(Warriors *);
void print_cells(Cells *);

// JSON
void log_movement(Cell *, cJSON **);
void log_movements_action(cJSON *);
void log_attack_action(size_t, size_t);
void log_equip_weapon_action(size_t);
void log_rounds(cJSON *);
void log_round(size_t);
void log_warriors(cJSON *);
void log_warrior(const char *);
void log_warrior_action(cJSON *);
void log_winner(Warrior *, cJSON *);
cJSON *log_fight(Warrior *);

// ACCESSORS
int **get_map();
Warriors *get_warriors();
Warrior *get_current_warrior();
Warrior *get_warrior_by_id(size_t);

// SEARCH
Nodes *a_star_algorithm(int **, Warrior *, Node *);


#endif //BALDWARS_GAME_GAME_FUNCTION_H
