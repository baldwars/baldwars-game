#include <time.h>
#include <stdio.h>
#include "game.h"

int main() {
    srand(time(NULL));

    cJSON *json_fight = game_start();
    char *json = cJSON_Print(json_fight);
    printf("%s\n", json);

    return 0;
}
