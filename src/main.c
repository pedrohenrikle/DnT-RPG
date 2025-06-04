#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include <classes.h>

#include <ncurses.h>

#define BAR_WIDTH 20

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
        drawHealthBar(party[i].className, party[i].hp, 100, BAR_WIDTH);
    }
    printw("\n");
}

// Returns a pointer to a randomly chosen character from the party with HP > 0.
Character* getRandomAliveCharacter(Character party[], int partySize, int isAttacking) {
    int aliveIndices[partySize];
    int count = 0;

    float bestWeight = -1.0f;
    int bestAlivePos = -1;  // This will be the index in aliveIndices array for the best candidate.

    for (int i = 0; i < partySize; i++) {
         if (party[i].hp > 0) {
            aliveIndices[count] = i;
            
            float weigth = (float)party[i].hp / (float)party[i].attack;
            if (weigth > bestWeight) {
                bestWeight = weigth;
                bestAlivePos = count;
            }

            count++;
         }
    }
    
    if (count == 0)
         return NULL;
    
    int randomIndex = rand() % count;

    if (isAttacking == 1) {
        return &party[aliveIndices[bestAlivePos]];
    } else {
        return &party[aliveIndices[randomIndex]];
    }

    return NULL;
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
    init_pair(3, COLOR_RED, COLOR_BLACK);    // Low 
    
    // Instantiate characters.
    Character warrior, mage, ranger, paladin, barbarian;

    warrior   = createCharacter(Warrior, 100, 20, 10, 20);
    mage      = createCharacter(Mage,      100, 30, 5,  25);
    ranger    = createCharacter(Ranger,    100, 18, 8,  15);
    paladin   = createCharacter(Paladin,   100, 15, 12, 30);
    barbarian = createCharacter(Barbarian, 100, 25, 6,  100);
    
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
                Character *attackerOne = getRandomAliveCharacter(partyOne, partySize, 1);
                Character *targetTwo   = getRandomAliveCharacter(partyTwo, partySize, 0);
                if (attackerOne && targetTwo) {
                    printw("-= Party One's turn =-\n");
                    performAttack(attackerOne, targetTwo);
                }
            }
            if (isPartyDefeated(partyTwo, partySize))
                break;
            {
                Character *attackerTwo = getRandomAliveCharacter(partyTwo, partySize, 1);
                Character *targetOne   = getRandomAliveCharacter(partyOne, partySize, 0);
                if (attackerTwo && targetOne) {
                    printw("-= Party Two's turn =-\n");
                    performAttack(attackerTwo, targetOne);
                }
            }
        } else {
            {
                Character *attackerTwo = getRandomAliveCharacter(partyTwo, partySize, 1);
                Character *targetOne   = getRandomAliveCharacter(partyOne, partySize, 0);
                if (attackerTwo && targetOne) {
                    printw("-= Party Two's turn =-\n");
                    performAttack(attackerTwo, targetOne);
                }
            }
            if (isPartyDefeated(partyOne, partySize))
                break;
            {
                Character *attackerOne = getRandomAliveCharacter(partyOne, partySize, 1);
                Character *targetTwo   = getRandomAliveCharacter(partyTwo, partySize, 0);
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
        round++;

        getch();
    }

    clear();

    
    // Announce the result.
    if (isPartyDefeated(partyOne, partySize)) {
        printw(">>> Party One has been defeated! Party Two wins!\n");
    }
    else if (isPartyDefeated(partyTwo, partySize)) {
        printw(">>> Party Two has been defeated! Party One wins!\n");
    }
    else {
        printw(">>>It's a draw!\n");
    }
    refresh();
    getch();

    endwin();
    
    return 0;
}