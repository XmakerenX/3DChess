#ifndef _PAWN_H_
#define _PAWN_H_

#include "Piece.h"

class Pawn : public piece
{
public:
	Pawn(void);
	Pawn(int playerColor);
	~Pawn(void);
	bool validateNewPos(int dx,int dy, BOARD_POINT startSqaure, BOARD_POINT targetSquare,piece * SBoard[boardY][boardX]);
	int   getType();
	//float getYpos();

	//static float Ypos;

private:
	static PIECES type;
};
 
#endif // _PAWN_H_
