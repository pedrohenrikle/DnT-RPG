#include <stdio.h>
#include <ncurses.h>
#include <string.h>
#include <stdlib.h>

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

// Executes an attack from one character to another with class-specific modifiers.
void performAttack(Character *attacker, Character *defender) {
    int chance = rand() % 101;
    int damage = 0;
    
    // Determine the HP colors for the attacker and defender.
    const int attackerHPColor = getHPColorPair(attacker->hp, 100);
    const int defenderHPColor = getHPColorPair(defender->hp, 100);

    // Display attack header with bold names and colored HP.
    attron(A_BOLD);
    printw(">> %s", attacker->className);
    attroff(A_BOLD);

    printw(" (");

    attron(COLOR_PAIR(attackerHPColor));
    printw("HP: %d", attacker->hp);
    attroff(COLOR_PAIR(attackerHPColor));

    printw(") attacks ");

    attron(A_BOLD);
    printw("%s", defender->className);
    attroff(A_BOLD);

    printw(" (");

    attron(COLOR_PAIR(defenderHPColor));
    printw("HP: %d", defender->hp);
    attroff(COLOR_PAIR(defenderHPColor));

    printw(")\n");
    
    // Barbarian never misses.
    if (attacker->characterClass == Barbarian) {
        printw("-> A Barbarian never misses an attack!\n");
        damage = attacker->attack;
    } else {
        // 20% chance to completely miss.
        if (chance < 20) {
            printw("-> Oh no! %s missed the attack on %s...\n\n", 
                   attacker->className, defender->className);
            return;
        }
        // 20% chance that the defender fails to block.
        if (chance < 40) {
            printw("-> %s failed to defend against %s's attack!\n",
                   defender->className, attacker->className);
            damage = attacker->attack;  // Full damage, ignoring defense.
        } else {
            damage = attacker->attack - defender->defense;
            if (damage < 0)
                damage = 0;
        }
        
        // Apply special abilities based on the attacker's class.
        switch (attacker->characterClass) {
            case Warrior:
                if (rand() % 100 < attacker->specialPercentage) {
                    printw("-> Critical hit! %s deals double damage!\n", attacker->className);
                    damage *= 2;
                }
                break;
            case Mage:
                if (rand() % 100 < attacker->specialPercentage) { 
                    printw("-> The mage doesn't care how small the room is. HE CAST FIREBALL! (Ignoring defenses...)\n");
                    damage = attacker->attack;  // Full damage, ignoring defense.
                }
                break;
            case Ranger:
                if (rand() % 100 < attacker->specialPercentage) { 
                    printw("-> Double Strike! %s gets a bonus attack!\n", attacker->className);
                    {
                        int extraDamage = attacker->attack - defender->defense;
                        if (extraDamage < 0)
                            extraDamage = 0;
                        damage += extraDamage;
                    }
                }
                break;
            default:
                break;
        }
    }
    
    // Paladin's passive: when defending, a chance to heal 20% of the incoming damage.
    if (defender->characterClass == Paladin) {
        if (rand() % 100 < defender->specialPercentage) {
            int heal = (int)(damage * 0.2);
            if (heal < 1) { heal = 1; }
            printw("-> May the sun shine upon me, my Godness! Paladin heals ");
            attron(COLOR_PAIR(1));
            printw("%d ", heal);
            attroff(COLOR_PAIR(1));
            printw("HP\n");
            defender->hp += heal;
        }
    }
    
    if (damage < 0)
        damage = 0;
    
    defender->hp -= damage;
    if (defender->hp < 0)
        defender->hp = 0;
    
    // Show attack result.
    printw("-> %s attacks %s for %d damage! %s now has %d HP left.\n\n",
           attacker->className,
           defender->className,
           damage,
           defender->className,
           defender->hp);
}