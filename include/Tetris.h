#ifndef TETRIS_H
#define TETRIS_H

#include "Tetromino.h"
#include "Board.h"

class Tetris {
private:
    Board gameBoard;
    Tetromino current;
    Tetromino nextT;

    string warning = "\033[36mYour name should not have more than 10 characters\033[0m";
    int score = 0;
    int updateScoreSuccess = 0;
    bool paused = 0;

    string name = "";
    string tmpName = "";
    string hello = "Hello ";

    int nameIter = 5;
    int width_score = 16;
    int glb = 0;
    int sizeTable = 3;
    int maxSizeTable = 6;
    int speed = 400;

public:
    Tetris();

    void Playing();
    void InsertName();
    void LevelAndContinue();
    void spawnTetromino();
    void spawnNextTetromino();
    void setTetromino(int fid, int fcolor, int nid, int ncolor);

    void handleInput();
    void merge();
    void handlePause();
    bool collides(int x, int y);
    string movePtr(int val);
    int handleEnter();

    void clearLines();
    void clearInputBuffer();
    int Option();
    void Help();

    void rotate();
    string centerWithColor(const string& content, int totalWidth);
    void outsider(int i);
    void draw();
    void SaveGame();
    void GameOver();
};

#endif