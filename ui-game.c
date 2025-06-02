#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include <ncurses.h>

#define BAR_WIDTH 20

// Define possible classes and an array to map them to strings.
typedef enum { Warrior, Mage, Ranger, Paladin, Barbarian } Class;
const char *classNames[] = { "Warrior", "Mage", "Ranger", "Paladin", "Barbarian" };

typedef struct {
    Class characterClass;
    int hp;
    int attack;
    int defense;
    int specialPercentage; // Chance (in percentage) to trigger the special.
} Character;

// Returns a color string based on the HP percentage.
// Assumes maxHp is 100.
int getHPColorPair(int hp, int maxHp) {
    // For example, if hp is above 50% of maxHp, use green.
    if (hp * 2 >= maxHp)
        return 1;  // Green color pair.
    // If above roughly 33%, use yellow.
    else if (hp * 3 >= maxHp)
        return 2;  // Yellow color pair.
    else
        return 3;  // Red color pair.
}

// Draws a health bar for a character given its current HP and a fixed max HP (e.g., 100).
void drawHealthBar(const char *name, int hp, int maxHp) {
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
    printw(">> %s", classNames[attacker->characterClass]);
    attroff(A_BOLD);

    printw(" (");

    attron(COLOR_PAIR(attackerHPColor));
    printw("HP: %d", attacker->hp);
    attroff(COLOR_PAIR(attackerHPColor));

    printw(") attacks ");

    attron(A_BOLD);
    printw("%s", classNames[defender->characterClass]);
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
                   classNames[attacker->characterClass], classNames[defender->characterClass]);
            return;
        }
        // 20% chance that the defender fails to block.
        if (chance < 40) {
            printw("-> %s failed to defend against %s's attack!\n",
                   classNames[defender->characterClass], classNames[attacker->characterClass]);
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
                    printw("-> Critical hit! %s deals double damage!\n", classNames[Warrior]);
                    damage *= 2;
                }
                break;
            case Mage:
                if (rand() % 100 < attacker->specialPercentage) { 
                    printw("-> The mage doesn't care how small the room is. HE CAST FIREBALL! (Ignoring defenses...)\n");
                    damage -= defender->defense;
                }
                break;
            case Ranger:
                if (rand() % 100 < attacker->specialPercentage) { 
                    printw("-> Double Strike! %s gets a bonus attack!\n", classNames[Ranger]);
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
            printw("-> May the sun shine upon me, my Godness! Paladin heals %d heal", heal);
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
           classNames[attacker->characterClass],
           classNames[defender->characterClass],
           damage,
           classNames[defender->characterClass],
           defender->hp);
}

// Returns 1 if every character in the party has 0 HP; 0 otherwise.
int isPartyDefeated(Character party[], int partySize) {
    for (int i = 0; i < partySize; i++) {
        if (party[i].hp > 0)
            return 0;
    }
    return 1;
}

// Prints the current status of a party using health bars.
void printPartyStatus(const char *partyName, Character party[], int partySize) {
    printw("> %s\n", partyName);
    for (int i = 0; i < partySize; i++) {
        drawHealthBar(classNames[party[i].characterClass], party[i].hp, 100);
    }
    printw("\n");
}

// Returns a pointer to a randomly chosen character from the party with HP > 0.
Character* getRandomAliveCharacter(Character party[], int partySize) {
    int aliveIndices[partySize];
    int count = 0;
    
    for (int i = 0; i < partySize; i++) {
         if (party[i].hp > 0)
              aliveIndices[count++] = i;
    }
    
    if (count == 0)
         return NULL;
    
    int randomIndex = rand() % count;
    return &party[aliveIndices[randomIndex]];
}

int main(void) {
    srand(time(NULL));  /* Seed the random number generator. */
    initscr();			/* Start the ncursor module */
    cbreak();		    /* Disable buffer  */
    noecho();           /* Disable echo, preventing double inputs */
    curs_set( false );  /* Disable cursor */
    start_color();      /* Start the colors module */

    /* Define the colors to specific integers */    
    init_pair(1, COLOR_GREEN, COLOR_BLACK);  // High HP
    init_pair(2, COLOR_YELLOW, COLOR_BLACK); // Medium HP
    init_pair(3, COLOR_RED, COLOR_BLACK);    // Low HP
    
    // Instantiate characters.
    Character warrior   = {Warrior,   100, 20, 10, 20};
    Character mage      = {Mage,      100, 30, 5,  25};
    Character ranger    = {Ranger,    100, 18, 8,  15};
    Character paladin   = {Paladin,   100, 15, 12, 30};
    Character barbarian = {Barbarian, 100, 25, 6,  100};
    
    // Create two parties.
    Character partyOne[5] = { warrior, mage, ranger, paladin, barbarian };
    Character partyTwo[5] = { warrior, mage, ranger, paladin, barbarian };
    int partySize = sizeof partyOne / sizeof partyOne[0];

    
    // Randomly decide which party starts the combat.
    int startingParty = rand() % 2;  // 0 for Party One, 1 for Party Two.
    printw("Randomly drawing the starting party...\n");
    if (startingParty == 0)
         printw("Party One will start the combat.\n\n");
    else
         printw("Party Two will start the combat.\n\n");
    
    int round = 1;

    getch();
    
    // Main battle loop: each round both parties attack, starting with the chosen party.
    while (!isPartyDefeated(partyOne, partySize) && !isPartyDefeated(partyTwo, partySize)) {
        clear(); /* Starts cleaning the terminal*/

        printw(">>> Round %d:\n\n", round);
        
        if (startingParty == 0) {
            {
                Character *attackerOne = getRandomAliveCharacter(partyOne, partySize);
                Character *targetTwo   = getRandomAliveCharacter(partyTwo, partySize);
                if (attackerOne && targetTwo) {
                    printw("-= Party One's turn =-\n");
                    performAttack(attackerOne, targetTwo);
                }
            }
            if (isPartyDefeated(partyTwo, partySize))
                break;
            {
                Character *attackerTwo = getRandomAliveCharacter(partyTwo, partySize);
                Character *targetOne   = getRandomAliveCharacter(partyOne, partySize);
                if (attackerTwo && targetOne) {
                    printw("-= Party Two's turn =-\n");
                    performAttack(attackerTwo, targetOne);
                }
            }
        } else {
            {
                Character *attackerTwo = getRandomAliveCharacter(partyTwo, partySize);
                Character *targetOne   = getRandomAliveCharacter(partyOne, partySize);
                if (attackerTwo && targetOne) {
                    printw("-= Party Two's turn =-\n");
                    performAttack(attackerTwo, targetOne);
                }
            }
            if (isPartyDefeated(partyOne, partySize))
                break;
            {
                Character *attackerOne = getRandomAliveCharacter(partyOne, partySize);
                Character *targetTwo   = getRandomAliveCharacter(partyTwo, partySize);
                if (attackerOne && targetTwo) {
                    printw("-= Party One's turn =-\n");
                    performAttack(attackerOne, targetTwo);
                }
            }
        }
        
        // Print health bars of both parties.
        printw(">>> Status after round %d:\n", round);
        printPartyStatus("Party One", partyOne, partySize);
        printPartyStatus("Party Two", partyTwo, partySize);
        printw("-------------------------------------------------\n\n");
        round++;

        getch();
    }
    
    // Announce the result.
    if (isPartyDefeated(partyOne, partySize)) {
        printw("Party One has been defeated! Party Two wins!\n");
    }
    else if (isPartyDefeated(partyTwo, partySize)) {
        printw("Party Two has been defeated! Party One wins!\n");
    }
    else {
        printw("It's a draw!\n");
    }
    
    endwin();
    
    return 0;
}