#include "King.h"
#include <cmath>

PIECES king::type = KING;
//-----------------------------------------------------------------------------
// Name : King (constructor)
//-----------------------------------------------------------------------------
// king::king(void)
// {
// }

//-----------------------------------------------------------------------------
// Name : King (constructor)
//-----------------------------------------------------------------------------
king::king(int playerColor, BOARD_POINT _boardPoistion)
    :piece(playerColor, _boardPoistion)
{
    dx[0]=-1;
    dx[1]=2;
    dy[0]=dx[0];
    dy[1]=dx[1];
}

//-----------------------------------------------------------------------------
// Name : King (destructor)
//-----------------------------------------------------------------------------
king::~king(void)
{
}

//-----------------------------------------------------------------------------
// Name : validateNewPos (destructor)
//-----------------------------------------------------------------------------
bool king::validateNewPos(int dx,int dy, BOARD_POINT startSqaure, BOARD_POINT targetSquare,piece * SBoard[boardY][boardX])
{
    piece * targetPiece = SBoard[targetSquare.col][targetSquare.row];

    if(dx == 0 && dy ==0 )
        return false;

    if (abs(dx)<= 1 && abs(dy) <= 1)
    {
        if (targetPiece != NULL)
        {
            if(targetPiece->getColor() != this->getColor())
                return true;
            else
                return false;
        }
        
        return true;
    }

    if ((dx == -2 || dx == 3) && dy == 0 && m_onStartPoint) //check if the player tried to do a castling move
    {
        DIR_VEC castlingDir;
        castlingDir.x = dx / abs(dx);
        castlingDir.y = 0;
        return isPieceInWay(startSqaure,targetSquare,castlingDir,SBoard);
    }

    return false;
}

//-----------------------------------------------------------------------------
// Name : getType
//-----------------------------------------------------------------------------
int king::getType()
{
	return king::type;
}

// float king::getYpos()
// {
// 	return king::Ypos;
// } 
