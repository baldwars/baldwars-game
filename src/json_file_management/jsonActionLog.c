#include "cJSON.h"
#include "jsonActionLog.h"

void logMovement (cJSON *actions, Character player) {
    cJSON *movementAction = cJSON_CreateObject();
    cJSON_AddItemToObject(actions, "movement log", movementAction);
    cJSON *remainingMP = cJSON_CreateNumber(player.movementPoint);
    cJSON_AddItemToObject(movementAction, "Remaining movement points", remainingMP);
    cJSON *newXPosition = cJSON_CreateNumber(player.xPosition);
    cJSON_AddItemToObject(movementAction, "Player new x position", newXPosition);
    cJSON *newYPosition = cJSON_CreateNumber(player.yPosition);
    cJSON_AddItemToObject(movementAction, "Player new x position", newYPosition);
    cJSON *newFacing = cJSON_CreateString(player.facing);
    cJSON_AddItemToObject(movementAction, "Player is now facing", newFacing);
}

void logAttack (cJSON *actions, int damage, Character target) {
    cJSON *attackAction = cJSON_CreateObject();
    cJSON_AddItemToObject(actions, "attack log", attackAction);
    cJSON *damageInflicted = cJSON_CreateNumber(damage);
    cJSON_AddItemToObject(attackAction, "Damages inflicted", damageInflicted);
    cJSON *targetRemainingLife = cJSON_CreateNumber(target.hp);
    cJSON_AddItemToObject(attackAction, "Target remaining life", targetRemainingLife);
}

void logDefence (cJSON *actions, Character *player) {
    cJSON *defenceAction = cJSON_CreateObject();
    cJSON_AddItemToObject(actions, "defence log", defenceAction);
    cJSON *armor = cJSON_CreateNumber(player->armor);
    cJSON_AddItemToObject(defenceAction, "Player new armor amount", armor);

}


