#include "path_finding.h"

HashTable *breadth_first_search(Graph graph, Node *start, Node *goal)
{
    Nodes *frontier_queue = nodes_init();
    nodes_push_back(frontier_queue, start);
    HashTable *came_from = hash_table_init();
    hash_table_set_entry(came_from, start, NULL);

    while (!is_empty(frontier_queue))
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
            if (hash_table_get_entry_by_key(came_from, neighbor) == NULL)
            {
                nodes_push_back(frontier_queue, neighbor);
                hash_table_set_entry(came_from, neighbor, current);
            }
        }
    }

    return came_from;
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
