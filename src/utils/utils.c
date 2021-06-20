#include <stdio.h>
#include "utils.h"

Node *node_init(size_t x, size_t y, unsigned short is_obstacle, unsigned short is_entity)
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
    return (a->x == b->x)
        && (a->y == b->y)
        && (a->is_obstacle == b->is_obstacle)
        && (a->is_entity == b->is_entity);
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

Nodes *nodes_reverse(Nodes *nodes)
{
    Nodes *reversed = nodes_init_alloc(nodes->capacity);
    size_t last_index = nodes->length - 1;

    for (int i = (int)last_index; i >= 0; --i) {
        nodes_push_back(reversed, nodes->items[i]);
    }

    return reversed;
}

int nodes_index_of(Nodes *nodes, size_t x, size_t y)
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
    printf("nodes: \n[\n");
    for (int i = 0; i < nodes->length; ++i) {
        Node *node = nodes->items[i];
        printf("  { x: %d, y: %d , is_obstacle: %d, is_entity: %d }\n",
               node->x, node->y, node->is_obstacle, node->is_entity);
    }
    printf("]\n");
}

unsigned short nodes_is_empty(Nodes *nodes)
{
    return nodes->length == 0;
}

Node *nodes_dequeue(Nodes *nodes)
{
    if (nodes_is_empty(nodes)) {
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

Entry *entry_init(Node *key, void *value)
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

void hash_table_set_entry(HashTable *hashtable, Node *key, void *value)
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

void *hash_table_get_entry_value_by_key(HashTable *hashtable, Node *key)
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

void print_nodes_hash_table(HashTable *hash_table)
{
    for (int i = 0; i < HASH_TABLE_CAPACITY; ++i) {
        Entry *entry = hash_table->entries[i];

        if (entry == NULL) {
            continue;
        }

        printf("slot[%4d]: ", i);

        for(;;)
        {
            Node *key = entry->key;
            if (entry->value == NULL)
            {
                printf("{ %d, %d, %d, %d } = NULL", key->x, key->y, key->is_obstacle, key->is_entity);
            }
            else
            {
                Node *value = (Node *)entry->value;
                printf("{ %d, %d, %d, %d } = { %d, %d, %hu, %hu } ",
                       key->x, key->y, key->is_obstacle, key->is_entity,
                       value->x, value->y, value->is_obstacle, value->is_entity);
            }

            if (entry->next == NULL)
            {
                break;
            }

            entry = entry->next;
        }
        printf("\n");
    }
}

void print_int_hash_table(HashTable *hash_table)
{
    for (int i = 0; i < HASH_TABLE_CAPACITY; ++i) {
        Entry *entry = hash_table->entries[i];

        if (entry == NULL) {
            continue;
        }

        printf("slot[%4d]: ", i);

        for(;;)
        {
            Node *key = entry->key;
            if (entry->value == NULL)
            {
                printf("{ %d, %d, %d, %d } = NULL", key->x, key->y, key->is_obstacle, key->is_entity);
            }
            else
            {
                int *value = (int *)entry->value;
                printf("{ %d, %d, %d, %d } = %d\n",
                       key->x, key->y, key->is_obstacle, key->is_entity, *value);
            }

            if (entry->next == NULL)
            {
                break;
            }

            entry = entry->next;
        }
        printf("\n");
    }
}

PQItem *pq_item_init(Node *value, size_t priority)
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

int priority_queue_peek(PriorityQueue *queue)
{
    size_t lowest_priority = INT_MAX;
    int index = -1;

    for (int i = 0; i < queue->length; ++i) {
        PQItem *item_i = queue->items[i];
        if ((lowest_priority == item_i->priority && index >= 0) || (lowest_priority > item_i->priority))
        {
            lowest_priority = queue->items[i]->priority;
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

unsigned short priority_queue_is_empty(PriorityQueue *queue)
{
    return queue->length == 0;
}

char *get_file_content(char * path)
{
    FILE *fp;

    char *buffer = calloc(CAPACITY_LIMIT, sizeof(char));

    fp = fopen(path,"r");
    fread(buffer, CAPACITY_LIMIT, 1, fp);
    fclose(fp);

    return buffer;
}