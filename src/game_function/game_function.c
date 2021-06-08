//
// Created by Latif on 07/06/2021.
//

#include "game_function.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include "game_struct.h"
//#include "../json_file_management/cJSON.h"
//#include "../json_file_management/cJSON.c"
#include "../json_file_management/jsonActionLog.h"

const int smallTABSIZE = 25;
const int mediumTABSIZE = 50;
const int largeTABSIZE = 100;
const int smallOBSTACLE = 125;
const int mediumOBSTACLE = 500;
const int largeOBSTACLE = 2000;

const long blockSize = 24;
long count = 20;
//char buffer[blockSize];
int turn = 0;
int nbPlayers = 2;
Character **players;
Character *currentPlayer;
int **map;

const int TABSIZES[3] = {smallTABSIZE, mediumTABSIZE, largeTABSIZE};
const int OBSTACLES[3] = {smallOBSTACLE, mediumOBSTACLE, largeOBSTACLE};

void initMap (int index) {
    map = (int**)malloc(TABSIZES[index] * sizeof(int));
    printf("non \n");
    for (int i = 0; i < TABSIZES[index]; i++) {
        printf("oui");
        map [i] = (int*) malloc(TABSIZES[index] * sizeof(int));
    }
}

void deleteMap (int index) {
    for (int i = 0; i < TABSIZES[index]; i++) {
        free (map[i]);
    }
    free(map);
}

void setupMap (int index) {

    for (int i = 0; i < TABSIZES[index]; i++) {
        for (int j = 0; j < TABSIZES[index]; j++) {
            map[i][j] = 0;
        }
    }

    int i, j;
    float nbObsCalc = 1/5 * (TABSIZES[index] * TABSIZES[index]);
    int nbObs = (int) nbObsCalc;

    int pose2 = TABSIZES[index] - nbObs;
    map[nbObs][nbObs] = 1;
    map[pose2][pose2] = 1;

    srand(NULL);
    for (int k = 0; k < OBSTACLES[index]; k++) {
        do {
            i = rand() % 25;
            j = rand() % 25;
        } while (map[i][j] != 0);
        map[i][j] = -1;
    }
}

void loadScript() {
    FILE *scriptFile;
    long read;
    char *path = "../script0.c";
    path[9] = (char) turn % nbPlayers;
    scriptFile = fopen(path, "r");
    if (scriptFile == NULL) {
        printf("error");
        exit(1);
    }
    //fread(buffer, blockSize, count, scriptFile);
    fclose(scriptFile);
}

void run () {
    currentPlayer = players[turn % nbPlayers];

    turn++;
}



bool inRange (Character *target) {
    int x, y;
    Weapon w = currentPlayer->weapon;
    x = abs(currentPlayer->xPosition - target->xPosition);
    y = abs(currentPlayer->yPosition - target->yPosition);
    if (x + y >= w.minRange && x + y <= w.maxRange) {
        printf("in range");
        return true;
    } else {
        printf("not in range");
        return false;
    }
}

bool linearXObstacleCheck (int a, int b) {
    if (a < b) {
        for (int j = a; j < b; j++) {
            if (map[currentPlayer->xPosition][j] == 1) {
                return true;
            }
        }
    }
    else if (a > b) {
        for (int j = b; j < a; j++) {
            if (map[currentPlayer->xPosition][j] == 1) {
                return true;
            }
        }
    }
    else {
        printf("you can't step on the other player");
        return false;
    }
}

bool linearYObstacleCheck (int a, int b) {
    if (a < b) {
        for (int i = a; i < b; i++) {
            if (map[i][currentPlayer->yPosition] == 1) {
                return true;
            }
        }
    }

    else {
        for (int i = b; i < a; i++) {
            if (map[i][currentPlayer->yPosition] == 1) {
                return true;
            }
        }
    }
}

bool obstacleInSight (Character *target) {

    if (currentPlayer->xPosition == target->xPosition) {
        linearXObstacleCheck(currentPlayer->yPosition, target->yPosition);
    }

    else if (currentPlayer->yPosition == target->yPosition) {
        linearYObstacleCheck(currentPlayer->xPosition, target->xPosition);
    }

    else {
        int absX = abs(currentPlayer->xPosition - target->xPosition);
        int absY = abs(currentPlayer->yPosition - target->yPosition);
        if (currentPlayer->xPosition < target->xPosition) {
            if (currentPlayer->yPosition < target->yPosition) {
                if (absX == absY) {
                    int j = currentPlayer->yPosition;
                    for (int i = currentPlayer->xPosition; i < target->xPosition; i++) {
                        if (map[i][j] == 1) {
                            return true;
                        }
                        j++;
                    }
                    return false;
                }
                else {
                    /*if () {
                        return true;
                    }
                    else {
                        return false;
                    }*/
                }
            }

            else  {
                for (int j = target->yPosition; j < currentPlayer->yPosition; j++) {
                    if (map[currentPlayer->xPosition][j] == 1) {
                        return true;
                    }
                }
            }

        }
    }
}

bool canAttack (Character *target) {
    bool range = inRange(target);
    bool obs = obstacleInSight(target);
    if (range && !obs) {
        printf("oui");
        return true;
    }
    else {
        printf("target can't be hit");
        return false;
    }
}

void attack (Character *target) {
    Weapon weapon = currentPlayer->weapon;
    if (!canAttack(target) || currentPlayer->actionPoint < weapon.cost) {
        printf("target can't be hit");
    }
    else {
        target->hp = target->hp - (weapon.damage - target->armor);
        printf("remaining hp %d", target->hp);
    }
}

void defend (cJSON *actions) {
    currentPlayer->armor = 5;
    logDefence(actions, currentPlayer);
}

void moveToEnemy (Character target) {


}

void moveFromEnemy (Character target) {

}

void writeJSON (char *string) {
    FILE *jsonFile;
    jsonFile = fopen("../test.json", "w");
    if (jsonFile == NULL) {
        printf("error");
        exit(1);
    }
    fprintf(jsonFile, string);
    fclose(jsonFile);
}