#ifndef _ROOK_H_
#define _ROOK_H_

#include "Piece.h"

class rook : public piece
{
public:
    rook(void);
    rook(int playerColor);
    ~rook(void);
    virtual bool validateNewPos(int dx,int dy, BOARD_POINT startSqaure, BOARD_POINT targetSquare,piece * SBoard[boardY][boardX]);
    int   getType();
//     float getYpos();
// 
//     static float    Ypos;

private:
	static PIECES type;
};

#endif // _ROOK_H_
 
