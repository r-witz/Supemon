#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "../include/display.h"
#include "../include/input.h"
#include "../include/player.h"

void stats_display(Player *ptrPlayer, Supemon *ptrEnemy);
int ask_move(Supemon *ptrSupemon);
int supemon_move(Player *ptrPlayer, Supemon *ptrEnemy, short moveID);
int enemy_move(Supemon *ptrSupemon, Supemon *ptrEnemy);
int change_supemon(Player *ptrPlayer);
int use_item(Player *ptrPlayer);
int capture(Player *ptrPlayer, Supemon *ptrEnemy);
int run_away(Supemon *ptrSupemon, Supemon *ptrEnemy);

void level_up(Supemon *ptrSupemon);
void win(Player *ptrPlayer, Supemon *ptrEnemy);
void reset_supemon_stats(Supemon *ptrSupemon);

void fight(Player *ptrPlayer) { 
    srand(time(NULL));

    Supemon *ptrEnemy = getSupemonByID(rand()%3+1);
    while (ptrEnemy->level < ptrPlayer->supemons[0]->level) {
        level_up(ptrEnemy);
    }

    char bufferStr[7];
    write(1, "You've stumbled against a lvl.", 30);
    sprintf(bufferStr, "%d", ptrEnemy->level);
    write(1, bufferStr, strlen(bufferStr));
    write(1, " ", 1);
    write(1, ptrEnemy->name, strlen(ptrEnemy->name));
    write(1, " !\n", 3);

    short playing;
    short choice;

    do { 
        stats_display(ptrPlayer, ptrEnemy);
        fight_menu();
        get_input("1, 2, 3, 4 or 5: ", &choice, 'i', 3);
        Supemon *ptrSupemon = ptrPlayer->supemons[0];
        
        switch (choice) {
            int moveID;
            case 1:
                moveID = ask_move(ptrSupemon);
                if (moveID != -1) {
                    if (ptrSupemon->speed > ptrEnemy->speed) {
                        playing = supemon_move(ptrPlayer, ptrEnemy, moveID);
                        if (playing == 0) {break;}
                        playing = enemy_move(ptrSupemon, ptrEnemy);
                    } else if (ptrSupemon->speed < ptrEnemy->speed) {
                        playing = enemy_move(ptrSupemon, ptrEnemy);
                        if (playing == 0) {break;}
                        playing = supemon_move(ptrPlayer, ptrEnemy, moveID);
                    } else {
                        short random = rand() % 2;
                        if (random == 0) {
                            playing = supemon_move(ptrPlayer, ptrEnemy, moveID);
                            if (playing == 0) {break;}
                            playing = enemy_move(ptrSupemon, ptrEnemy);
                        } else {
                            playing = enemy_move(ptrSupemon, ptrEnemy);
                            if (playing == 0) {break;}
                            playing = supemon_move(ptrPlayer, ptrEnemy, moveID);
                        }
                    }
                } else {
                    playing = 1;
                }
                break;
            case 2:
                if (change_supemon(ptrPlayer)) {
                    playing = enemy_move(ptrSupemon, ptrEnemy);
                } else {
                    playing = 1;
                }
                break;
            case 3:
                if (use_item(ptrPlayer)) {
                    playing = enemy_move(ptrSupemon, ptrEnemy);
                } else {
                    playing = 1;
                }
                break;
            case 4:
                playing = capture(ptrPlayer, ptrEnemy);
                if (playing == 0) {break;}
                playing = enemy_move(ptrSupemon, ptrEnemy);
                break;
            case 5:
                playing = run_away(ptrSupemon, ptrEnemy);
                if (playing == 0) {break;}
                playing = enemy_move(ptrSupemon, ptrEnemy);
                break;
            default:
                playing = 1;
                write(1, "Please enter a number between 1-5\n", 34);
        }
    } while(playing);

    reset_supemon_stats(ptrPlayer->supemons[0]);

    main_menu();
}

void stats_display(Player *ptrPlayer, Supemon *ptrEnemy) {
  char bufferStr[33];

  write(1, "--------------------------------\n", 33);
  write(1, ptrEnemy->name, strlen(ptrEnemy->name));
  write(1, " (enemy)\n", 9);
  sprintf(bufferStr, "HP: %d/%d \tLvl: %d\n", ptrEnemy->hp, ptrEnemy->max_hp, ptrEnemy->level);
  write(1, bufferStr, strlen(bufferStr));
  sprintf(bufferStr, "Atk: %d  \tDef: %d\n", ptrEnemy->attack, ptrEnemy->defense);
  write(1, bufferStr, strlen(bufferStr));
  sprintf(bufferStr, "Acc: %d  \tEva: %d\n", ptrEnemy->accuracy, ptrEnemy->evasion);
  write(1, bufferStr, strlen(bufferStr));
  write(1, "--------------------------------\n", 33);
  
  Supemon *ptrSupemon = ptrPlayer->supemons[0];
 
  write(1, ptrSupemon->name, strlen(ptrSupemon->name));
  write(1, " (", 2);
  write(1, ptrPlayer->name, strlen(ptrPlayer->name));
  write(1, ")\n", 2);
  sprintf(bufferStr, "HP: %d/%d \tLvl: %d\n", ptrSupemon->hp, ptrSupemon->max_hp, ptrSupemon->level);
  write(1, bufferStr, strlen(bufferStr));
  sprintf(bufferStr, "Atk: %d\t\tDef: %d\n", ptrSupemon->attack, ptrSupemon->defense);
  write(1, bufferStr, strlen(bufferStr));
  sprintf(bufferStr, "Acc: %d\t\tEva: %d\n", ptrSupemon->accuracy, ptrSupemon->evasion);
  write(1, bufferStr, strlen(bufferStr));
  write(1, "--------------------------------\n\n", 34);

}

int ask_move(Supemon *ptrSupemon) {
    for (short i=0; i<2; i++) {
        char bufferStr[7];
        sprintf(bufferStr, "%d", i+1);
        write(1, bufferStr, strlen(bufferStr));
        write(1, ". ", 2);
        write(1, ptrSupemon->moves[i]->name, strlen(ptrSupemon->moves[i]->name));
        write(1, "\n", 1);
    }
    write(1, "3. Cancel\n", 10);

    short choice;

    do {
        get_input("1, 2 or 3: ", &choice, 'i', 3);

        switch (choice) {
            case 1:
            case 2:
                return choice-1;
                break;
            case 3:
                write(1, "You canceled\n", 13);
                return -1;
                break;
            default:
                write(1, "Please enter a number between 1-3\n", 34);
        }

    } while (choice != 1 && choice != 2 && choice != 3);

    return -1;
}

int use_move(Supemon *ptrSupemon, Supemon *ptrEnemy, short moveID) {
    if (ptrSupemon->moves[moveID]->damage != 0) {
        float success_rate = ((float)ptrSupemon->accuracy / (ptrSupemon->accuracy + ptrEnemy->evasion) + 0.1)*100;
        short random_number = rand() % 100;

        if (random_number < success_rate) {
            float damage = ptrSupemon->attack * ptrSupemon->moves[moveID]->damage / (float)(ptrEnemy->defense);
            ptrEnemy->hp -= damage == (int)damage ? damage : (rand()%100 < 50) ? (int)damage : (int)(damage+1);
            write(1, "The attack hit ", 15);
            write(1, ptrEnemy->name, strlen(ptrEnemy->name));
            write(1, " successfully !\n", 16);
            
        } else {
            write(1, ptrEnemy->name, strlen(ptrEnemy->name));
            write(1, " has dodged the attack !\n", 25);
        }
        
    }
    ptrSupemon->attack += ptrSupemon->moves[moveID]->attack_bonus;
    ptrSupemon->defense += ptrSupemon->moves[moveID]->defense_bonus;
    ptrSupemon->evasion += ptrSupemon->moves[moveID]->evasion_bonus;

    if (ptrEnemy->hp <= 0) {
        ptrEnemy->hp = 0;
        write(1, ptrEnemy->name, strlen(ptrEnemy->name));
        write(1, " is K.O.\n", 9);
        return 0;
    }

    return 1;
}

int supemon_move(Player *ptrPlayer, Supemon *ptrEnemy, short moveID) {
    Supemon *ptrSupemon = ptrPlayer->supemons[0];
    write(1, "You use ", 8);
    write(1, ptrSupemon->moves[moveID]->name, strlen(ptrSupemon->moves[moveID]->name));
    write(1, " !\n", 3);
    short win_nb = use_move(ptrSupemon, ptrEnemy, moveID);
    if (win_nb == 0) {
        write(1, "You've won the battle !\n", 24);
        win(ptrPlayer, ptrEnemy);
        freeSupemon(ptrEnemy);
    }
    return win_nb;
}

int enemy_move(Supemon *ptrSupemon, Supemon *ptrEnemy) {
    short moveID = rand() % 2;
    write(1, ptrEnemy->name, strlen(ptrEnemy->name));
    write(1, " (enemy) uses ", 14);
    write(1, ptrEnemy->moves[moveID]->name, strlen(ptrEnemy->moves[moveID]->name));
    write(1, " !\n", 3);
    short win = use_move(ptrEnemy, ptrSupemon, moveID);
    if (win == 0) {freeSupemon(ptrEnemy);}
    return win;
}

int change_supemon(Player *ptrPlayer) {
    write(1, "+------------Your Team-----------+\n", 35);
    for (short i=0; i<6; i++) {
        char bufferStr[7];
        sprintf(bufferStr, "%d", i+1);
        write(1, bufferStr, strlen(bufferStr));
        write(1, ". ", 2);
        if (ptrPlayer->supemons[i] != 0) {
            write(1, ptrPlayer->supemons[i]->name, strlen(ptrPlayer->supemons[i]->name));
            write(1, " lvl.", 5);
            sprintf(bufferStr, "%d", ptrPlayer->supemons[i]->level);
            write(1, bufferStr, strlen(bufferStr));
            write(1, "\t(", 2);
            sprintf(bufferStr, "%d", ptrPlayer->supemons[i]->hp);
            write(1, bufferStr, strlen(bufferStr));
            write(1, "/", 1);
            sprintf(bufferStr, "%d", ptrPlayer->supemons[i]->max_hp);
            write(1, bufferStr, strlen(bufferStr));
            write(1, "HP)\n", 4);
        } else {
            write(1, "EMPTY\n", 6);
        }
    }
    write(1, "7. Cancel\n", 11);

    short choice;
    short valid_supemon = 0;
    
    do {
        get_input("1, 2, 3, 4, 5, 6 or 7: ", &choice, 'i', 3);
        
        switch (choice) {
            case 1:
                write(1, ptrPlayer->supemons[0]->name, strlen(ptrPlayer->supemons[0]->name));
                write(1, " is already fighting !\n", 23);
                valid_supemon = 0;
                break;
            case 2:
            case 3:
            case 4:
            case 5:
            case 6:
                if (ptrPlayer->supemons[choice-1] == 0) {
                    write(1, "Slot is empty.\n", 15);
                    valid_supemon = 0;
                } else {
                    Supemon *temp = ptrPlayer->supemons[0];
                    ptrPlayer->supemons[0] = ptrPlayer->supemons[choice-1];
                    ptrPlayer->supemons[choice-1] = temp;
                    valid_supemon = 1;

                    write(1, "You bring ", 10);
                    write(1, ptrPlayer->supemons[0]->name, strlen(ptrPlayer->supemons[0]->name));
                    write(1, " to the fight !\n", 16);
                }
                break;
            case 7:
                write(1, "You canceled, you are back to the fight !\n", 42);
                valid_supemon = 1;
                return 0;
                break;
            default:
                write(1, "Please enter a number between 1-7\n", 34);
        }
    } while (valid_supemon == 0 || (choice != 2 && choice != 3 && choice != 4 && choice != 5 && choice != 6 && choice != 7));
    return 1; 
}

int use_item(Player *ptrPlayer) {
    char bufferStr[7];

    write(1, "You have:\n", 10);
    write(1, "1. Potion ", 10);
    write(1, "(x", 2);
    sprintf(bufferStr, "%d", ptrPlayer->potions);
    write(1, bufferStr, strlen(bufferStr));
    write(1, ")\n", 2);
    write(1, "2. Super Potion ", 16);
    write(1, "(x", 2);
    sprintf(bufferStr, "%d", ptrPlayer->super_potions);
    write(1, bufferStr, strlen(bufferStr));
    write(1, ")\n", 2);
    write(1, "3. Rare Candy ", 14);
    write(1, "(x", 2);
    sprintf(bufferStr, "%d", ptrPlayer->rare_candy);
    write(1, bufferStr, strlen(bufferStr));
    write(1, ")\n", 2);
    write(1, "4. Cancel\n", 10);

    short choice;
    short is_item_used;

    do {
        get_input("1, 2, 3 or 4: ", &choice, 'i', 3);
        
        switch (choice) {
            case 1:
                if (ptrPlayer->potions > 0 && ptrPlayer->items_used < 4) {
                    is_item_used = 1;
                    ptrPlayer->potions--;
                    ptrPlayer->supemons[0]->hp += 5;
                    if (ptrPlayer->supemons[0]->hp > ptrPlayer->supemons[0]->max_hp) {
                        ptrPlayer->supemons[0]->hp = ptrPlayer->supemons[0]->max_hp;
                    }
                    write(1, "You used a potion\n", 18);
                    ptrPlayer->items_used++;
                } else if (ptrPlayer->items_used >= 4) {
                    is_item_used = 0;
                    write(1, "You have reached the maximum number of items used in this battle\n", 65);
                } else {
                    is_item_used = 0;
                    write(1, "You don't have any potion\n", 26);
                }
                break;
            case 2:
                if (ptrPlayer->super_potions > 0 && ptrPlayer->items_used < 4) {
                    is_item_used = 1;
                    ptrPlayer->super_potions--;
                    ptrPlayer->supemons[0]->hp += 10;
                    if (ptrPlayer->supemons[0]->hp > ptrPlayer->supemons[0]->max_hp) {
                        ptrPlayer->supemons[0]->hp = ptrPlayer->supemons[0]->max_hp;
                    }
                    write(1, "You used a super potion\n", 24);
                    ptrPlayer->items_used++;
                } else if (ptrPlayer->items_used >= 4) {
                    is_item_used = 0;
                    write(1, "You have reached the maximum number of items used in this battle\n", 65);
                } else {
                    is_item_used = 0;
                    write(1, "You don't have any super potion\n", 32);
                }
                break;
            case 3:
                if (ptrPlayer->rare_candy > 0 && ptrPlayer->items_used < 4) {
                    is_item_used = 1;
                    ptrPlayer->rare_candy--;
                    level_up(ptrPlayer->supemons[0]);
                    write(1, "You used a rare candy\n", 22);
                    ptrPlayer->items_used++;
                } else if (ptrPlayer->items_used >= 4) {
                    is_item_used = 0;
                    write(1, "You have reached the maximum number of items used in this battle\n", 65);
                } else {
                    is_item_used = 0;
                    write(1, "You don't have any rare candy\n", 30);
                }
                break;
            case 4:
                is_item_used = 1;
                write(1, "You canceled\n", 14);
                return 0;
                break;
            default:
                is_item_used = 0;
                write(1, "Invalid choice\n", 16);
        }
    } while (is_item_used == 0);

    return 1;
}

int capture(Player *ptrPlayer, Supemon *ptrEnemy) {
    float success_rate = ((float)(ptrEnemy->max_hp - ptrEnemy->hp) / ptrEnemy->max_hp - 0.5)*100;
    short random_number = rand() % 100;

    if (random_number < success_rate) {
        write(1, "You successfully captured ", 26);
        write(1, ptrEnemy->name, sizeof(ptrEnemy->name)+1);
        write(1, " !\n", 3);
        
        short available_position = -1;
        for (short i=0; i<6; i++) {

            if (ptrPlayer->supemons[i] == 0) {
                available_position = i;
                reset_supemon_stats(ptrEnemy);
                ptrPlayer->supemons[i] = ptrEnemy;
                ptrPlayer->supemons[i]->hp = ptrPlayer->supemons[i]->max_hp;
                break;
            }
        }

        if (available_position == -1) {
            write(1, "You have no more available places in your team\n", 47);
            write(1, "What Supemon do you want to replace ?\n", 38);

            for (short i=0; i<6; i++) {
                char bufferStr[7];
                sprintf(bufferStr, "%d", i+1);
                write(1, bufferStr, strlen(bufferStr));
                write(1, ". ", 2);
                write(1, ptrPlayer->supemons[i]->name, strlen(ptrPlayer->supemons[i]->name));
                write(1, " lvl.", 5);
                sprintf(bufferStr, "%d", ptrPlayer->supemons[i]->level);
                write(1, bufferStr, strlen(bufferStr));
                write(1, "\t(", 2);
                sprintf(bufferStr, "%d", ptrPlayer->supemons[i]->hp);
                write(1, bufferStr, strlen(bufferStr));
                write(1, "/", 1);
                sprintf(bufferStr, "%d", ptrPlayer->supemons[i]->max_hp);
                write(1, bufferStr, strlen(bufferStr));
                write(1, "HP)\n", 4);
            }
            
            write(1, "7. Don't replace any Supemon\n", 29);

            short choice;
            
            do {
                get_input("1, 2, 3, 4, 5, 6 or 7: ", &choice, 'i', 3);

                switch (choice) {
                    case 1:
                    case 2:
                    case 3:
                    case 4:
                    case 5:
                    case 6:
                        reset_supemon_stats(ptrEnemy);
                        ptrPlayer->supemons[choice-1] = ptrEnemy;
                        ptrPlayer->supemons[choice-1]->hp = ptrPlayer->supemons[choice-1]->max_hp;
                        break;
                    case 7:
                        write(1, "You've released ", 16);
                        write(1, ptrEnemy->name, sizeof(ptrEnemy->name));
                        write(1, " into the wild !\n", 17);
                        break;
                    default:
                        write(1, "Please enter a number between 1-7\n", 34);
                }

            } while(choice != 1 && choice != 2 && choice != 3 && choice != 4 && choice != 5 && choice != 6 && choice != 7);
        } 
        win(ptrPlayer, ptrEnemy);
        return 0;
    } else {
        write(1, "You failed to capture ", 22);
        write(1, ptrEnemy->name, sizeof(ptrEnemy->name)+1);
        write(1, " !\n", 3);
        return 1;
    }
}

int run_away(Supemon *ptrSupemon, Supemon *ptrEnemy) {
    float success_rate = ((float)ptrSupemon->speed / (ptrSupemon->speed + ptrEnemy->speed))*100;
    short random_number = rand() % 100;

    if (random_number < success_rate) {
        write(1, "You successfully run away !\n", 28);
        freeSupemon(ptrEnemy);
        return 0;
    } else {
        write(1, "You failed to run away !\n", 25);
        return 1;
    }
}

int round_number(float num) {
    return num == (int) num ? num : (rand() % 100 < 50) ? (int)(num + 1) : (int)(num);
}

void level_up(Supemon *ptrSupemon) {
    ptrSupemon->level++;
    ptrSupemon->experience = 0;
    ptrSupemon->experience_max += 1000;
    
    ptrSupemon->max_hp = round_number(ptrSupemon->max_hp * 1.05);
    ptrSupemon->base_attack = round_number(ptrSupemon->base_attack * 1.05);
    ptrSupemon->base_defense = round_number(ptrSupemon->base_defense * 1.05);
    ptrSupemon->base_evasion = round_number(ptrSupemon->base_evasion * 1.05);
    ptrSupemon->base_accuracy = round_number(ptrSupemon->base_accuracy * 1.05);
    ptrSupemon->speed = round_number(ptrSupemon->speed * 1.05);
    
    ptrSupemon->hp = ptrSupemon->max_hp;
    ptrSupemon->attack = ptrSupemon->base_attack;
    ptrSupemon->defense = ptrSupemon->base_defense;
    ptrSupemon->evasion = ptrSupemon->base_evasion;
    ptrSupemon->accuracy = ptrSupemon->base_accuracy;
}

void win(Player *ptrPlayer, Supemon *ptrEnemy) {
    short random_supecoins = rand() % 401 + 100;
    short random_experience = rand() % 401 + 100;

    char bufferStr[7];
    write(1, "You've won ", 11);
    sprintf(bufferStr, "%d", random_supecoins);
    write(1, bufferStr, strlen(bufferStr));
    write(1, " supecoins and ", 15);
    sprintf(bufferStr, "%d", random_experience);
    write(1, bufferStr, strlen(bufferStr));
    write(1, " experience !\n", 14);

    ptrPlayer->money += random_supecoins;
    if (ptrPlayer->supemons[0]->experience + random_experience >= ptrPlayer->supemons[0]->experience_max) {
        int overflow_experience = (ptrPlayer->supemons[0]->experience + random_experience) - ptrPlayer->supemons[0]->experience_max;
        level_up(ptrPlayer->supemons[0]);
        ptrPlayer->supemons[0]->experience += overflow_experience;
        write(1, ptrPlayer->supemons[0]->name, strlen(ptrPlayer->supemons[0]->name));
        write(1, " has level up !\n", 16); 
    } else {
        ptrPlayer->supemons[0]->experience += random_experience;
    }
}

void reset_supemon_stats(Supemon *ptrSupemon) {
    ptrSupemon->attack = ptrSupemon->base_attack;
    ptrSupemon->defense = ptrSupemon->base_defense;
    ptrSupemon->evasion = ptrSupemon->base_evasion;
    ptrSupemon->accuracy = ptrSupemon->base_accuracy;
}
