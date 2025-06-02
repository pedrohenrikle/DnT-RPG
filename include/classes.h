#ifndef CLASSES_H
#define CLASSES_H

#define MAX_NAME_LENGTH 20

// Define possible character classes.
typedef enum {
    Warrior,
    Mage,
    Ranger,
    Paladin,
    Barbarian
} Class;

// Character structure
typedef struct {
    Class characterClass;
    char className[MAX_NAME_LENGTH];
    int hp;
    int attack;
    int defense;
    int specialPercentage;
} Character;

Character createCharacter(Class classType, int hp, int attack, int defense, int specialChance);

int getHPColorPair(int hp, int maxHp);  

void drawHealthBar(const char *name, int hp, int maxHp, int BAR_WIDTH);

void performAttack(Character *attacker, Character *defender);

#endif
