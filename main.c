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
};

//returns true if ship has been sunk and false if it was not
void shipSunk (struct Player *p, struct Player *p1) {
    if (p->CarrierCount==0) {
        p->nbrOfShipsSunk+=1;
        p->CarrierCount=-1;
        printf("Congrats! You have sunk %s's Carrier!", p1->name);
    } else if (p->BattleshipCount==0) {
        p->nbrOfShipsSunk+=1;
        p->BattleshipCount=-1;
        printf("Congrats! You have sunk %s's Battleship!", p1->name);
    } else if (p->DestroyerCount==0) {
        p->nbrOfShipsSunk+=1;
        p->DestroyerCount=-1;
        printf("Congrats! You have sunk %s's Destroyer!", p1->name);
    } else if (p->SubmarineCount==0) {
        p->nbrOfShipsSunk+=1;
        p->SubmarineCount=-1;
        printf("Congrats! You have sunk %s's Submarine!", p1->name);
    }
}


// alternates players according to their turn
void alternatePlayers(struct Player **p, struct Player *p1, struct Player *p2) {
    if (*p == p1) {
        *p = p2;
    } else *p = p1;
}


// initializes the grid
void initializeGrid (struct Player *p) {
    p->CarrierCount = 5;
    p->BattleshipCount = 4;
    p->DestroyerCount = 3;
    p->SubmarineCount = 2;
    for (int i=0; i<p->rows; i++) {
        for (int j=0; j<p->columns; j++) {
            p->grid[i][j]='~';
            p->smokeGrid[i][j]='~';
        }
    }
}

void initializeDisplayedGrid (struct Player *p) {
    for (int i=0; i<p->rows; i++) {
        for (int j=0; j<p->columns; j++) {
            p->displayedGrid[i][j]='~';
        }
    }
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

// prints the smoked grid
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


// check if orientation is valid
bool validOrientation (char o []) {
    if ((strcmp(o, "horizontal")==0) || (strcmp(o, "vertical")==0)) {
        return true;
    } else {
        printf("Invalid orientation.\n");
        return false;
    }
}


// checks if the placement of the ship is beyond the boundaries of the grid or not
bool checkBeyondGrid (char orientation [], int nbrOfCells, int col, int row) {
    if ((strcmp(orientation, "horizontal")==0 && ((col+nbrOfCells)-1)>9) || (strcmp(orientation, "vertical")==0 && (((row)+nbrOfCells)-1)>9)) {
        printf("Coordinates chosen result in an out-of-bounds placement of the ship. Please enter new coordinates.\n");
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
                printf("Coordinates chosen are already occupied. Please enter new coordinates.\n");
                return false;
            }
        }
    } else {
        int j=col;
        for (int i=row; i<(row+nbrOfCells); i++) {
            if (p->grid[i][j]=='X') {
                printf("Coordinates chosen are already occupied. Please enter new coordinates.\n");
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
            p->shipGrid[i][j] = nbrOfCells;//each ship will have uniquely-valued cells
        }
    } else {
        int j=col;
        for (int i=row; i<(row+nbrOfCells); i++) {
            p->grid[i][j]='X';
            p->smokeGrid[i][j]='X';
            p->shipGrid[i][j] = nbrOfCells;
        }
    }
}


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


// will return true if it hit, false if it missed
bool HitOrMiss (struct Player *p, struct Player *p1,  char coordinates [], char difficultyLevel []) {
    if (validCoordinates(coordinates)) {
        int row = getRow(coordinates);
        int col = convertToColumnIndex(coordinates[0]);
        if (p1->grid[row][col]=='~') {
            if (strcmp(difficultyLevel, "easy")==0) {
                p->displayedGrid[row][col]='o'; //changes display on grid if missed and if in easy mode
            }
            p1->grid[row][col]='o';
            return false;
        } else if (p1->grid[row][col]=='X') {
            p->displayedGrid[row][col]='*'; //changes display on grid to hit in both modes
            decrementShipTypeCounter(p, p1->shipGrid[row][col]);
            return true;
        } else return false; //if it already was missed before
    }
}

// radar looks for ships in 2x2 area
void radar (struct Player *p, int col, int row, int position) {
    if(position == 1){
        if (p->smokeGrid[row][col] == 'X') {
            printf("Enemy ships found\n");
            return;
        }
        if (col < 9) {
            if (p->smokeGrid[row][col + 1] == 'X') {
                printf("Enemy ships found\n");
                return;
            }
        }
        if (row < 9) {
            if (p->smokeGrid[row + 1][col] == 'X') {
                printf("Enemy ships found\n");
                return;
            }
        }
        if (col < 9 && row < 9) {
            if (p->smokeGrid[row + 1][col + 1] == 'X') {
                printf("Enemy ships found\n");
                return;
            }
        }
    }else if(position == 2){
        if (p->smokeGrid[row][col] == 'X') {
            printf("Enemy ships found\n");
            return;
        }
        if (col > 1) {
            if (p->smokeGrid[row][col - 1] == 'X') {
                printf("Enemy ships found\n");
                return;
            }
        }
        if (row < 9) {
            if (p->smokeGrid[row + 1][col] == 'X') {
                printf("Enemy ships found\n");
                return;
            }
        }
        if (col > 1 && row < 9) {
            if (p->smokeGrid[row + 1][col - 1] == 'X') {
                printf("Enemy ships found\n");
                return;
            }
        }
    }else if(position == 3){
        if (p->smokeGrid[row][col] == 'X') {
            printf("Enemy ships found\n");
            return;
        }
        if (col < 9) {
            if (p->smokeGrid[row][col + 1] == 'X') {
                printf("Enemy ships found\n");
                return;
            }
        }
        if (row > 1) {
            if (p->smokeGrid[row - 1][col] == 'X') {
                printf("Enemy ships found\n");
                return;
            }
        }
        if (col < 9 && row > 1) {
            if (p->smokeGrid[row - 1][col + 1] == 'X') {
                printf("Enemy ships found\n");
                return;
            }
        }
    }else{
        if (p->smokeGrid[row][col] == 'X') {
            printf("Enemy ships found\n");
            return;
        }
        if (col > 1) {
            if (p->smokeGrid[row][col - 1] == 'X') {
                printf("Enemy ships found\n");
                return;
            }
        }
        if (row > 1) {
            if (p->smokeGrid[row - 1][col] == 'X') {
                printf("Enemy ships found\n");
                return;
            }
        }
        if (col > 1 && row > 1) {
            if (p->smokeGrid[row - 1][col - 1] == 'X') {
                printf("Enemy ships found\n");
                return;
            }
        }
    }

    printf("No enemy ships found\n");
}

// smoke hides ships from radars in 2x2 area
void smoke (struct Player *p, int col, int row, int position) {
    if(position = 1){
        p->smokeGrid[row][col] = '~';
        if (row < 9) {
            p->smokeGrid[row + 1][col] = '~';
        }
        if (col < 9) {
            p->smokeGrid[row][col + 1] = '~';
        }
        if (col < 9 && row < 9) {
            p->smokeGrid[row + 1][col + 1] = '~';
        }
    }else if(position = 2){
        p->smokeGrid[row][col] = '~';
        if (row < 9) {
            p->smokeGrid[row + 1][col] = '~';
        }
        if (col > 1) {
            p->smokeGrid[row][col - 1] = '~';
        }
        if (col > 1 && row < 9) {
            p->smokeGrid[row + 1][col - 1] = '~';
        }
    }else if(position = 3){
        p->smokeGrid[row][col] = '~';
        if (row > 1) {
            p->smokeGrid[row - 1][col] = '~';
        }
        if (col < 9) {
            p->smokeGrid[row][col + 1] = '~';
        }
        if (col < 9 && row > 1) {
            p->smokeGrid[row - 1][col + 1] = '~';
        }
    }else if(position = 4){
        p->smokeGrid[row][col] = '~';
        if (row > 1) {
            p->smokeGrid[row - 1][col] = '~';
        }
        if (col > 1) {
            p->smokeGrid[row][col - 1] = '~';
        }
        if (col > 1 && row > 1) {
            p->smokeGrid[row - 1][col - 1] = '~';
        }
    }
}

void fire(struct Player *nextPlayer, struct Player *otherPlayer, char coordinates[], char difficultyLevel[]) {

    if (HitOrMiss(nextPlayer, otherPlayer, coordinates, difficultyLevel)) {

        printf("Hit!\n");

    } else {

        printf("Miss!\n");

    }

    printGrid(otherPlayer);

}

bool artilleryCheck = false;

void unlockArtillery() {
    artilleryCheck = true;
}

void artillery(struct Player *nextPlayer, struct Player *otherPlayer, char difficultyLevel[], char coordinates[]) {
    if (!artilleryCheck) {
        printf("Artillery is still locked.\n");
        return;
    }

    int row = getRow(coordinates);

    int col = convertToColumnIndex(coordinates[0]);

    char cellCoordinates[4];

    cellCoordinates[0] = 'A' + col;

    if (row + 1 >= 10) {
        cellCoordinates[1] = '1';
        cellCoordinates[2] = '0' + (row - 9);
        cellCoordinates[3] = '\0';
    } else {
        cellCoordinates[1] = '1' + row;
        cellCoordinates[2] = '\0';
    }
    fire(nextPlayer, otherPlayer, cellCoordinates, difficultyLevel);

    cellCoordinates[0] = 'A' + col + 1;

    fire(nextPlayer, otherPlayer, cellCoordinates, difficultyLevel);

    cellCoordinates[0] = 'A' + col;

    if (row + 2 >= 10) {
        cellCoordinates[1] = '1';
        cellCoordinates[2] = '0' + (row - 9 + 1);
        cellCoordinates[3] = '\0';
    } else {
        cellCoordinates[1] = '1' + row + 1;
        cellCoordinates[2] = '\0';
    }

    fire(nextPlayer, otherPlayer, cellCoordinates, difficultyLevel);

    cellCoordinates[0] = 'A' + col + 1;

    fire(nextPlayer, otherPlayer, cellCoordinates, difficultyLevel);

    artilleryCheck = false;
}


bool torpedoCheck = false;
void unlockTorpedo() {
    torpedoCheck = true;
}

void torpedo(struct Player *nextPlayer, struct Player *otherPlayer, char difficultyLevel[], char coordinates[]) {
    if (!torpedoCheck) {
        printf("Torpedo is still locked.\n");
        return;
    }

    torpedoCheck = false;

    char cellCoordinates[4];

    if (coordinates[0] >= 'A' && coordinates[0] <= 'J') {

        int col = convertToColumnIndex(coordinates[0]);

        for (int row = 0; row < otherPlayer->rows; row++) {
            cellCoordinates[0] = 'A' + col;
            if (row + 1 >= 10) {
                cellCoordinates[1] = '1';
                cellCoordinates[2] = '0' + (row - 9);
                cellCoordinates[3] = '\0';
            } else {
                cellCoordinates[1] = '1' + row;
                cellCoordinates[2] = '\0';
            }
            fire(nextPlayer, otherPlayer, cellCoordinates, difficultyLevel);
        }
    } else {

        int row = getRow(coordinates);

        for (int col = 0; col < otherPlayer->columns; col++) {

            cellCoordinates[0] = 'A' + col;

            if (row + 1 >= 10) {
                cellCoordinates[1] = '1';
                cellCoordinates[2] = '0' + (row - 9);
                cellCoordinates[3] = '\0';
            } else {
                cellCoordinates[1] = '1' + row;
                cellCoordinates[2] = '\0';
            }
            fire(nextPlayer, otherPlayer, cellCoordinates, difficultyLevel);
        }
    }
}

void main(void)
{

    //initialize player 1
    struct Player p1;
    p1.rows = 10;
    p1.columns = 10;
    p1.placedShips = false; //did not place ships yet
    p1.nbrOfShipsSunk = 0;
    p1.radarCount = 3;
    p1.smokeCount = 0;
    initializeGrid(&p1);
    initializeDisplayedGrid(&p1);


    //initialize player 2
    struct Player p2;
    p2.rows = 10;
    p2.columns = 10;
    p2.placedShips = false;
    p2.nbrOfShipsSunk = 0;
    p2.radarCount = 3;
    p1.smokeCount = 0;
    initializeGrid(&p2);
    initializeDisplayedGrid(&p2);


    //asking for difficulty level
    char difficultyLevel [20];
    printf("Please enter the tracking difficulty level (easy/hard) of your choice:\t");
    scanf("%s", difficultyLevel);
    toLower(difficultyLevel);
    while ((strcmp(difficultyLevel, "easy")!=0 && strcmp(difficultyLevel, "hard")!=0)) { //keeps on asking until input is valid
        printf("Invalid input. Please enter the tracking difficulty level (easy/hard) of your choice:\t");
        scanf("%s", difficultyLevel);
        toLower(difficultyLevel);
    }


    //asking for the names of the players
    printf("Player 1, please enter your name:\t");
    scanf("%s", p1.name);
    printf("Player 2, please enter your name:\t");
    scanf("%s", p2.name);


    //choosing who gets the first turn randomly
    struct Player* nextPlayer;
    struct Player* otherPlayer;
    srand((unsigned int)time(NULL));
    int num = rand()%2;
    if (num==0) {
        nextPlayer = &p1;
        otherPlayer = &p2;
    } else {
        nextPlayer = &p2;
        otherPlayer = &p1;
    }
    printf("%s%s\n", nextPlayer->name, " will go first."); //notifying who will have the first turn


    //placement of ships on the grid
    while (!p1.placedShips || !p2.placedShips) { //asks both players starting with the chosen one for the placement of ships
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
            if ((validCoordinates(coordinates)) && validOrientation(orientation) && (!checkBeyondGrid(orientation,counter, col, row)) && (checkCellAvailability(nextPlayer, orientation, counter, col, row))){
                placeShips(nextPlayer, orientation, counter, convertToColumnIndex(coordinates[0]), getRow(coordinates));
                counter --;
            }
            printGrid(nextPlayer);
        }
        nextPlayer->placedShips = true;
        alternatePlayers(&nextPlayer,&p1,&p2);
    }

    while (true) {
        char move[10];
        printf("%s, what is your nextt move?\n\tfire\n\tradar\n\tsmoke\n\tartillery\n\ttorpedo\n", nextPlayer->name);
        scanf("%s", &move);
        toLower(move);
        while ((strcmp(move, "fire")) != 0 && (strcmp(move, "radar")) != 0 && (strcmp(move, "smoke")) != 0 && (strcmp(move, "artillery")) != 0 && (strcmp(move, "torpedo")) != 0) {
            printf("Invalid input. %s, what would you like to do?\nfire\nradar\nsmoke\nartillery\ntorpedo", nextPlayer->name);
            scanf("%s", &move);
            toLower(move);
        }
        char location[3];
        int position = 0;
        char coordinates[4];
        if((strcmp(move, "fire")) == 0){
                printf("Enter coordinates to fire (e.g., B3): ");
                scanf("%s", coordinates);
                fire(nextPlayer, otherPlayer, coordinates, difficultyLevel);
        }else if((strcmp(move, "radar")) == 0){
            if(nextPlayer->radarCount == 0){
                printf("%s, you do not have any more radars. Turn skipped.", nextPlayer->name);
                alternatePlayers(&nextPlayer,&p1,&p2);
                alternatePlayers(&otherPlayer,&p1,&p2);
                continue;
            }
            nextPlayer->radarCount--;
            printf("Where would you like to activate your radar?\n");
            scanf("%s", &location);
            printf("In which position is your selection?\n(1)top-left\n(2)top-right\n(3)bottom-left\n(4)bottom-right\n");
            scanf("%d", &position);
            while(position < 1 || position > 4){
                printf("Invalid input. In which position is your selection?\n(1)top-left\n(2)top-right\n(3)bottom-left\n(4)bottom-right\n");
                scanf("%d", &position);
            }
            radar(otherPlayer, convertToColumnIndex(location[0]), getRow(location), position);
        }else if((strcmp(move, "smoke")) == 0){
            if(nextPlayer->smokeCount == 0){
                printf("%s, you do not have any smokes right now. Turn skipped.", nextPlayer->name);
                alternatePlayers(&nextPlayer,&p1,&p2);
                alternatePlayers(&otherPlayer,&p1,&p2);
                continue;
            }
            nextPlayer->smokeCount--;
            printf("Where would you like to activate your smoke?");
            scanf("%s", &location);
            printf("In which position is your selection?\n(1)top-left\n(2)top-right\n(3)bottom-left\n(4)bottom-right\n");
            scanf("%d", &position);
            while(position < 1 || position > 4){
                printf("Invalid input. In which position is your selection?\n(1)top-left\n(2)top-right\n(3)bottom-left\n(4)bottom-right\n");
                scanf("%d", &position);
            }
            smoke(nextPlayer, convertToColumnIndex(location[0]), getRow(location), position);
            printSmokeGrid(nextPlayer);
        }else if((strcmp(move, "artillery")) == 0){
            if (artilleryCheck) {
            printf("Enter top-left coordinates for artillery (e.g., B3): ");
            scanf("%s", coordinates);
            artillery(nextPlayer, otherPlayer, difficultyLevel, coordinates);
        } else {
            printf("Artillery is not unlocked yet.\n");
        }
        }else{
            if (torpedoCheck) {
            printf("Enter row or column for torpedo attack (e.g., B for column or 3 for row): ");
            scanf("%s", coordinates);
            torpedo(nextPlayer, otherPlayer, difficultyLevel, coordinates);
        } else {
            printf("Torpedo is not unlocked yet.\n");
        }
        alternatePlayers(&nextPlayer,&p1,&p2);
        alternatePlayers(&otherPlayer,&p1,&p2);
        }

}
}
