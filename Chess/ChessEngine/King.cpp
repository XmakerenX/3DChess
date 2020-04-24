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

#include "King.h"
#include <cmath>

PIECES king::type = KING;
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
{}

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
