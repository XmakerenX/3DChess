#ifndef _BISHOP_H_
#define _BISHOP_H_

#include "Piece.h"

class bishop : public piece
{
public:
    bishop(void);
    bishop(int playerColor);
    ~bishop(void);
    virtual bool validateNewPos(int dx,int dy, BOARD_POINT startSqaure, BOARD_POINT targetSquare,piece * SBoard[boardY][boardX]);
    int   getType();
// 	float getYpos();
// 
// 	static float    Ypos;

private:
    static PIECES type;
};

#endif // _BISHOP_H_
 
