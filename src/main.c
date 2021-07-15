#include <time.h>
#include <stdio.h>
#include "game/game.h"

int main() {
    srand(time(NULL));

    cJSON *json_fight = game_start();
    char *json = cJSON_Print(json_fight);
    printf("json: %s\n", json);

    return 0;
}
