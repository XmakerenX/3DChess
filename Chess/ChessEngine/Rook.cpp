//
// 3D Chess - A cross platform Chess game for 2 players made using OpenGL and c++
// Copyright (C) 2016-2020 Matan Keren <xmakerenx@gmail.com>
//
// This file is part of 3D Chess.
//
// 3D Chess is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// 3D Chess is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with 3D Chess.  If not, see <http://www.gnu.org/licenses/>.
//

#include "Rook.h"
#include <cmath>

PIECES rook::type = ROOK;
//-----------------------------------------------------------------------------
// Name : Rook (constructor)
//-----------------------------------------------------------------------------
rook::rook(int playerColor, BOARD_POINT _boardPoistion)
    :piece(playerColor, _boardPoistion)
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
{}

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
            dirVec.x = dx / std::abs(dx);
        else
            dirVec.x = 0;

        if (dy != 0)
            dirVec.y = dy / std::abs(dy);
        else
            dirVec.y = 0;

        if (targetPiece != nullptr)//check if there is a piece on the target square
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
