#include "Piece.h"

//-----------------------------------------------------------------------------
// Name : Piece (constructor)
//-----------------------------------------------------------------------------
piece::piece(int playerColor, BOARD_POINT _boardPoistion) : boardPoistion(_boardPoistion)
{
    m_onStartPoint = true;
    this->color = playerColor;
}

//-----------------------------------------------------------------------------
// Name : Piece (destructor)
//-----------------------------------------------------------------------------
piece::~piece(void)
{}

//-----------------------------------------------------------------------------
// Name : setMoved 
//-----------------------------------------------------------------------------
void piece::setMoved(bool newStatus)
{
    m_onStartPoint = !newStatus;
}

//-----------------------------------------------------------------------------
// Name : setBoardPosition 
//-----------------------------------------------------------------------------
void piece::setBoardPosition(BOARD_POINT newPosition)
{
    boardPoistion = newPosition;
}

//-----------------------------------------------------------------------------
// Name : getColor 
//-----------------------------------------------------------------------------
int piece::getColor()
{
    return color;
}

//-----------------------------------------------------------------------------
// Name : getPosition 
//-----------------------------------------------------------------------------
BOARD_POINT piece::getPosition()
{
    return boardPoistion;
}

//-----------------------------------------------------------------------------
// Name : isMoved 
//-----------------------------------------------------------------------------
bool piece::isMoved()
{
    return !m_onStartPoint;
}

//-----------------------------------------------------------------------------
// Name : getDx 
//-----------------------------------------------------------------------------
void piece::getDx(int inputArray[])
{
    inputArray[0] = dx[0];
    inputArray[1] = dx[1];
}

//-----------------------------------------------------------------------------
// Name : getDy 
//-----------------------------------------------------------------------------
void piece::getDy(int inputArray[])
{
    inputArray[0] = dy[0];
    inputArray[1] = dy[1];
}


//-----------------------------------------------------------------------------
// Name : isPieceInWay 
//-----------------------------------------------------------------------------
bool piece::isPieceInWay(BOARD_POINT startSquare, BOARD_POINT targetSquare, DIR_VEC dirVec, piece * sBoard[boardY][boardX])
{
    BOARD_POINT curSquare = startSquare;
    piece * curPiece;

    curSquare.col -= dirVec.y;
    curSquare.row -= dirVec.x;

    while (curSquare.col != targetSquare.col || curSquare.row != targetSquare.row)
    {
        curPiece = sBoard[curSquare.col][curSquare.row];//gets the piece on the current square
        if (curPiece != nullptr)//if there is a piece return false as there should be no piece in the way to the target square
            return false;

        curSquare.col -= dirVec.y;
        curSquare.row -= dirVec.x;
    }

    return true;
} 
