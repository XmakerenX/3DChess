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

#include "Knight.h"
#include <cmath>

PIECES knight::type = KNIGHT;
//-----------------------------------------------------------------------------
// Name : knight (constructor)
//-----------------------------------------------------------------------------
knight::knight(int playerColor, BOARD_POINT _boardPoistion)
    :piece(playerColor, _boardPoistion)
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
{}

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
