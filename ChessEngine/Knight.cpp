#include "Knight.h"
#include <cmath>

PIECES knight::type = KNIGHT;
//-----------------------------------------------------------------------------
// Name : knight (constructor)
//-----------------------------------------------------------------------------
knight::knight(void)
{
}

//-----------------------------------------------------------------------------
// Name : knight (constructor)
//-----------------------------------------------------------------------------
knight::knight(int playerColor)
    :piece(playerColor)
{
    dx[0]=-2;
    dx[1]=3;
    dy[0]=-2;
    dy[1]=3;
}

//-----------------------------------------------------------------------------
// Name : knight (destructor)
//-----------------------------------------------------------------------------
knight::~knight(void)
{
}

//-----------------------------------------------------------------------------
// Name : validateNewPos
//-----------------------------------------------------------------------------
bool knight::validateNewPos(int dx,int dy, BOARD_POINT startSqaure, BOARD_POINT targetSquare,piece * SBoard[boardY][boardX])
{
    piece * targetPiece = SBoard[targetSquare.col][targetSquare.row];

    if (dx==0 || dy==0)
        return false;

    if (( std::abs(dx) + std::abs(dy) == 3))
    {
        if (targetPiece != NULL)
        {
            if(targetPiece->getColor() != this->getColor())
                return true;
            else
                return false;
        }
        else
            return true;
    }
    
    return false;
}

//-----------------------------------------------------------------------------
// Name : getType
//-----------------------------------------------------------------------------
int knight::getType()
{
    return knight::type;
}

// float knight::getYpos()
// {
// 	return knight::Ypos;
// } 
