#include <stdio.h>
#include <stdlib.h>
#include "path_finding/path_finding.h"

int main() {
    // Create grid
    int **grid = malloc(sizeof(int *) * 5);
    for (int i = 0; i < 5; ++i) {
        grid[i] = malloc(sizeof(int) * 5);
    }
    // Fill grid
    for (int i = 0; i < 5; ++i) {
        for (int j = 0; j < 5; ++j) {
            if ((i == 0 && j == 1) || (i == 3 && j == 3))
            {
                grid[i][j] = 1;
            }
            else if ((i == 1 && j == 2) || (i == 2 && j == 2) || (i == 2 && j == 3))
            {
                grid[i][j] = -1;
            }
            else {
                grid[i][j] = 0;
            }
        }
    }
    // Display grid
    printf("grid:\n");
    for (int i = 0; i < 5; ++i) {
        for (int j = 0; j < 5; ++j) {
            printf("%d ", grid[i][j]);
        }
        printf("\n");
    }
    printf("\n");


    Graph graph = graph_init();
    graph.nodes = convert_grid_to_nodes(grid, 5, 5);
    print_nodes(graph.nodes);

    Node *start = graph.nodes->items[1]; // player
    Node *goal = graph.nodes->items[18]; // enemy

    HashTable *ht = breadth_first_search(graph, start, goal);
    printf("\nPath obtained from { 0, 1 } to { 3, 3 } with Breadth First:\n");
    print_nodes_hash_table(ht);

    HashTable *d_came_from = hash_table_init();
    HashTable *d_cost_so_far = hash_table_init();

    dijkstra_search(graph, start, goal, d_came_from, d_cost_so_far);
    Nodes *d_path = reconstruct_path(d_came_from, start, goal);

    printf("\nPath obtained from { 0, 1 } to { 3, 3 } with Dijkstra:\n");
    print_nodes(d_path);

    HashTable *a_came_from = hash_table_init();
    HashTable *a_cost_so_far = hash_table_init();

    a_star_search(graph, start, goal, a_came_from, a_cost_so_far);
    Nodes *a_path = reconstruct_path(a_came_from, start, goal);

    printf("\nPath obtained from { 0, 1 } to { 3, 3 } with A*:\n");
    print_nodes(a_path);

    return 0;
}
