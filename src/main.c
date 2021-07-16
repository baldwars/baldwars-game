#include <time.h>
#include <stdio.h>
#include "game/game.h"
#include "utils/utils.h"

int main() {
    srand((unsigned int)time(NULL));

    cJSON *json_fight = game_start();
    char *json = no_space(cJSON_Print(json_fight));
    
    printf("%s\n", json);

    return 0;
}
