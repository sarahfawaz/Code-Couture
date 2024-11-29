#ifndef PLAYER_H
#define PLAYER_H
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
#endif
