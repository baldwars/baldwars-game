#include <stdio.h>
#include "utils.h"

Edge edge_init(Node from, Node to, size_t weight)
{
    Edge *edge = malloc(sizeof(Edge));
    *edge = (Edge){
            .from = from,
            .to = to,
            .weight = weight
    };

    return *edge;
}

Edges edges_init_alloc(size_t capacity)
{
    Edges *edges = malloc(sizeof(Edges));
    *edges = (Edges){
            .length = 0,
            .capacity = capacity,
            .items = malloc(sizeof(Edge) * capacity)
    };

    return *edges;
}

Edges edges_init()
{
    return edges_init_alloc(1);
}

void edges_check_alloc(Edges *edges)
{
    if (edges->length >= edges->capacity)
    {
        edges->capacity += (edges->capacity < CAPACITY_LIMIT) ? edges->capacity : CAPACITY_LIMIT;
        edges->items = realloc(edges->items, (sizeof(Edge *) * edges->capacity));
    }
}

void edges_push_back(Edges *edges, Edge value)
{
    edges_check_alloc(edges);
    edges->items[edges->length++] = value;
}

Node *node_init(int x, int y, unsigned short is_obstacle, unsigned short is_entity)
{
    Node *node = malloc(sizeof(Node));
    *node = (Node){
            .x = x,
            .y = y,
            .is_obstacle = is_obstacle,
            .is_entity = is_entity };

    return node;
}

Nodes *nodes_init_alloc(size_t capacity)
{
    Nodes *nodes = malloc(sizeof(Nodes));
    *nodes = (Nodes){
            .length = 0,
            .capacity = capacity,
            .items = malloc(sizeof(Node *) * capacity)
    };

    return nodes;
}

Nodes *nodes_init()
{
    return nodes_init_alloc(1);
}

void nodes_check_alloc(Nodes *nodes)
{
    if (nodes->length >= nodes->capacity)
    {
        nodes-> capacity += (nodes->capacity < CAPACITY_LIMIT) ? nodes->capacity : CAPACITY_LIMIT;
        nodes->items = realloc(nodes->items, (sizeof(Node *) * nodes->capacity));
    }
}

void nodes_push_back(Nodes *nodes, Node *value)
{
    nodes_check_alloc(nodes);
    nodes->items[nodes->length++] = value;
}

unsigned short nodes_are_equals(Node *a, Node *b)
{
    return (a->x == b->x) && (a->y == b->y);
}

int nodes_includes(Nodes *nodes, Node *value)
{
    for (int i = 0; i < nodes->length; ++i) {
        Node *node = nodes->items[i];

        if (nodes_are_equals(node, value))
            return 1;
    }
    return 0;
}

Graph graph_init_alloc(size_t nodes_capacity, size_t edges_capacity)
{
    Graph *graph = malloc(sizeof(Graph));
    *graph = (Graph){
            .nodes = nodes_init_alloc(nodes_capacity),
            .edges = edges_init_alloc(edges_capacity)
    };

    return *graph;
}

Graph graph_init()
{
    return graph_init_alloc(1, 1);
}

int nodes_index_of(Nodes *nodes, int x, int y)
{
    for (int i = 0; i < nodes->length; ++i) {
        if (nodes->items[i]->x == x && nodes->items[i]->y == y)
            return i;
    }

    return -1;
}

Nodes *convert_grid_to_nodes(int **grid, size_t rows, size_t columns)
{
    Nodes *nodes = nodes_init();
    for (int i = 0; i < rows; ++i) {
        for (int j = 0; j < columns; ++j) {
            Node *node;
            if (grid[i][j] == 0) {
                node = node_init(i, j, 0, 0);
            }
            else if (grid[i][j] <= -1)
            {
                node = node_init(i, j, 1, 0);
            }
            else
            {
                node = node_init(i, j, 0, 1);
            }

            nodes_push_back(nodes, node);
        }
    }

    return nodes;
}

void print_nodes(Nodes *nodes)
{
    printf("graph nodes: \n[\n");
    for (int i = 0; i < nodes->length; ++i) {
        Node *node = nodes->items[i];
        printf("  { x: %d, y: %d , is_obstacle: %d, is_entity: %d }\n", node->x, node->y, node->is_obstacle, node->is_entity);
    }
    printf("]\n");
}

unsigned short is_empty(Nodes *nodes)
{
    return nodes->length == 0;
}

Node *nodes_dequeue(Nodes *nodes)
{
    if (is_empty(nodes)) {
        return NULL;
    }

    Node *first = nodes->items[0];
    nodes->length--;

    for (int i = 0; i < nodes->length; ++i)
        nodes->items[i] = nodes->items[i + 1];

    return first;
}

unsigned int hash_node(Node *node)
{
    unsigned int hash = (node->y << 16) ^ node->x;
    hash %= HASH_TABLE_CAPACITY;

    return hash;
}

Entry *entry_init(Node *key, Node *value)
{
    Entry *entry = malloc(sizeof(Entry));
    entry->key = key;
    entry->value = value;

    entry->next = NULL;

    return entry;
}

HashTable *hash_table_init()
{
    HashTable *hash_table = malloc(sizeof(HashTable));
    hash_table->entries = malloc(sizeof(Entry *) * HASH_TABLE_CAPACITY);

    for (int i = 0; i < HASH_TABLE_CAPACITY; ++i) {
        hash_table->entries[i] = NULL;
    }

    return hash_table;
}

void hash_table_set_entry(HashTable *hashtable, Node *key, Node *value)
{
    unsigned int slot = hash_node(key);
    Entry *entry = hashtable->entries[slot];

    if (entry == NULL) {
        hashtable->entries[slot] = entry_init(key, value);
        return;
    }

    Entry *prev;

    while (entry != NULL) {
        if (nodes_are_equals(entry->key, key))
        {
            entry->value = value;
            return;
        }
        prev = entry;
        entry = prev->next;
    }
    prev->next = entry_init(key, value);
}

Node *hash_table_get_entry_by_key(HashTable *hashtable, Node *key)
{
    unsigned int slot = hash_node(key);
    Entry *entry = hashtable->entries[slot];

    if (entry == NULL) {
        return NULL;
    }

    while (entry != NULL) {
        if (nodes_are_equals(entry->key, key)) {
            return entry->value;
        }
        entry = entry->next;
    }
    return NULL;
}

void hash_table_delete_entry_by_key(HashTable *hashtable, Node *key)
{
    unsigned int bucket = hash_node(key);
    Entry *entry = hashtable->entries[bucket];

    if (entry == NULL) {
        return;
    }

    Entry *prev;
    int idx = 0;

    while (entry != NULL) {
        if (nodes_are_equals(entry->key, key)) {
            if (entry->next == NULL && idx == 0) {
                hashtable->entries[bucket] = NULL;
            }

            if (entry->next != NULL && idx == 0) {
                hashtable->entries[bucket] = entry->next;
            }

            if (entry->next == NULL && idx != 0) {
                prev->next = NULL;
            }

            if (entry->next != NULL && idx != 0) {
                prev->next = entry->next;
            }

            free(entry->key);
            free(entry->value);
            free(entry);

            return;
        }

        prev = entry;
        entry = prev->next;
        ++idx;
    }
}

void print_hash_table(HashTable *hash_table)
{
    for (int i = 0; i < HASH_TABLE_CAPACITY; ++i) {
        Entry *entry = hash_table->entries[i];

        if (entry == NULL) {
            continue;
        }

        printf("slot[%d]: ", i);

        for(;;) {
            if (entry->value == NULL) {
                printf("{ %d, %d, %d, %d } = NULL", entry->key->x, entry->key->y, entry->key->is_obstacle, entry->key->is_entity);
            } else {
                printf("{ %d, %d, %d, %d } = { %d, %d, %hu, %hu } ", entry->key->x, entry->key->y, entry->key->is_obstacle, entry->key->is_entity, entry->value->x, entry->value->y, entry->value->is_obstacle, entry->value->is_entity);
            }
            if (entry->next == NULL) {
                break;
            }

            entry = entry->next;
        }

        printf("\n");
    }
}

PQItem *pq_item_init(Node *value, int priority)
{
    PQItem *item = malloc(sizeof(PQItem));

    if (item == NULL) return NULL;

    *item = (PQItem){
        .value = value,
        .priority = priority
    };

    return item;
}

PriorityQueue *priority_queue_init()
{
    return priority_queue_init_alloc(1);
}

PriorityQueue *priority_queue_init_alloc(size_t capacity)
{
    PriorityQueue *queue = malloc(sizeof(PriorityQueue));
    *queue = (PriorityQueue){
        .length = 0,
        .capacity = capacity,
        .items = malloc(sizeof(PQItem *))
    };
    return queue;
}

void priority_queue_check_alloc(PriorityQueue *queue)
{
    if (queue->length >= queue->capacity)
    {
        queue-> capacity += (queue->capacity < CAPACITY_LIMIT) ? queue->capacity : CAPACITY_LIMIT;
        queue->items = realloc(queue->items, (sizeof(PQItem *) * queue->capacity));
    }
}

size_t priority_queue_peek(PriorityQueue *queue)
{
    int highest_priority = INT_MIN;
    int index = -1;

    for (int i = 0; i <= queue->length; ++i) {
        if ((highest_priority == queue->items[i]->priority
            && index >= 0
            && hash_node(queue->items[index]->value) > hash_node(queue->items[i]->value))
            || (highest_priority < queue->items[i]->priority))
        {
            highest_priority = queue->items[i]->priority;
            index = i;
        }
    }

    return index;
}

void priority_queue_enqueue(PriorityQueue *queue, PQItem *value)
{
    priority_queue_check_alloc(queue);
    queue->items[queue->length++] = value;
}

Node *priority_queue_dequeue(PriorityQueue *queue)
{
    size_t index = priority_queue_peek(queue);
    Node *node = queue->items[index]->value;

    for (size_t i = index; i < queue->length - 1; ++i)
    {
        queue->items[i] = queue->items[i + 1];
    }

    queue->length--;

    return node;
}
