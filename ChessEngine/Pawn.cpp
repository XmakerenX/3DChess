#include "Pawn.h"
#include <cmath>

PIECES Pawn::type = PAWN;
//-----------------------------------------------------------------------------
// Name : Pawn (constructor)
//-----------------------------------------------------------------------------
Pawn::Pawn(int playerColor, BOARD_POINT _boardPoistion)
    :piece(playerColor, _boardPoistion)
{
    dx[0]=-1;
    dx[1]=2;
    dy[0]=-2;
    dy[1]=3;
}

//-----------------------------------------------------------------------------
// Name : Pawn (destructor)
//-----------------------------------------------------------------------------
Pawn::~Pawn(void)
{
}

//-----------------------------------------------------------------------------
// Name : validateNewPos
//-----------------------------------------------------------------------------
bool Pawn::validateNewPos(int dx,int dy, BOARD_POINT startSqaure, BOARD_POINT targetSquare,piece * SBoard[boardY][boardX])
{
    piece * targetPiece = SBoard[targetSquare.col][targetSquare.row];
    // 1(white) moving up, -1(black) moving down
    int direction = this->getColor();

    // check if movement direction is legal and that the targetPiece is Null
    if ( dy == direction && dx == 0 && targetPiece == nullptr) 
        return true;

    // check if the eating movement(diagonally) is legal and that the direction is legal 
    if ( std::abs(dx) == 1 && dy == direction && targetPiece != nullptr )
        if(targetPiece->getColor() != this->getColor())
            return true;

        // check if the movement of 2 squares is legal for this pawn
    if (dy == 2*direction && dx == 0 && m_onStartPoint && targetPiece == nullptr) 
    {
        if (m_onStartPoint)
        {
            int col= targetSquare.col + direction;
            int row =targetSquare.row;
            // check if there is a piece in the middle of the pawn way
            piece * middlePiece=SBoard[col][row]; 
            if (middlePiece == nullptr)
                return true;
        }
    }

    // move was not a valid one
    return false;
}

int Pawn::getType()
{
    return Pawn::type;
}
