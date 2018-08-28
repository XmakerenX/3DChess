#ifndef _CHESS_H_ 
#define _CHESS_H_

const int boardX = 8;
const int boardY = 8;

enum PIECES{PAWN,KNIGHT,BISHOP,ROOK,QUEEN,KING};

struct BOARD_POINT  
{
    BOARD_POINT(unsigned int _row, unsigned int _col)
    {
        row = _row;
        col = _col;
    }
    
    BOARD_POINT(const BOARD_POINT& toCopy)
    {
        row = toCopy.row;
        col = toCopy.col;
    }
    
    unsigned int row;   //j
    unsigned int col;   //i
};

struct DIR_VEC
{
    DIR_VEC()
    {
        ;
    }

    DIR_VEC(int newX, int newY)
    {
        x = newX;
        y = newY;
    }

    int x;
    int y;
};

#endif // _PIECE_H_ 
