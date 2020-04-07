#ifndef _ROOK_H_
#define _ROOK_H_

#include "Piece.h"

class rook : public piece
{
public:
    rook(int playerColor, BOARD_POINT _boardPoistion);
    ~rook(void);
    virtual bool validateNewPos(int dx,int dy, BOARD_POINT startSqaure, BOARD_POINT targetSquare,piece * SBoard[boardY][boardX]);
    int   getType();

private:
	static PIECES type;
};

#endif // _ROOK_H_
 
