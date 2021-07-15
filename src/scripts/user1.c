#include "scripts.h"

void run_script_user1()
{
    if (get_weapon() == 0) {
        set_weapon(WEAPON_PISTOL); // Cost 1 action point
    }

    size_t enemy = get_nearest_enemy();
    move_toward(enemy);

    while (can_use_weapon(enemy)) {
        use_weapon(enemy);
    }
}
