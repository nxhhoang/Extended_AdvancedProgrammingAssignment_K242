#include "Board.h"

Board::Board()
{
    board = VectorMatrix(HEIGHT + 2, vector<int>(WIDTH + 2, 0));
    row = HEIGHT + 2;
    col = WIDTH + 2;
}

void Board::setBoardXY(int x, int y, int val)
{
    if (x < 0 || y < 0 || x >= row || y >= col)
        return;
    board[x][y] = val;
}

int Board::getBoardXY(int x, int y)
{
    if (x < 0 || y < 0 || x >= row || y >= col)
        return 0;
    return board[x][y];
}

void Board::setBoard(VectorMatrix tmp)
{
    board = tmp;
}

void Board::RemoveRow(int k) {
    board[k] = board[k - 1];
}

void Board::resetRow(int k) {
    board[k] = vector<int>(WIDTH, 0); 
}

VectorMatrix &Board::getBoard()
{
    return board;
}