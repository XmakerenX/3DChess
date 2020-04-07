#ifndef _QUEEN_H_
#define _QUEEN_H_

#include "Bishop.h"
#include "Rook.h"

class queen : public piece
{
public:
    queen(int playerColor, BOARD_POINT _boardPoistion);
    bool validateNewPos(int dx,int dy, BOARD_POINT startSqaure, BOARD_POINT targetSquare,piece * SBoard[boardY][boardX]);
    ~queen(void);
    int getType();
    
private:
	static PIECES type;
};
 
#endif // _QUEEN_H_
