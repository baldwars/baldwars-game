#ifndef BALDWARS_GAME_GAME_STRUCT_H
#define BALDWARS_GAME_GAME_STRUCT_H

typedef struct Weapon Weapon;
struct Weapon {
    char *name;
    int damage;
    int cost;
    int minRange;
    int maxRange;
};

typedef struct Character Character;
struct Character {
    char *name;
    int level;
    int hp;
    int armor;
    Weapon weapon;
    int movementPoint;
    int actionPoint;
    int xPosition;
    int yPosition;
    char *facing;
};


#endif //BALDWARS_GAME_GAME_STRUCT_H
