#ifndef _KING_H_
#define _KING_H_

#include "Piece.h"

class king : public piece
{
public:
    king(void);
    king(int playerColor);
    ~king(void);
    bool validateNewPos(int dx,int dy, BOARD_POINT startSqaure, BOARD_POINT targetSquare,piece * SBoard[boardY][boardX]);
    int   getType();
//     float getYpos();
// 
//     static float  Ypos;

private:
    static PIECES type;
};

#endif // _KING_H_
 
