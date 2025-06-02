#include <stdio.h>
#include <ncurses.h>
#include <string.h>

#include "classes.h"

// Array for mapping class names
const char *classNames[] = {
    "Warrior",
    "Mage",
    "Ranger",
    "Paladin",
    "Barbarian"
};

// Function to create a new character
Character createCharacter(Class classType, int hp, int attack, int defense, int specialChance) {
    Character newCharacter;
    newCharacter.characterClass = classType;
    newCharacter.hp = hp;
    newCharacter.attack = attack;
    newCharacter.defense = defense;
    newCharacter.specialPercentage = specialChance;
    strncpy(newCharacter.className, classNames[classType], sizeof(newCharacter.className) - 1);
    newCharacter.className[sizeof(newCharacter.className) - 1] = '\0';  // Null-terminate    
    return newCharacter;
}


int getHPColorPair(int hp, int maxHp) {
    // If hp is above 50% of maxHp, use green.
    if (hp * 2 >= maxHp)
        return 1;  // Green color pair.
    // If above roughly 33%, use yellow.
    else if (hp * 3 >= maxHp)
        return 2;  // Yellow color pair.
    else
        return 3;  // Red color pair.
}

// Draws a health bar for a character given its current HP and a fixed max HP (e.g., 100).
void drawHealthBar(const char *name, int hp, int maxHp, int BAR_WIDTH) {
    int filled = (hp * BAR_WIDTH) / maxHp;
    
    // Print the character's name in bold.
    attron(A_BOLD);
    printw("%-10s", name);
    attroff(A_BOLD);
    
    printw(": [");

    // Get the appropriate color pair for the filled portion.
    int colorPair = getHPColorPair(hp, maxHp);
    attron(COLOR_PAIR(colorPair));
    for (int i = 0; i < filled; i++) {
        printw("#");
    }
    attroff(COLOR_PAIR(colorPair));
    
    // Draw the empty portion of the bar.
    for (int i = filled; i < BAR_WIDTH; i++) {
        printw(" ");
    }
    
    printw("] ");

    // Reprint the hp values in the same color as the filled portion.
    attron(COLOR_PAIR(colorPair));
    printw("%3d", hp);
    attroff(COLOR_PAIR(colorPair));
    
    printw("/%3d", maxHp);
    printw("\n");
}