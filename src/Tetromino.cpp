#include "Tetromino.h"

Tetromino::Tetromino() : x(WIDTH / 2 - 2), y(0) {
}

int Tetromino::getColor() const {
    return this->color;
}
void Tetromino::setColor(int color) {
    this->color = color;
}

int Tetromino::getX() const {
    return this->x;
}

void Tetromino::setX(int x) {
    this->x = x;
}

int Tetromino::getY() const {
    return y;
}

void Tetromino::setY(int y) {
    this->y = y;
}

vector<vector<int>> &Tetromino::getShape() {
    return this->shape;
}

void Tetromino::setShape(vector<vector<int>> update) {
    this->shape = update;
}