#ifndef UNTITLED_PATH_FINDING_H
#define UNTITLED_PATH_FINDING_H

#include <stdlib.h>
#include "utils.h"

#define DIRECTION_NO_DIAGONALS 0
#define DIRECTION_WITH_DIAGONALS 1
#define NO_DIAGONALS_NEIGHBORS_LENGTH 4
#define WITH_DIAGONALS_NEIGHBORS_LENGTH 8


typedef struct direction_t Direction;
typedef struct directions_t Directions;

struct direction_t {
    short x;
    short y;
};

struct directions_t {
    Direction **items;
    size_t length;
    size_t capacity;
};

int heuristic(Node *, Node *);
Nodes *reconstruct_path(HashTable *, Node *, Node *);

HashTable *breadth_first_search(Nodes *, Node *, Node *);
void dijkstra_search(Nodes *, Node *, Node *, HashTable *, HashTable *);
void a_star_search(Nodes *, Node *, Node *, HashTable *, HashTable *);

Nodes *neighbors_of(Node *, Nodes *, unsigned short);
Directions *get_directions(unsigned short);
Direction *direction_init(short, short);
Directions *directions_init_alloc(size_t);
Directions *directions_init();
void directions_check_alloc(Directions *);
void directions_push_back(Directions *, Direction *);

#endif //UNTITLED_PATH_FINDING_H
