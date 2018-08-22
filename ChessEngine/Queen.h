#ifndef _QUEEN_H_
#define _QUEEN_H_

#include "Bishop.h"
#include "Rook.h"

class queen : public piece
{
public:
    queen(void);
    queen(int playerColor);
    bool validateNewPos(int dx,int dy, BOARD_POINT startSqaure, BOARD_POINT targetSquare,piece * SBoard[boardY][boardX]);
    ~queen(void);
    int getType();
//     float getYpos();
// 
//     static float    Ypos;

private:
	static PIECES type;
};
 
#endif // _QUEEN_H_
