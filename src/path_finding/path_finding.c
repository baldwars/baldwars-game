#include <stdio.h>
#include "path_finding.h"

HashTable *breadth_first_search(Graph graph, Node *start, Node *goal)
{
    Nodes *frontier_queue = nodes_init();
    nodes_push_back(frontier_queue, start);
    HashTable *came_from = hash_table_init();
    hash_table_set_entry(came_from, start, NULL);

    while (!nodes_is_empty(frontier_queue))
    {
        Node *current = nodes_dequeue(frontier_queue);
        if (nodes_are_equals(current, goal))
        {
            break;
        }

        Nodes *neighbors = neighbors_of(current, graph.nodes, DIRECTION_NO_DIAGONALS);
        for (int i = 0; i < neighbors->length; ++i)
        {
            Node *neighbor = neighbors->items[i];
            if (hash_table_get_entry_value_by_key(came_from, neighbor) == NULL)
            {
                nodes_push_back(frontier_queue, neighbor);
                hash_table_set_entry(came_from, neighbor, current);
            }
        }
    }

    return came_from;
}

void dijkstra_search(Graph graph, Node *start, Node *goal, HashTable *came_from, HashTable *cost_so_far)
{
    PriorityQueue *frontier = priority_queue_init();
    priority_queue_enqueue(frontier, pq_item_init(start, 0));

    int priority_start = 0;
    hash_table_set_entry(came_from, start, NULL);
    hash_table_set_entry(cost_so_far, start, &priority_start);

    while (!priority_queue_is_empty(frontier))
    {
        Node *current = priority_queue_dequeue(frontier);

        if (nodes_are_equals(current, goal))
        {
            break;
        }

        Nodes *neighbors = neighbors_of(current, graph.nodes, DIRECTION_NO_DIAGONALS);
        for (int i = 0; i < neighbors->length; ++i) {
            Node *neighbor = neighbors->items[i];
            int *current_cost = (int *)hash_table_get_entry_value_by_key(cost_so_far, current);
            int *neighbor_cost = (int *)hash_table_get_entry_value_by_key(cost_so_far, neighbor);
            int new_cost = *current_cost + 1;

            if (neighbor_cost == NULL || new_cost < *neighbor_cost)
            {
                hash_table_set_entry(cost_so_far, neighbor, &new_cost);
                hash_table_set_entry(came_from, neighbor, current);
                priority_queue_enqueue(frontier, pq_item_init(neighbor, new_cost));
            }
        }
    }
}

void a_star_search(Graph graph, Node *start, Node *goal, HashTable *came_from, HashTable *cost_so_far)
{
    PriorityQueue *frontier = priority_queue_init();
    priority_queue_enqueue(frontier, pq_item_init(start, 0));

    int priority_start = 0;
    hash_table_set_entry(came_from, start, start);
    hash_table_set_entry(cost_so_far, start, &priority_start);


    while (!priority_queue_is_empty(frontier))
    {
        Node *current = priority_queue_dequeue(frontier);

        if (nodes_are_equals(current, goal))
        {
            break;
        }

        int *current_cost = (int *)hash_table_get_entry_value_by_key(cost_so_far, current);
        Nodes *neighbors = neighbors_of(current, graph.nodes, DIRECTION_NO_DIAGONALS);
        for (int i = 0; i < neighbors->length; ++i) {
            Node *neighbor = neighbors->items[i];
            int *neighbor_cost = (int *)hash_table_get_entry_value_by_key(cost_so_far, neighbor);
            int new_cost = *current_cost + 1;

            if (neighbor_cost == NULL || new_cost < *neighbor_cost)
            {
                hash_table_set_entry(cost_so_far, neighbor, &new_cost);
                int priority = new_cost + heuristic(neighbor, goal);
                priority_queue_enqueue(frontier, pq_item_init(neighbor, priority));
                hash_table_set_entry(came_from, neighbor, current);
            }
        }
    }
}

Nodes *reconstruct_path(HashTable *came_from, Node *start, Node *goal)
{
    Nodes *path = nodes_init();
    Node *current = goal;
    while (!nodes_are_equals(current, start))
    {
        nodes_push_back(path, current);
        current = (Node *)hash_table_get_entry_value_by_key(came_from, current);
    }

    nodes_push_back(path, start);

    return nodes_reverse(path);
}

int heuristic(Node *a, Node *b)
{
    return abs(a->x - b->x) + abs(a->y - b->y);
}

Nodes *neighbors_of(Node *origin, Nodes *nodes, unsigned short include_diagonals)
{
    Directions *directions = get_directions(include_diagonals);
    Nodes *neighbors = nodes_init();

    for (int i = 0; i < directions->length; ++i)
    {
        int x = origin->x + directions->items[i]->x;
        int y = origin->y + directions->items[i]->y;

        int neighbor_index = nodes_index_of(nodes, x, y);

        if (neighbor_index == -1)
            continue;

        Node *neighbor = nodes->items[neighbor_index];

        if (nodes_includes(nodes, neighbor) && !neighbor->is_obstacle)
        {
            nodes_push_back(neighbors, neighbor);
        }
    }

    return neighbors;
}

Directions *get_directions(unsigned short include_diagonals)
{
    unsigned short direction_possibilities = include_diagonals == 0 ? 4 : 8;
    Directions *directions = directions_init();

    for (int i = 0; i < direction_possibilities; ++i) {
        Direction *direction;
        switch (i) {
            case 0:
                direction = direction_init(-1, 0); // left
                break;
            case 1:
                direction = direction_init(0, 1); // up
                break;
            case 2:
                direction = direction_init(1, 0); // right
                break;
            case 3:
                direction = direction_init(0, -1); // down
                break;
            case 4:
                direction = direction_init(-1, 1); // left up
                break;
            case 5:
                direction = direction_init(1, 1); // right up
                break;
            case 6:
                direction = direction_init(-1, -1); // left down
                break;
            case 7:
                direction = direction_init(1, -1); // right down
                break;
            default:
                return NULL;
        }
        directions_push_back(directions, direction);
    }

    return directions;
}

Direction *direction_init(short x, short y)
{
    Direction *direction = malloc(sizeof(Direction));
    *direction = (Direction){
            .x = x,
            .y = y };

    return direction;
}

Directions *directions_init_alloc(size_t capacity)
{
    Directions *directions = malloc(sizeof(Directions));
    *directions = (Directions){
            .length = 0,
            .capacity = capacity,
            .items = malloc(sizeof(Direction *) * capacity)
    };

    return directions;
}

Directions *directions_init()
{
    return directions_init_alloc(1);
}

void directions_check_alloc(Directions *directions)
{
    if (directions->length >= directions->capacity)
    {
        directions->capacity += (directions->capacity < CAPACITY_LIMIT) ? directions->capacity : CAPACITY_LIMIT;
        directions->items = realloc(directions->items, (sizeof(Direction *) * directions->capacity));
    }
}

void directions_push_back(Directions *directions, Direction *value)
{
    directions_check_alloc(directions);
    directions->items[directions->length++] = value;
}
