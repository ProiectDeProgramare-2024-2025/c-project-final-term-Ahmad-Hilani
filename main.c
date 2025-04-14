#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "colors.h"

#define MAX_PLAYERS 1000
#define MAX_NAME 100
#define FILE_NAME "matches.txt"

typedef struct {
    char name[MAX_NAME];
    int wins;
    int losses;
} Player;

void clearScreen() {
#ifdef _WIN32
    system("cls");
#else
    system("clear");
#endif
}

void header() {
    printf(COLOR_CYAN "--------------\n");
    printf(BOLD "Tennis Match Tracker\n" BOLD_RESET);
    printf("--------------\n" COLOR_RESET);
    printf("1 - " COLOR_GREEN "Add Match\n" COLOR_RESET);
    printf("2 - " COLOR_YELLOW "Display Rankings\n" COLOR_RESET);
    printf("3 - " COLOR_MAGENTA "View Match History\n" COLOR_RESET);
    printf("4 - " COLOR_RED "Delete All Matches\n" COLOR_RESET);
    printf("0 - " COLOR_BLUE "Exit\n" COLOR_RESET);
    printf(COLOR_CYAN "--------------\n" COLOR_RESET);
}

int getValidMenuOption() {
    char input[10];
    int option;
    while (1) {
        printf(COLOR_WHITE "Enter option (0-4): " COLOR_RESET);
        fgets(input, sizeof(input), stdin);
        if (sscanf(input, "%d", &option) == 1 && option >= 0 && option <= 4) {
            return option;
        }
        printf(COLOR_RED "Invalid input! Please enter a number between 0 and 4.\n" COLOR_RESET);
    }
}

void addMatch() {
    char input[200];
    char player1[MAX_NAME], player2[MAX_NAME];
    int score1, score2;

    clearScreen();
    printf(COLOR_CYAN "Add New Match\n" COLOR_RESET);
    printf("Enter match result in the following format:\n");
    printf(COLOR_YELLOW "Player1 Score1 Player2 Score2\n" COLOR_RESET);
    printf("Example: " COLOR_GREEN "John 6 Roger 4\n" COLOR_RESET);

    while (1) {
        printf("\nEnter match result: ");
        fgets(input, sizeof(input), stdin);
        if (sscanf(input, "%s %d %s %d", player1, &score1, player2, &score2) == 4) {
            if (score1 >= 0 && score2 >= 0) {
                break;
            }
            printf(COLOR_RED "Invalid scores! Scores must be non-negative numbers.\n" COLOR_RESET);
        } else {
            printf(COLOR_RED "Invalid input format! Please enter in the format: Player1 Score1 Player2 Score2\n" COLOR_RESET);
        }
    }

    FILE *file = fopen(FILE_NAME, "a");
    if (!file) {
        printf(COLOR_RED "Error opening file!\n" COLOR_RESET);
        return;
    }
    fprintf(file, "%s %d %s %d\n", player1, score1, player2, score2);
    fclose(file);

    printf(COLOR_GREEN "\nMatch added successfully!\n" COLOR_RESET);
}

void displayRankings() {
    clearScreen();
    FILE *file = fopen(FILE_NAME, "r");
    if (!file) {
        printf(COLOR_RED "No match data found!\n" COLOR_RESET);
        return;
    }

    Player players[MAX_PLAYERS];
    int playerCount = 0;
    char p1[MAX_NAME], p2[MAX_NAME];
    int s1, s2;

    while (fscanf(file, "%s %d %s %d", p1, &s1, p2, &s2) != EOF) {
        int found1 = -1, found2 = -1;
        for (int i = 0; i < playerCount; i++) {
            if (strcmp(players[i].name, p1) == 0) found1 = i;
            if (strcmp(players[i].name, p2) == 0) found2 = i;
        }

        if (found1 == -1) {
            strcpy(players[playerCount].name, p1);
            players[playerCount].wins = 0;
            players[playerCount].losses = 0;
            found1 = playerCount++;
        }
        if (found2 == -1) {
            strcpy(players[playerCount].name, p2);
            players[playerCount].wins = 0;
            players[playerCount].losses = 0;
            found2 = playerCount++;
        }

        if (s1 > s2) {
            players[found1].wins++;
            players[found2].losses++;
        } else {
            players[found2].wins++;
            players[found1].losses++;
        }
    }
    fclose(file);

    printf(COLOR_CYAN "\nCurrent Rankings:\n" COLOR_RESET);
    for (int i = 0; i < playerCount; i++) {
        printf("%d. " COLOR_YELLOW "%s" COLOR_RESET " - " COLOR_GREEN "Wins: %d" COLOR_RESET ", " COLOR_RED "Losses: %d\n" COLOR_RESET, 
               i + 1, players[i].name, players[i].wins, players[i].losses);
    }
}

void viewHistory() {
    clearScreen();
    char searchName[MAX_NAME];
    printf("Enter player name to view their match history: ");

    while (1) {
        fgets(searchName, sizeof(searchName), stdin);
        searchName[strcspn(searchName, "\n")] = 0;
        if (strlen(searchName) > 0) {
            break;
        }
        printf(COLOR_RED "Invalid input! Please enter a valid player name: " COLOR_RESET);
    }

    FILE *file = fopen(FILE_NAME, "r");
    if (!file) {
        printf(COLOR_RED "No match data found!\n" COLOR_RESET);
        return;
    }

    int found = 0;
    char p1[MAX_NAME], p2[MAX_NAME];
    int s1, s2;

    printf(COLOR_CYAN "\nMatch history for " COLOR_YELLOW "%s" COLOR_CYAN ":\n" COLOR_RESET, searchName);
    while (fscanf(file, "%s %d %s %d", p1, &s1, p2, &s2) != EOF) {
        if (strcmp(p1, searchName) == 0 || strcmp(p2, searchName) == 0) {
            printf(COLOR_YELLOW "%s" COLOR_RESET " %d - %d " COLOR_YELLOW "%s\n" COLOR_RESET, p1, s1, s2, p2);
            found = 1;
        }
    }
    fclose(file);

    if (!found) {
        printf(COLOR_RED "No matches found for %s!\n" COLOR_RESET, searchName);
    }
}

void deleteMatches() {
    clearScreen();
    char confirm;
    printf(COLOR_RED "WARNING: This will delete all match records!\n" COLOR_RESET);
    printf("Are you sure you want to continue? (y/n): ");
    
    while (1) {
        scanf(" %c", &confirm);
        if (confirm == 'y' || confirm == 'Y') {
            FILE *file = fopen(FILE_NAME, "w");
            if (file) {
                fclose(file);
                printf(COLOR_GREEN "All match records have been deleted!\n" COLOR_RESET);
            } else {
                printf(COLOR_RED "Error deleting match records!\n" COLOR_RESET);
            }
            break;
        } else if (confirm == 'n' || confirm == 'N') {
            printf(COLOR_YELLOW "Operation cancelled.\n" COLOR_RESET);
            break;
        } else {
            printf(COLOR_RED "Invalid input! Please enter 'y' or 'n': " COLOR_RESET);
        }
    }
}

void menu(int option) {
    switch (option) {
        case 1: addMatch(); break;
        case 2: displayRankings(); break;
        case 3: viewHistory(); break;
        case 4: deleteMatches(); break;
        default: printf(COLOR_BLUE "Exiting...\n" COLOR_RESET); break;
    }
}

int main() {
    int option;
    do {
        clearScreen();
        header();
        option = getValidMenuOption();
        menu(option);
    } while (option > 0 && option <= 4);
    return 0;
}
