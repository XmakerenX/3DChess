#include "Bishop.h"
#include <cmath>

//initialization of static vars
PIECES bishop::type = BISHOP;
//-----------------------------------------------------------------------------
// Name : Bishop (constructor)
//-----------------------------------------------------------------------------
// bishop::bishop(void)
// {
// }

//-----------------------------------------------------------------------------
// Name : Bishop (constructor)
//-----------------------------------------------------------------------------
bishop::bishop(int playerColor, BOARD_POINT _boardPoistion)
    :piece(playerColor, _boardPoistion)
{
    dx[0] = -1;
    dx[1] =  2;
    dy[0] = dx[0];
    dy[1] = dx[1];
}

//-----------------------------------------------------------------------------
// Name : Bishop (destructor)
//-----------------------------------------------------------------------------
bishop::~bishop(void)
{
}

//-----------------------------------------------------------------------------
// Name : validateNewPos
//-----------------------------------------------------------------------------
bool bishop::validateNewPos(int dx,int dy, BOARD_POINT startSqaure, BOARD_POINT targetSquare,piece * SBoard[boardY][boardX])
{
    if (dx == 0 || dy == 0)
        return false;

    //check that dx and dy are 1,1 or 2,2 and etc
    if ( std::abs(dx / dy) == 1 && dx % dy == 0)
    {
        piece * targetPiece = SBoard[targetSquare.col][targetSquare.row];
        
        DIR_VEC dirVec;
        dirVec.x = dx / std::abs(dx);
        dirVec.y = dy / std::abs(dy);

        //check if there is a piece on the target square
        if (targetPiece != nullptr) 
        {
            //check if it is belong to the other player so we can eat it 
            if (targetPiece->getColor() != this->getColor()) 
            {
                //check that there was no piece in the middle of the way
                return isPieceInWay(startSqaure,targetSquare,dirVec,SBoard); 
            }
        }
        //else square is empty just need to check that there was no piece in the middle of the way
        else 
        {
            //check that there was no piece in the middle of the way
            return isPieceInWay(startSqaure,targetSquare,dirVec,SBoard);
        }
    }
    
    return false;
}

//-----------------------------------------------------------------------------
// Name : validateNewPos
//-----------------------------------------------------------------------------
int bishop::getType()
{
    return bishop::type;
}

// float bishop::getYpos()
// {
// 	return bishop::Ypos;
// } 
