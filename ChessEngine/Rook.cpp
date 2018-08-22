#include "Rook.h"
#include <cmath>

PIECES rook::type = ROOK;
//-----------------------------------------------------------------------------
// Name : Rook (constructor)
//-----------------------------------------------------------------------------
rook::rook(void)
{
}

//-----------------------------------------------------------------------------
// Name : Rook (constructor)
//-----------------------------------------------------------------------------
rook::rook(int playerColor)
    :piece(playerColor)
{
    dx[0] = -1;
    dx[1] =  2;
    dy[0] = dx[0];
    dy[1] = dx[1];
}

//-----------------------------------------------------------------------------
// Name : Rook (destructor)
//-----------------------------------------------------------------------------
rook::~rook(void)
{
}

//-----------------------------------------------------------------------------
// Name : validateNewPos
//-----------------------------------------------------------------------------
bool rook::validateNewPos(int dx,int dy, BOARD_POINT startSqaure, BOARD_POINT targetSquare,piece * SBoard[boardY][boardX])
{
    if (dx == 0 && dy == 0)//if both dx and dy are 0 than there was no movement therefore move is illegal
        return false;

    if (dx * dy == 0)// if dx * dy = 0 it means we are moving horizontally or vertically
    {
        piece * targetPiece = SBoard[targetSquare.col][targetSquare.row];

        DIR_VEC dirVec;
        if (dx != 0)
            dirVec.x = dx / abs(dx);
        else
            dirVec.x = 0;

        if (dy != 0)
            dirVec.y = dy / abs(dy);
        else
            dirVec.y = 0;

        if (targetPiece != NULL)//check if there is a piece on the target square
        {
            if (targetPiece->getColor() != this->getColor()) //check if it is belong to the other player so we can eat it 
            {
                return isPieceInWay(startSqaure,targetSquare,dirVec,SBoard);//check that there was no piece in the middle of the way
            }
        }
        else//else square is empty just need to check that there was no piece in the middle of the way
        {
            return isPieceInWay(startSqaure,targetSquare,dirVec,SBoard);//check that there was no piece in the middle of the way
        }
    }
    
    return false;
}

//-----------------------------------------------------------------------------
// Name : getType
//-----------------------------------------------------------------------------
int rook::getType()
{
    return rook::type;
}

// float rook::getYpos()
// {
// 	return rook::Ypos;
// }
