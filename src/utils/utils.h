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
typedef struct priority_queue_item_t PQItem;
typedef struct priority_queue_t PriorityQueue;

struct node_t {
    size_t x;
    size_t y;
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
    void *value;
    Entry *next;
};

struct hash_table_t {
    Entry **entries;

} ;

struct priority_queue_item_t {
    Node *value;
    size_t priority;
};


struct priority_queue_t {
    PQItem **items;
    size_t length;
    size_t capacity;
};

// NODES
Node *node_init(size_t, size_t, unsigned short, unsigned short);
Nodes *nodes_init_alloc(size_t);
Nodes *nodes_init();
void nodes_check_alloc(Nodes *);
void nodes_push_back(Nodes *, Node *);
unsigned short nodes_are_equals(Node *, Node *);
int nodes_includes(Nodes *, Node *);
int nodes_index_of(Nodes *, size_t, size_t);
void print_nodes(Nodes *);
unsigned short nodes_is_empty(Nodes *);
Nodes *nodes_reverse(Nodes *);


// UTIL
Nodes *convert_grid_to_nodes(int **, size_t, size_t);
size_t hash_node(Node *);

// HASH TABLE
Entry *entry_init(Node *, void *);
HashTable *hash_table_init();
void hash_table_set_entry(HashTable *, Node *, void *);
void *hash_table_get_entry_value_by_key(HashTable *, Node *);
void hash_table_delete_entry_by_key(HashTable *, Node *);
void print_nodes_hash_table(HashTable *);
void print_int_hash_table(HashTable *);

// QUEUE
Node *nodes_dequeue(Nodes *);

// PRIORITY QUEUE

PQItem *pq_item_init(Node *, size_t);
PriorityQueue *priority_queue_init();
PriorityQueue *priority_queue_init_alloc(size_t);
void priority_queue_check_alloc(PriorityQueue *);
int priority_queue_peek(PriorityQueue *);
void priority_queue_enqueue(PriorityQueue *, PQItem *);
Node *priority_queue_dequeue(PriorityQueue *);
unsigned short priority_queue_is_empty(PriorityQueue *);


// FILE
char *get_file_content(char * path);

#endif //UNTITLED_UTILS_H
