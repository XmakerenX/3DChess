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

#include "Bishop.h"
#include <cmath>

//initialization of static vars
PIECES bishop::type = BISHOP;

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
{}

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
