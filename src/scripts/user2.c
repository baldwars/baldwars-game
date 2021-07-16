#include "scripts.h"
#include <stdio.h>

void run_script_user2()
{
    printf("ok1\n");
    set_weapon(WEAPON_PISTOL); // Cost 1 action point
    printf("ok2\n");

    size_t enemy = get_nearest_enemy();
    printf("ok3\n");

    use_weapon(enemy);
    printf("ok4\n");

    move_away_from(enemy);
    printf("ok5\n");
    
}
