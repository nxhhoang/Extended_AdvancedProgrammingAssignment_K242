#ifndef BOARD_H
#define BOARD_H

#include "ConstVariable.h"

class Board {
private:
    VectorMatrix board; 
    int row;
    int col;
public:
    Board();
    void setBoardXY(int y, int x, int val);
    int getBoardXY(int y, int x);
    void setBoard(VectorMatrix tmp);    
    void RemoveRow(int k);
    void resetRow(int k);
    VectorMatrix &getBoard();
};


#endif