#ifndef _KNIGHT_H_
#define _KNIGHT_H_

#include "Piece.h"

class knight : public piece
{
public:
    knight(int playerColor, BOARD_POINT _boardPoistion);
    ~knight(void);
    bool validateNewPos(int dx,int dy, BOARD_POINT startSqaure, BOARD_POINT targetSquare,piece * SBoard[boardY][boardX]);
    int   getType();
// 	float getYpos();
// 
// 	static float    Ypos;

private:
    static PIECES   type;
};

#endif // _KNIGHT_H_
