#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include <stdbool.h>


// attributes of each player
struct Player {
    char name[20];
    char grid [10][10];
    char displayedGrid[10][10];
    char smokeGrid [10][10];
    char shipGrid [10][10];
    int rows;
    int columns;
    bool placedShips;
    int nbrOfShipsSunk;
    int radarCount;
    int smokeCount;
    int CarrierCount;
    int BattleshipCount;
    int DestroyerCount;
    int SubmarineCount;
    bool unlockedArtillery;
    bool unlockedTorpedo;
    int turnCount;
    int artilleryMove;
    int torpedoMove;
    bool bot;
    int probability [10][10];
    char* checkeredGrid [50];
    char prevHit [4];
    int adjacencyCounter;
    char* nextHit [18];
    bool isRandom;
    char saveMisses [10][10];
};


// initializes nextHit array
void initializeNextHit (struct Player* bot) {
    for (int i=0; i<17; i++) {
        bot->nextHit [i] = "";
    }
    bot->nextHit [17] = "\0";
}


// initializes everything needed
void initialize (struct Player* p) {
    p->turnCount = 0;
    p->artilleryMove = 0;
    p->torpedoMove = 0;
    p->unlockedArtillery = true;
    p->unlockedTorpedo = true;
    p->CarrierCount = 5;
    p->BattleshipCount = 4;
    p->DestroyerCount = 3;
    p->SubmarineCount = 2;
    p->rows = 10;
    p->columns = 10;
    p->placedShips = false; //did not place ships yet
    p->nbrOfShipsSunk = 0;
    p->radarCount = 3;
    p->smokeCount = 0;
    for (int i=0; i<p->rows; i++) {
        for (int j=0; j<p->columns; j++) {
            p->grid[i][j]='~';
            p->smokeGrid[i][j]='~';
            p->displayedGrid[i][j]='~';
            p->shipGrid[i][j]='~';
            if (p->bot==true) {
                p->checkeredGrid[i][j]='~';
                p->probability [i][j] = 0;
                p->saveMisses [i][j] = '~';
            }
        }
    }
    if (p->bot==true) {
        p->isRandom = true;
        strcpy(p->prevHit, "");
        initializeNextHit(p->nextHit);
    }
}


// calculates the probability of each cell of being part of a ship
void calculateProbability (struct Player* bot) {
    int p = 0;
    for (int i=0; i<10; i++) {
        for (int j=0; j<10; j++) {
            if (bot->displayedGrid [i][j] == '~') {
                for (int k=1; k<5; k++) {
                    if (j+k>9) {
                        break;
                    }
                    if (bot->displayedGrid[i][j+k]=='~') {
                        p += 1;
                    } else break;
                }
                for (int k=1; k<5; k++) {
                    if (i+k>9) {
                        break;
                    }
                    if (bot->displayedGrid[i+k][j]=='~') {
                        p += 1;
                    } else break;
                }
                for (int k=1; k<5; k++) {
                    if (j-k<0) {
                        break;
                    }
                    if (bot->displayedGrid[i][j-k]=='~') {
                        p += 1;
                    } else break;
                }
                for (int k=1; k<5; k++) {
                    if (i-k<0) {
                        break;
                    }
                    if (bot->displayedGrid[i-k][j]=='~') {
                        p += 1;
                    } else break;
                }
                bot->probability [i][j] = p;
            }
        }
    }
}

// calculates the probability of each cell of being part of a ship (this is used in the smokeBot sequence)
void calculateProbabilityPlayer (struct Player* player) {
    int p = 0;
    for (int i=0; i<10; i++) {
        for (int j=0; j<10; j++) {
            if (player->displayedGrid [i][j] == '~') {
                for (int k=1; k<5; k++) {
                    if (j+k>9) {
                        break;
                    }
                    if (player->displayedGrid[i][j+k]=='~') {
                        p += 1;
                    } else break;
                }
                for (int k=1; k<5; k++) {
                    if (i+k>9) {
                        break;
                    }
                    if (player->displayedGrid[i+k][j]=='~') {
                        p += 1;
                    } else break;
                }
                for (int k=1; k<5; k++) {
                    if (j-k<0) {
                        break;
                    }
                    if (player->displayedGrid[i][j-k]=='~') {
                        p += 1;
                    } else break;
                }
                for (int k=1; k<5; k++) {
                    if (i-k<0) {
                        break;
                    }
                    if (player->displayedGrid[i-k][j]=='~') {
                        p += 1;
                    } else break;
                }
                player->probability [i][j] = p;
            }
        }
    }
}
// bot's coordinates similar format as the user's input (character array having letter representing column then row)
char* botCoordinates (int row, int column) {
    char *c= malloc(3 * sizeof(char));
    if (column==0) {
        c[0] = 'A';
    } else if (column==1) {
        c[0] = 'B';
    } else if (column==2) {
        c[0] = 'C';
    } else if (column==3) {
        c[0] = 'D';
    } else if (column==4) {
        c[0] = 'E';
    } else if (column==5) {
        c[0] = 'F';
    } else if (column==6) {
        c[0] = 'G';
    } else if (column==7) {
        c[0] = 'H';
    } else if (column==8) {
        c[0] = 'I';
    } else c[0] = 'J';
    if (row==10) {
        c[1] = '1';
        c[2] = '0';
    } else c[1] = row + '0';
    return c;
}


// finds the max probability in the bot's probability grid
char* findMaxProbability (struct Player* bot) {
    int max;
    char* coordinates = (char*)malloc(4 * sizeof(char));
    for (int i=0; i<10; i++) {
        for (int k=0; k<10; k++) {
            if (bot->probability[i][k]>max) {
                max = bot->probability[i][k];
                strcpy(coordinates, botCoordinates(i, k));
            }
        }
    }
    return coordinates;
}

// finds the max probability in the player's probability grid
char* findMaxProbabilityPlayer (struct Player* player) {
    char maxNums[100];
    for(int i = 0; i < 100; i++){
        maxNums[i] = -1;
    }
    int index = 0;
    int max;
    char* coordinates = (char*)malloc(4 * sizeof(char));
    for (int i=0; i<10; i++) {
        for (int k=0; k<10; k++) {
            if (player->probability[i][k]>max) {
                max = player->probability[i][k];
            }
        }
    }
    for (int i=0; i<10; i++) {
        for (int k=0; k<10; k++) {
            if (player->probability[i][k]==max) {
                maxNums[index] = i;
                maxNums[index+1] = k;
                index+=2;
            }
        }
    }
    srand(time(NULL));
    int randomIndex = rand() % 99;
    while(randomIndex % 2 != 0 || maxNums[randomIndex] == -1){
        int randomIndex = rand() % 99;
    }
    strcpy(coordinates, botCoordinates(randomIndex, randomIndex + 1));
    return coordinates;
}

// converts index of column to corresponding letter on the grid
char convertToLetter (int c) {
    char l;
    if (c==0) {
        l = 'A';
    } else if (c==1) {
        l = 'B';
    } else if (c==2) {
        l = 'C';
    } else if (c==3) {
        l = 'D';
    } else if (c==4) {
        l = 'E';
    } else if (c==5) {
        l = 'F';
    } else if (c==6) {
        l = 'G';
    } else if (c==7) {
        l = 'H';
    } else if (c==8) {
        l = 'I';
    } else l = 'J';
    return l;
}


// checkered grid for considering Carrier was the last ship
void carrierCheckeredGrid (struct Player* bot) {
    int counter = 0;
    int index = 5;
    for (int i=0; i<10; i++) {
        int j = index%5;
        for ( ; j<10; j+=5) {
            if (bot->displayedGrid[i][j]=='~' && bot->saveMisses [i][j]=='~') {
                bot->checkeredGrid[counter][0] = convertToLetter(j);
                if (i==9) {
                    bot->checkeredGrid[counter][1] = '1';
                    bot->checkeredGrid[counter][2] = '0';
                    bot->checkeredGrid[counter][3] = '\0';
                } else {
                    bot->checkeredGrid[counter][1] = (i+1) + '0';
                    bot->checkeredGrid[counter][2] = '\0';
                }
                counter += 1;
            }
        }
        index += 1;
    }
}


// checkered grid for considering Battleship was the last ship
void battleshipCheckeredGrid (struct Player* bot) {
    int counter = 0;
    int index = 4;
    for (int i=0; i<10; i++) {
        int j = index%4;
        for ( ; j<10; j+=4) {
            if (bot->displayedGrid[i][j]=='~' && bot->saveMisses [i][j]=='~') {
                bot->checkeredGrid[counter][0] = convertToLetter(j);
                if (i==9) {
                    bot->checkeredGrid[counter][1] = '1';
                    bot->checkeredGrid[counter][2] = '0';
                    bot->checkeredGrid[counter][3] = '\0';
                } else {
                    bot->checkeredGrid[counter][1] = (i+1) + '0';
                    bot->checkeredGrid[counter][2] = '\0';
                }
                counter += 1;
            }
        }
        index += 1;
    }
}


// checkered grid for considering Destroyer was the last ship
void destroyerCheckeredGrid (struct Player* bot) {
    int counter = 0;
    int index = 3;
    for (int i=0; i<10; i++) {
        int j = index%3;
        for ( ; j<10; j+=3) {
            if (bot->displayedGrid[i][j]=='~' && bot->saveMisses [i][j]=='~') {
                bot->checkeredGrid[counter][0] = convertToLetter(j);
                if (i==9) {
                    bot->checkeredGrid[counter][1] = '1';
                    bot->checkeredGrid[counter][2] = '0';
                    bot->checkeredGrid[counter][3] = '\0';
                } else {
                    bot->checkeredGrid[counter][1] = (i+1) + '0';
                    bot->checkeredGrid[counter][2] = '\0';
                }
                counter += 1;
            }
        }
        index += 1;
    }
}


// checkered grid for considering Submarine was the last ship
void submarineCheckeredGrid (struct Player* bot) {
    int counter = 0;
    for (int i=0; i<10; i++) {
        int j;
        if (i%2==0) {
            j=0;
        } else j=1;
        for ( ; j<10; j+=2) {
            if (bot->displayedGrid[i][j]=='~' && bot->saveMisses [i][j]=='~') {
                bot->checkeredGrid[counter][0] = convertToLetter(j);
                if (i==9) {
                    bot->checkeredGrid[counter][1] = '1';
                    bot->checkeredGrid[counter][2] = '0';
                    bot->checkeredGrid[counter][3] = '\0';
                } else {
                    bot->checkeredGrid[counter][1] = (i+1) + '0';
                    bot->checkeredGrid[counter][2] = '\0';
                }
                counter += 1;
            }
        }
    }
}


// checks which ship is the last one we need to sink and changes the checkered grid accordingly
void checksLastShipGrid (struct Player* bot) {
    if (bot->nbrOfShipsSunk==3) {
        if (bot->CarrierCount>0) {
            carrierCheckeredGrid(bot);
        } else if (bot->SubmarineCount>0) {
            submarineCheckeredGrid(bot);
        } else if (bot->BattleshipCount>0) {
            battleshipCheckeredGrid(bot);
        } else if (bot->DestroyerCount>0) {
            destroyerCheckeredGrid(bot);
        }
    }
}


// alternates players according to their turn
void alternatePlayers(struct Player **p, struct Player *p1, struct Player *p2) {
    if (*p == p1) {
        *p = p2;
    } else *p = p1;
}


// prints the grid
void printGrid (struct Player *p) {
    printf("\tA\tB\tC\tD\tE\tF\tG\tH\tI\tJ\n");
    for (int i=0; i<p->rows; i++) {
        printf("%d\t", i+1);
        for (int j=0; j<p->columns; j++) {
            printf("%c\t", p->grid[i][j]);
        }
        printf("\n");
    }
}


// prints displayed grid
void printDisplayedGrid(struct Player *p) {
    printf("\tA\tB\tC\tD\tE\tF\tG\tH\tI\tJ\n");
    for (int i=0; i<p->rows; i++) {
        printf("%d\t", i+1);
        for (int j=0; j<p->columns; j++) {
            printf("%c\t", p->displayedGrid[i][j]);
        }
        printf("\n");
    }
}


// prints the smoke grid
void printSmokeGrid (struct Player *p) {
    printf("\tA\tB\tC\tD\tE\tF\tG\tH\tI\tJ\n");
    for (int i=0; i<p->rows; i++) {
        printf("%d\t", i+1);
        for (int j=0; j<p->columns; j++) {
            printf("%c\t", p->smokeGrid[i][j]);
        }
        printf("\n");
    }
}


// asks for coordinates and orientation of each ship
char* getQuestion (int i) {
    char *q;
    if (i==5) {
        q = (char*)malloc(8 * sizeof(char));
        strcpy(q, "Carrier (5 cells): \t");
    } else if (i==4) {
        q = (char*)malloc(11 * sizeof(char));
        strcpy(q, "BattleShip (4 cells): \t");
    } else if (i==3) {
        q = (char*)malloc(10 * sizeof(char));
        strcpy(q, "Destroyer (3 cells): \t");
    } else  {
        q = (char*)malloc(10 * sizeof(char));
        strcpy(q, "Submarine (2 cells): \t");
    }
    return q;
}


// converts a string to lower case
void toLower (char a[]) {
    for (int i=0; i<strlen(a); i++) {
        if (a[i] >= 'A' && a[i] <= 'Z') {
            a[i] += 32;
        }
    }
}


// gets row number from the coordinates input (as it would be in the grid 2D array)
int getRow(char c[]) {
    if (c[1] == '1' && c[2] == '0') {
        return 10-1;
    } else {
        return (c[1] - '0')-1;
    }
}


// converts the letter column to the index that corresponds to it in the grid
int convertToColumnIndex (char a) {
    if (a=='A' || a=='a') {
        return 0;
    } else if (a=='B' || a=='b') {
        return 1;
    } else if (a=='C' || a=='c') {
        return 2;
    } else if (a=='D' || a=='d') {
        return 3;
    } else if (a=='E' || a=='e') {
        return 4;
    } else if (a=='F' || a=='f') {
        return 5;
    } else if (a=='G' || a=='g') {
        return 6;
    } else if (a=='H' || a=='h') {
        return 7;
    } else if (a=='I' || a=='i') {
        return 8;
    } else if (a=='J' || a=='j') {
        return 9;
    }
}


// checks if coordinates are valid
bool validCoordinates (char c []) {
    if (((c[0] >= 'A' && c[0] <= 'J') || (c[0] >= 'a' && c[0] <= 'j'))
        && ((c[1] >= '1' && c[1] <= '9' && c[2]=='\0')
            || (c[1]=='1' && c[2]=='0'))) {
        return true;
    } else {
        printf("Invalid coordinates.\n");
        return false;
    }
}


// checks if orientation is valid
bool validOrientation (char o []) {
    if ((strcmp(o, "horizontal")==0) || (strcmp(o, "vertical")==0)) {
        return true;
    } else {
        printf("Invalid orientation.\n");
        return false;
    }
}


// checks if the placement of the ship is beyond the boundaries of the grid or not
bool checkBeyondGrid (struct Player *p, char orientation [], int nbrOfCells, int col, int row) {
    if ((strcmp(orientation, "horizontal")==0 && ((col+nbrOfCells)-1)>9) || (strcmp(orientation, "vertical")==0 && (((row)+nbrOfCells)-1)>9)) {
        if (p->bot==false) {
            printf("Coordinates chosen result in an out-of-bounds placement of the ship. Please enter new coordinates.\n");
        }
        return true; //return true if it is beyond the boundaries and can't be placed
    }
    return false;
}


// checks if a ship is already found in a specific position
bool checkCellAvailability (struct Player *p, char orientation [], int nbrOfCells, int col, int row) {
    if (strcmp(orientation, "horizontal")==0) {
        int i=row;
        for (int j=col; j<(col+nbrOfCells); j++) {
            if (p->grid[i][j]=='X') {
                if (p->bot==false) {
                    printf("Coordinates chosen are already occupied. Please enter new coordinates.\n");
                }
                return false;
            }
        }
    } else {
        int j=col;
        for (int i=row; i<(row+nbrOfCells); i++) {
            if (p->grid[i][j]=='X') {
                if (p->bot==false) {
                    printf("Coordinates chosen are already occupied. Please enter new coordinates.\n");
                }
                return false;
            }
        }
    }
    return true;
}


// places the ships on the grid based on input
void placeShips (struct Player *p, char orientation [], int nbrOfCells, int col, int row) {
    toLower(orientation);
    if (strcmp(orientation, "horizontal")==0) {
        int i=row;
        for (int j=col; j<(col+nbrOfCells); j++) {
            p->grid[i][j]='X';
            p->smokeGrid[i][j]='X';
            p->shipGrid[i][j] = (nbrOfCells + '0') ;//each ship will have uniquely-valued cells
        }
    } else {
        int j=col;
        for (int i=row; i<(row+nbrOfCells); i++) {
            p->grid[i][j]='X';
            p->smokeGrid[i][j]='X';
            p->shipGrid[i][j] = (nbrOfCells + '0');
        }
    }
}


// places the ships of the bot
void placeShips (struct Player *bot) {
    int numberOfCells = 5;
    while (numberOfCells>1) {
        srand(time(NULL));
        int row = rand() % 10; // random row
        int column = rand() % 10; // random column
        int r = rand() % 2; // in order to choose a random orientation
        char orientation [11];
        if (r==0) {
            strcpy(orientation, "horizontal");
        } else strcpy(orientation, "vertical");
        while (checkBeyondGrid(bot, orientation, numberOfCells, column, row) && !checkCellAvailability(bot, orientation, numberOfCells, column, row)) {
            row = rand() % 10;         // keeps on randomizing the values of row and column until valid
            column = rand() % 10;
        }
        placeShips(bot, orientation, numberOfCells, column, row);
        numberOfCells-=1;
    }
}


// decrements counter for a specific ship everytime a part of it is hit
void decrementShipTypeCounter (struct Player *p, char c) {
    if (c=='5') {
        p->CarrierCount-=1;
    } else if (c=='4') {
        p->BattleshipCount-=1;
    } else if (c=='3') {
        p->DestroyerCount-=1;
    } else if (c=='2') {
        p->SubmarineCount-=1;
    }

}


//returns true if ship has been sunk and false if it was not
void shipSunk (struct Player *p, struct Player *p1) {
    if (p->CarrierCount==0) {
        p->nbrOfShipsSunk+=1;
        p->CarrierCount=-1;
        p->smokeCount+=1;
        printf("Congrats! You have sunk %s's Carrier!\n", p1->name);
    } else if (p->BattleshipCount==0) {
        p->nbrOfShipsSunk+=1;
        p->BattleshipCount=-1;
        p->smokeCount+=1;
        printf("Congrats! You have sunk %s's Battleship!\n", p1->name);
    } else if (p->DestroyerCount==0) {
        p->nbrOfShipsSunk+=1;
        p->DestroyerCount=-1;
        p->smokeCount+=1;
        printf("Congrats! You have sunk %s's Destroyer!\n", p1->name);
    } else if (p->SubmarineCount==0) {
        p->nbrOfShipsSunk+=1;
        p->SubmarineCount=-1;
        p->smokeCount+=1;
        printf("Congrats! You have sunk %s's Submarine!\n", p1->name);
    }
}


// will return true if it hit, false if it missed
bool HitOrMiss (struct Player *p, struct Player *p1,  char coordinates [], char difficultyLevel []) {
    if (validCoordinates(coordinates)) {
        int row = getRow(coordinates);
        int col = convertToColumnIndex(coordinates[0]);
        if (p1->grid[row][col]=='~') {
            if (strcmp(difficultyLevel, "easy")==0) {
                p->displayedGrid[row][col]='o'; //changes display on grid if missed and if in easy mode
            }
            if (p->bot==true) {
                p->saveMisses[row][col]='o'; // saves the misses for medium & hard bot modes
            }
            p1->grid[row][col]='o';
            return false;
        } else if (p1->grid[row][col]=='X') {
            p->displayedGrid[row][col]='*'; //changes display on grid to hit in both modes
            decrementShipTypeCounter(p, p1->shipGrid[row][col]);
            return true;
        } else {
            printf("Already missed before\n");
            return false; //if it already was missed before
        }
    }
}


// radar looks for ships in 2x2 area
void radar (struct Player *p, int col, int row) {
    for (int i=row; i<=row+1; i++) {
        for (int j=col; j<=col+1; j++) {
            if (p->smokeGrid[i][j]=='X') {
                printf("Enemy ships found.\n");
                return;
            }
        }
    }
    printf("No enemy ships found.\n");
}


// radar move can't be done on row/column 9;
bool checkInBounds (int row, int col) {
    if (row==9 || col==9) {
        printf("Index out of bounds.\n");
        return false;
    } else return true;
}


// smoke hides ships from radars in 2x2 area
void smoke (struct Player *p, int col, int row) {
    for (int i=row; i<=row+1; i++) {
        for (int j=col; j<=col+1; j++) {
            p->smokeGrid[i][j] = '~';
        }
    }
}


// fires at the chosen coordinates and states whether it was a hit or a miss
void fire(struct Player *nextPlayer, struct Player *otherPlayer, char coordinates[], char difficultyLevel[]) {

    if (HitOrMiss(nextPlayer, otherPlayer, coordinates, difficultyLevel)) {
        printf("Hit!\n");
    } else {
        printf("Miss!\n");
    }
    printDisplayedGrid(nextPlayer);

}


// checks the next available index in which we need to add
int getIndex (struct Player* bot) {
    for (int i=0; i<17; i++) {
        if (strcmp(bot->nextHit[i], "")==0) {
            return i;
        }
    }
    return -1;
}


// gets index of next hit
int getNextHit (struct Player* bot) {
    for (int i=0; i<17; i++) {
        if ((strcmp(bot->nextHit[i], "")!=0) && (strcmp(bot->nextHit[i], "-1")!=0)) {
            return i;
        }
    }
    return -1;
}


// fires at random, fires according to probability for hard mode starting from the 4th turn, fires according to checkered grid for the last ship in medium and hard modes
void randomFire (struct Player* bot, struct Player* otherPlayer, char difficultyLevel[]) {
    srand(time(NULL));
    if (!strcmp(difficultyLevel, "easy")==0 && bot->nbrOfShipsSunk==3) {
        int row = rand() % 10;
        int col = rand() % 10;
        checksLastShipGrid(bot);
        while (bot->checkeredGrid[row][col]=='~') {
            row = rand() % 10;
            col = rand() % 10;
        }
        char* coordinates = botCoordinates(row, col);
        fire(bot, otherPlayer, coordinates, difficultyLevel);
        if (bot->displayedGrid[row][col]=='X') {
            strcpy(bot->prevHit, coordinates);
            bot->isRandom = false;
        }
    } else if (strcmp(difficultyLevel, "hard") && bot->turnCount>3) {  // probability method starts after at least 4 turns
        calculateProbability(bot);
        char* coordinates = findMaxProbability(bot);
        int row = getRow(coordinates);
        int col = convertToColumnIndex(coordinates[0]);
        fire(bot, otherPlayer, coordinates, difficultyLevel);
        if (bot->displayedGrid[row][col]=='X') {
            strcpy(bot->prevHit, coordinates);
            bot->isRandom = false;
        }
    } else {
        int row = rand() % 10;
        int col = rand() % 10;
        while (bot->displayedGrid[row][col]!='~') {
            row = rand() % 10;
            col = rand() % 10;
        }
        char* coordinates= botCoordinates(row, col);
        fire(bot, otherPlayer, coordinates,difficultyLevel);
        if (bot->displayedGrid[row][col]=='X') {
            strcpy(bot->prevHit, coordinates);
            bot->isRandom = false;
        }
    }

}


// adds the adjacent cells to the adjacency array
void fireAdjacency (struct Player* bot, struct Player* otherPlayer, char difficultyLevel[]) {
    if (bot->adjacencyCounter==4) {
        int n = getNextHit(bot);
        if (n==-1) {
            bot->isRandom = true;
            randomFire(bot, otherPlayer, difficultyLevel);
        } else {
            strcpy(bot->prevHit, bot->nextHit[n]);
            bot->nextHit[n] = "-1";
        }
    } else {
        char *coordinates = bot->prevHit;
        int row = getRow(coordinates);
        int col = convertToColumnIndex(coordinates[0]);
        if (bot->adjacencyCounter==0) {
            if (row+1<10 && bot->displayedGrid[row+1][col]=='~' && bot->saveMisses[row+1][col]=='~') {
                fire(bot, otherPlayer, botCoordinates(row+1, col), difficultyLevel);
                if (bot->displayedGrid[row+1][col]=='X') {
                    bot->nextHit[getIndex(bot)] = botCoordinates(row+1, col);
                }
            } else bot->adjacencyCounter+=1;
        }
        if (bot->adjacencyCounter==1) {
            if (row-1>-1 && bot->displayedGrid[row-1][col]=='~' && bot->saveMisses[row-1][col]=='~') {
                fire(bot, otherPlayer, botCoordinates(row-1, col), difficultyLevel);
                if (bot->displayedGrid[row-1][col]=='X') {
                    bot->nextHit[getIndex(bot)] = botCoordinates(row-1, col);
                }
            } else bot->adjacencyCounter+=1;
        }
        if (bot->adjacencyCounter==2) {
            if (col+1<10 && bot->displayedGrid[row][col+1]=='~' && bot->saveMisses[row][col+1]=='~') {
                fire(bot, otherPlayer, botCoordinates(row, col+1), difficultyLevel);
                if (bot->displayedGrid[row][col+1]=='X') {
                    bot->nextHit[getIndex(bot)] = botCoordinates(row, col+1);
                }
            } else bot->adjacencyCounter+=1;
        }
        if (bot->adjacencyCounter==3) {
            if (col-1>-1 && bot->displayedGrid[row][col-1]=='~' && bot->saveMisses[row][col-1]=='~') {
                fire(bot, otherPlayer, botCoordinates(row, col-1), difficultyLevel);
                if (bot->displayedGrid[row][col-1]=='X') {
                    bot->nextHit[getIndex(bot)] = botCoordinates(row, col-1);
                }
            } else bot->adjacencyCounter+=1;
        }
    }
}


// gets the next coordinates for artillery
char* nextCoordinates1 (char coordinates[]) {
    char *c = malloc(4 * sizeof(char));
    c[0] = coordinates[0] + 1;
    if (coordinates[1]=='1' && coordinates [2]=='0' && coordinates [3] == '\0') {
        c[1] = '1';
        c[2] = '0';
        c[3] = '\0';
    } else {
        c[1] = coordinates[1];
        c[2] = '\0';
    }
    return c;
}


// gets the next coordinates for artillery
char* nextCoordinates2 (char coordinates[]) {
    char *c = malloc(4 * sizeof(char));
    c[0] = coordinates[0];
    if (coordinates[1]=='9') {
        c[1] = '1';
        c[2] = '0';
        c[3] = '\0';
    } else {
        c[1]=coordinates[1]+1;
        c[2] = '\0';
    }
    return c;
}


// artillery move: fires at a 2x2 area
void artillery(struct Player *nextPlayer, struct Player *otherPlayer, char difficultyLevel[], char coordinates[]) {
    fire(nextPlayer, otherPlayer, coordinates, difficultyLevel);
    fire(nextPlayer, otherPlayer, nextCoordinates1(coordinates), difficultyLevel);
    fire(nextPlayer, otherPlayer, nextCoordinates2(coordinates), difficultyLevel);
    fire(nextPlayer, otherPlayer, nextCoordinates1(nextCoordinates2(coordinates)), difficultyLevel);
    nextPlayer->unlockedArtillery = false;
}


// when it is chosen to use the torpedo move on a row
void torpedoRow (struct Player *nextPlayer, struct Player *otherPlayer, char row [], char difficultyLevel []) {
    char coordinates [4];
    if (strcasecmp(row, "1") == 0) {
        strcpy(coordinates, "A1");
    } else if (strcasecmp(row, "2") == 0) {
        strcpy(coordinates, "A2");
    } else if (strcasecmp(row, "3") == 0) {
        strcpy(coordinates, "A3");
    } else if (strcasecmp(row, "4") == 0) {
        strcpy(coordinates, "A4");
    } else if (strcasecmp(row, "5") == 0) {
        strcpy(coordinates, "A5");
    } else if (strcasecmp(row, "6") == 0) {
        strcpy(coordinates, "A6");
    } else if (strcasecmp(row, "7") == 0) {
        strcpy(coordinates, "A7");
    } else if (strcasecmp(row, "8") == 0) {
        strcpy(coordinates, "A8");
    } else if (strcasecmp(row, "9") == 0) {
        strcpy(coordinates, "A9");
    } else {
        strcpy(coordinates, "A10");
    }

    for (int i=1; i<=10; i++) {
        fire(nextPlayer, otherPlayer, coordinates, difficultyLevel);
        strcpy(coordinates, nextCoordinates1(coordinates));
    }
    nextPlayer->unlockedTorpedo = false;
}


// when it is chosen to use the torpedo move on a column
void torpedoColumn (struct Player *nextPlayer, struct Player *otherPlayer, char col [], char difficultyLevel []) {
    char *coordinates = malloc(3 * sizeof(char));
    coordinates [0] = col[0];
    coordinates [1] = '1';
    coordinates [2] = '\0';
    for (int i=1; i<=10; i++) {
        fire(nextPlayer, otherPlayer, coordinates, difficultyLevel);
        strcpy(coordinates, nextCoordinates2(coordinates));
    }
    nextPlayer->unlockedTorpedo = false;
}


// checks if the 2x2 area chosen for the bot's artillery move at random has at a specific amount of cells not fired at before (no info of)
bool artilleryCheck (struct Player *bot, int row, int col, int minUnknownCells) {
    int emptyCellCounter = 0;
    for (int j=col; j<col+2; j++) {
        if (bot->grid[row][j]=='~' && bot->saveMisses[row][j]=='~') {
            emptyCellCounter+=1;
        }
        if (bot->grid[row+1][j]=='~' && bot->saveMisses[row+1][j]=='~') {
            emptyCellCounter+=1;
        }
    }
    if (emptyCellCounter>=minUnknownCells) {
        return true;
    } else return false;
}


// checks for easy mode if the row/column chosen at random has at least 1 cell that we have no info on
bool torpedoCheck (struct Player* bot, int n, int o) {
    if (o==0) { // then we chose it to be a row
        for (int j=0; j<10; j++) {
            if (bot->grid[n][j]=='~' && bot->saveMisses[n][j]=='~') {
                return true;
            }
        }
    } else {
        for (int i=0; i<10; i++) {
            if (bot->grid[i][n]=='~' && bot->saveMisses[i][n]=='~') {
                return true;
            }
        }
    }
    return false;
}


// searches for the row or column (according to the one needed) that we have the least info on in order to use the torpedo move on it
int leastInfo (struct Player* bot, int o) {
    int r;
    int c;
    int colCounter = 0;
    int rowCounter = 0;
    int rMax = -1;
    int cMax = -1;
   if (o==0) {
       for (int i=0; i<10; i++) {
           for (int j=0; j<10; j++) {
               if (bot->grid[i][j]=='~' && bot->saveMisses[i][j]=='~') {
                   rowCounter+=1;
               }
           }
           if (rowCounter>rMax) {
               rMax = rowCounter;
               r = i;
           }
           rowCounter = 0;
       }
       return r;
   } else {
       for (int j=0; j<10; j++) {
           for (int i=0; i<10; i++) {
               if (bot->grid[i][j]=='~' && bot->saveMisses[i][j]=='~') {
                   colCounter+=1;
               }
           }
           if (colCounter>cMax) {
               cMax = colCounter;
               c = j;
           }
           colCounter = 0;
       }
       return c;
   }
}
// helper method designed to help us know how many unknown cells are around a 2x2 radar area
// it will be helpful for us in the diffulty levels for the botRadarMove
int countUnknownCells(int rows, int columns, char displayedGrid[10][10], int row, int column)
{
    int unknownCount=0;
    // loop to iterate over a 4x4 grid thats around the 2x2 radar area
    for(int i=row-1; i<=row+2; i++){
        for(int j=column-1; j<=column+2; j++){
            // its going to check if the cell is unknown(~) and within the bounds
            if(i>=0 && i<rows && j>=0 && j<columns && displayedGrid[i][j]=='~'){
                unknownCount++;
            }
        }
    }
    return unknownCount;
}
// bot radar move
void botRadarMove(struct Player* bot, struct Player* opponent, int difficulty)
{
    // to check if radar still has any radar moves left
    if(bot->radarCount <= 3)
    {
        printf("Bot reached maximum radar moves allowed.\n");
        return;
    }
    int x=0, y=0;
    bool found=false;

    // easy difficulty : random radar with at least 1 unknown surrounding cell
    if(difficulty==1)
    {
        do{
            x = rand() % (bot->rows -1);
            y = rand() % (bot->columns -1);
        }
        while(countUnknownCells(bot->rows, bot->columns, bot->displayedGrid, x, y)<1);
    }
    // medium difficulty : radar near a previous hit with at least 2 unknown surrounding cells
    else if(difficulty==2)
    {
        if(bot->prevHit[0] != '\0') // check if there was a previous hit, if there was : calculate the coordinates based on that hit
        {
            x=bot->prevHit[0]-'A'; //converts row char to index
            y=bot->prevHit[1]-'0'; //converts column char to int
            if(x>0){x=x-1;} //moves up by 1 row only if its not the first row
            if(y>0){y=y-1;} //moves left by 1 column only if its not the first column
        }
    }
    // hard difficulty : radar in a high probability area with at least 3 unknown surrounding cells
    else if (difficulty==3)
    {
        calculateProbability(bot);
        int maxProbabilty=0; //keeps track of the highest probability in the grid
        for(int i=0; i<bot->rows-1; i++){
            for(int j=0; j<bot->columns-1; j++){
                if(bot->probability[i][j] > maxProbabilty && countUnknownCells(bot->rows, bot->columns, bot->displayedGrid, i, j)>=3)
                {
                    maxProbabilty= bot->probability[i][j]; //update maxprobability to the current cell probability
                    x=i, y=j; //store the coordinates in x and y
                }
            }
        }
    }
    else{
       printf("Please enter a valid difficult level. \n");
       return;
    }
    //radar move code
    for(int i=x; i<x+2 && i<bot->rows; i++){
        for(int j=y; j<y+2 && j<bot->columns; j++){
            if(opponent->grid[i][j]=='X')
            {
                found=true; 
                break;
            }
        }
    }
    bot->radarCount++; //update the nb of radar moves used by the bot

    if(found)
        printf("Enemy ships found.\n");
    else
        printf("No enemy ships found.\n");
}

// bot version of smoke function
void smokeBot (struct Player *bot, struct Player *p, char difficultyLevel[]) {
    int minKnownCells;
    if(strcmp(difficultyLevel, "easy")){ //smokes at least one ship-containing cell
        minKnownCells = 1;
        srand(time(NULL));
        int row = rand() % 10;
        int col = rand() % 10;
        while (!smokeCheck(bot, row, col, minKnownCells)) {
            row = rand() % 10;
            col = rand() % 10;
        }
        for (int i=row; i<=row+1; i++) {
            for (int j=col; j<=col+1; j++) {
                bot->smokeGrid[i][j] = '~';
            }
        }
    }else if(strcmp(difficultyLevel, "medium")){ //smokes first ship containing cell encountered (guarantees two cells)
        minKnownCells = 2;
        for (int i=0; i<=9; i++) {
            for (int j=0; j<=9; j++) {
                if(smokeCheck(bot, i, j, minKnownCells)){
                    for (int row=i; row<=i+1; row++) {
                        for (int col=j; col<=j+1; col++) {
                            bot->smokeGrid[row][col] = '~';
                        }
                    }
                    return;
                }
            }
        }
    }else if(strcmp(difficultyLevel, "hard")){ //smokes most likely cell to be shot by the players (also guarantees two cells)
        minKnownCells = 2;
        char coordinates [] = findMaxProbabilityPlayer(p);
        while (!smokeCheck(bot, getRow(coordinates), convertToColumnIndex(coordinates), minKnownCells)) {
            strcpy(coordinates, findMaxProbabilityPlayer(p));
        }
        int row = getRow(coordinates);
        int col = convertToColumnIndex(coordinates[0]);
        for (int i=row; i<=row+1; i++) {
            for (int j=col; j<=col+1; j++) {
                bot->smokeGrid[i][j] = '~';
            }
        }
    }
}

// checks if the 2x2 area chosen for the bot's smoke move at random has a specific amount of cells that contain a ship
bool smokeCheck (struct Player *bot, int row, int col, int minUnknownCells) {
    int shipCellCounter = 0;
    for (int j=col; j<col+2; j++) {
        if (bot->smokeGrid[row][j]=='X') {
            shipCellCounter+=1;
        }
        if (bot->smokeGrid[row+1][j]=='X'){
            shipCellCounter+=1;
        }
    }
    if (shipCellCounter>=minUnknownCells) {
        return true;
    } else return false;
}

// chooses the bot's next move
void nextMove (struct Player *bot, struct Player *p, char difficultyLevel []) {
    srand(time(NULL));

    if (bot->unlockedArtillery) {
        int row = rand() % 10;
        int column = rand() % 10;
        int minUnknownCells;
        if (strcmp(difficultyLevel, "easy")) {
            minUnknownCells = 1;
        } else minUnknownCells = 2;
        while (!artilleryCheck(bot, row, column, minUnknownCells)) {
            row = rand() % 10;
            column = rand() % 10;
        }
        artillery(bot, p, difficultyLevel, botCoordinates(row, column));
    } else if (bot->unlockedTorpedo) {
        int o = rand() % 2;
        int t;
        char i [2];
        if (strcmp(difficultyLevel, "easy")) {
            t = (rand() % 10);
            while (!torpedoCheck(bot, t, o)) { // as long as the row/column doesn't have any unknown cells, change it
                t = (rand() % 10);
            }
        } else {
            t = leastInfo(bot, o); // get the row/column that we have the least info on
        }
        if (o==0) {
            i[0] = t + '0'; // converts it to a character
            torpedoRow(bot, p, i, difficultyLevel);
        } else {
            i[0] = convertToLetter(t);  //needs the column as a letter
            torpedoColumn(bot, p, i, difficultyLevel);
        }
    }
    // for fire move
    if (bot->isRandom==true) {
        randomFire(bot, p, difficultyLevel);
    } else fireAdjacency(bot, p, difficultyLevel);

    //for radar move
    if(bot->radarCount <= 3) //checks if bot still has radar moves
    {
        int row = rand() % 10;
        int column = rand() % 10;

        //if difficulty selected is EASY:
        if(strcmp(difficultyLevel, "easy")==0)
        {
            if(countUnknownCells(bot, row, column) >=1) //at least one cell should be unknown
            {
                radar(bot, row, column);
                bot->radarCount++; //increment radar count
            }
        }
        //if difficulty selected is MEDIUM:
        else if(strcmp(difficultyLevel, "medium")==0)
        {
            if(countUnknownCells(bot, row, column) >=2) //at least 2 unknown cells 
            {
                radar(bot, row, column);
                bot->radarCount++;
            }
        }
        //if difficulty selected is HARD:
        else if(strcmp(difficultyLevel, "hard")==0)
        {
            if(countUnknownCells(bot, row, column) >=3) //at least 3 unknown cells
            {
                radar(bot, row, column);
                bot->radarCount++;
            }
        }

}


/*void main(void)
{

    //initialize player 1
    struct Player p;
    p.bot = false;
    initialize(&p);

    //initialize bot
    struct Player BOT;
    char a [] = "BOT";
    strcpy(BOT.name, a);
    BOT.bot = true;
    initialize(&BOT);


    //asking for difficulty level
    char difficultyLevel [20];
    printf("Please enter the tracking difficulty level (easy/medium/hard) of your choice:\t");
    scanf("%s", difficultyLevel);
    toLower(difficultyLevel);
    while ((strcmp(difficultyLevel, "easy")!=0 && strcmp(difficultyLevel, "hard")!=0)) { //keeps on asking until input is valid
        printf("Invalid input. Please enter the tracking difficulty level (easy/hard) of your choice:\t");
        scanf("%s", difficultyLevel);
        toLower(difficultyLevel);
    }


    //asking for the names of the players
    printf("Please enter your name:\t");
    scanf("%s", p.name);



    //choosing who gets the first turn randomly
    struct Player* nextPlayer;
    struct Player* otherPlayer;
    srand((unsigned int)time(NULL));
    int num = rand()%2;
    if (num==0) {
        nextPlayer = &p;
        otherPlayer = &BOT;
    } else {
        nextPlayer = &BOT;
        otherPlayer = &p;
    }
    printf("%s%s\n", nextPlayer->name, " will go first."); //notifying who will have the first turn

    // place ships for bot

    //placement of ships on the grid
    while (!p.placedShips) { //asks the player for the placement of ships
        printf("%s, %s\n", nextPlayer->name, "please enter the desired coordinates as well as whether or not you want the placement to be vertical or horizontal (ex: B3 horizontal) for the following ships:");
        int counter=5; //acts as the size of the ship as well as for the sequence of the questions
        while (true) {
            if (counter==1) {
                break; //asked about the coordinates of all ships
            }
            printf("%s", getQuestion(counter));
            char orientation [11]; //vertical or horizontal
            char coordinates [4]; //column and row
            scanf("%s%s", coordinates, orientation);
            toLower(orientation);
            int col = convertToColumnIndex(coordinates[0]);
            int row = getRow(coordinates);
            if ((validCoordinates(coordinates)) && validOrientation(orientation) && (!checkBeyondGrid(&p, orientation,counter, col, row)) && (checkCellAvailability(nextPlayer, orientation, counter, col, row))){
                placeShips(nextPlayer, orientation, counter, convertToColumnIndex(coordinates[0]), getRow(coordinates));
                counter --;
            }
            printGrid(nextPlayer);
        }
        p.placedShips = true;
        system("clear");  //clears terminal on mac
    }

    while (true) {
        nextPlayer->turnCount +=1;
        if ((nextPlayer->artilleryMove>0)&&(nextPlayer->artilleryMove<nextPlayer->turnCount)) {
            nextPlayer->unlockedArtillery = false;
        }
        if ((nextPlayer->torpedoMove>0)&&(nextPlayer->torpedoMove<nextPlayer->turnCount)) {
            nextPlayer->unlockedTorpedo = false;
        }
        char move[10];
        printf("%s, what is your next move?\n\tfire\n\tradar\n\tsmoke\n\tartillery\n\ttorpedo\n", nextPlayer->name);
        scanf("%s", &move);
        toLower(move);
        while ((strcmp(move, "fire")) != 0 && (strcmp(move, "radar")) != 0 && (strcmp(move, "smoke")) != 0 && (strcmp(move, "artillery")) != 0 && (strcmp(move, "torpedo")) != 0) {
            printf("Invalid input. %s, what would you like to do?\nfire\nradar\nsmoke\nartillery\ntorpedo\n", nextPlayer->name);
            scanf("%s", &move);
            toLower(move);
        }
        char location[3];
        char coordinates[4];
        if((strcmp(move, "fire")) == 0){
            printf("Enter coordinates to fire (e.g., B3): ");
            scanf("%s", coordinates);
            if (!validCoordinates(coordinates)) {
                printf("Turn skipped.\n");
            } else fire(nextPlayer, otherPlayer, coordinates, difficultyLevel);
        }else if((strcmp(move, "radar")) == 0){
            if(nextPlayer->radarCount == 0){
                printf("%s, you do not have any more radars. Turn skipped.\n", nextPlayer->name);
                alternatePlayers(&nextPlayer,&p1,&p2);
                alternatePlayers(&otherPlayer,&p1,&p2);
                continue;
            }
            nextPlayer->radarCount--;
            printf("Where would you like to activate your radar?\n");
            scanf("%s", &location);
            if (!checkInBounds(getRow(location), convertToColumnIndex(location[0])) || !validCoordinates(location)) {
                printf("Turn skipped.\n");
            } else radar(otherPlayer, convertToColumnIndex(location[0]), getRow(location));
        }else if((strcmp(move, "smoke")) == 0) {
            if(nextPlayer->smokeCount == 0){
                printf("%s, you do not have the option to use the smoke move right now.\nIt can be used each time an opponent's ship is sunk.\nTurn skipped.\n", nextPlayer->name);
                alternatePlayers(&nextPlayer,&p1,&p2);
                alternatePlayers(&otherPlayer,&p1,&p2);
                continue;
            }
            nextPlayer->smokeCount--;
            printf("Where would you like to activate your smoke? ");
            scanf("%s", &location);
            if (!checkInBounds(getRow(location), convertToColumnIndex(location[0])) || !validCoordinates(location)) {
                printf("Turn skipped.\n");
            } else {
                smoke(nextPlayer, convertToColumnIndex(location[0]), getRow(location));
                printSmokeGrid(nextPlayer);
                system("clear"); //clears terminal on mac
            }
        }else if((strcmp(move, "artillery")) == 0){
            if (nextPlayer->nbrOfShipsSunk==1 && nextPlayer->unlockedArtillery) {
            printf("Enter top-left coordinates for artillery (e.g., B3): ");
            scanf("%s", coordinates);
                while (!validCoordinates(coordinates) || !checkInBounds(getRow(coordinates), convertToColumnIndex(coordinates[0]))) {
                    printf("Please try again.\n");
                    scanf("%s", coordinates);
                }
            artillery(nextPlayer, otherPlayer, difficultyLevel, coordinates);
            } else {
            if (nextPlayer->nbrOfShipsSunk==0) {
                printf("Artillery move is not unlocked yet.\n");
            } else printf("Artillery move is not available anymore.\n");
        }
        }else{
            char rOc [3];
            if (nextPlayer->nbrOfShipsSunk==3 && nextPlayer->unlockedTorpedo) {
                printf("Enter row/column of torpedo: ");
                scanf("%s", rOc);
                toLower(rOc);
                if (rOc[0]>='a'&& rOc[0]<='j' && rOc[1]=='\0') {
                    torpedoColumn(nextPlayer, otherPlayer, rOc, difficultyLevel);
                } else if (((rOc[0] >= '1' && rOc[0] <= '9' && rOc[1]=='\0') || (rOc[0]=='1' && rOc[1]=='0' && rOc[2]=='\0'))){
                    torpedoRow(nextPlayer, otherPlayer, rOc, difficultyLevel);
                }
            } else {
                if (nextPlayer->nbrOfShipsSunk<3) {
                    printf("Torpedo move is not unlocked yet.\n");
                } else printf("Torpedo move is not available anymore.\n");
            }
        }
    shipSunk(nextPlayer, otherPlayer);
    if(p1.BattleshipCount<1 && p1.CarrierCount<1 && p1.DestroyerCount<1 && p1.SubmarineCount<1){
        printf("Congratulations, %s. You sunk all of %s's ships!", nextPlayer->name, otherPlayer->name);
        break;
    }
    if(p2.BattleshipCount<1 && p2.CarrierCount<1 && p2.DestroyerCount<1 && p2.SubmarineCount<1){
        printf("Congratulations, %s. You sunk all of %s's ships!", nextPlayer->name, otherPlayer->name);
        break;
    }
        if (nextPlayer->artilleryMove==0 && nextPlayer->nbrOfShipsSunk==1) {
            nextPlayer->artilleryMove = (nextPlayer->turnCount)+1;
        }
        if (nextPlayer->torpedoMove==0 && nextPlayer->nbrOfShipsSunk==3) {
            nextPlayer->torpedoMove = (nextPlayer->turnCount)+1;
        }

    alternatePlayers(&nextPlayer,&p,&BOT);
    alternatePlayers(&otherPlayer,&p,&BOT);

}
}
*/
