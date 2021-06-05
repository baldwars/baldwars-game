#ifndef UNTITLED_UTILS_H
#define UNTITLED_UTILS_H

#include <stdlib.h>

#define CAPACITY_LIMIT 2000
#define HASH_TABLE_CAPACITY 10000

typedef struct node_t Node;
typedef struct nodes_t Nodes;
typedef struct edge_t Edge;
typedef struct edges_t Edges;
typedef struct graph_t Graph;
typedef struct entry_t Entry;
typedef struct hash_table_t HashTable;

struct node_t {
    int x;
    int y;
    unsigned short is_obstacle;
    unsigned short is_entity;
};

struct nodes_t {
    Node **items;
    size_t length;
    size_t capacity;
};

struct edge_t {
    Node from;
    Node to;
    size_t weight;
};

struct edges_t {
    Edge *items;
    size_t length;
    size_t capacity;
};

struct graph_t {
    Nodes *nodes;
    Edges edges;
};

struct entry_t {
    Node *key;
    Node *value;
    Entry *next;
};

struct hash_table_t {
    Entry **entries;
} ;

Edge edge_init(Node, Node, size_t);
Edges edges_init_alloc(size_t);
Edges edges_init();
void edges_check_alloc(Edges *);
void edges_push_back(Edges *, Edge);
Node *node_init(int, int, unsigned short, unsigned short);
Nodes *nodes_init_alloc(size_t);
Nodes *nodes_init();
void nodes_check_alloc(Nodes *);
void nodes_push_back(Nodes *, Node *);
unsigned short nodes_are_equals(Node *, Node *);
int nodes_includes(Nodes *, Node *);
Graph graph_init_alloc(size_t, size_t);
Graph graph_init();
int nodes_index_of(Nodes *, int, int);
Nodes *convert_grid_to_nodes(int **, size_t, size_t);
void print_nodes(Nodes *);

unsigned short is_empty(Nodes *);
Node *nodes_dequeue(Nodes *);
unsigned int hash_node(Node *);
Entry *entry_init(Node *, Node *);
HashTable *hash_table_init();
void hash_table_set_entry(HashTable *, Node *, Node *);
Node *hash_table_get_entry_by_key(HashTable *, Node *);
void hash_table_delete_entry_by_key(HashTable *, Node *);
void print_hash_table(HashTable *);

#endif //UNTITLED_UTILS_H