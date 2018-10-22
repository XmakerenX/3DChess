#ifndef _BISHOP_H_
#define _BISHOP_H_

#include "Piece.h"

class bishop : public piece
{
public:
    bishop(int playerColor, BOARD_POINT _boardPoistion);
    ~bishop(void);
    virtual bool validateNewPos(int dx,int dy, BOARD_POINT startSqaure, BOARD_POINT targetSquare,piece * SBoard[boardY][boardX]);
    int   getType();

private:
    static PIECES type;
};

#endif // _BISHOP_H_
 
