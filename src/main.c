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

    printf("\nPaths obtained from { 0, 0 }:\n");
    Node *start = graph.nodes->items[0];
    Node *goal = graph.nodes->items[13];
    HashTable *ht = breadth_first_search(graph, start, goal);
    print_hash_table(ht);

    return 0;
}
