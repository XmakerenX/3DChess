#ifndef _KING_H_
#define _KING_H_

#include "Piece.h"

class king : public piece
{
public:
    king(int playerColor, BOARD_POINT _boardPoistion);
    ~king(void);
    bool validateNewPos(int dx,int dy, BOARD_POINT startSqaure, BOARD_POINT targetSquare,piece * SBoard[boardY][boardX]);
    int   getType();

private:
    static PIECES type;
};

#endif // _KING_H_
 
