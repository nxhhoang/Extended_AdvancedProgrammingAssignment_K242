#ifndef TETROMINO_H
#define TETROMINO_H

#include "ConstVariable.h"

enum TetrominoType { I, O, T, L, J, S, Z };

class Tetromino {
private:
    VectorMatrix shape;
    int uid;
    int color;
    int x;
    int y;

public:
    Tetromino();

    int getColor() const;
    void setColor(int color);
    
    int getID() const;
    void setID(int id);

    int getX() const;
    void setX(int x);
    
    int getY() const;
    void setY(int y);    

    VectorMatrix &getShape();
    void setShape(VectorMatrix &update);
};

#endif