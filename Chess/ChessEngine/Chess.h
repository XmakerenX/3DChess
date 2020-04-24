/* * 3D Chess - A cross platform Chess game for 2 players made using OpenGL and c++
 * Copyright (C) 2016-2020 Matan Keren <xmakerenx@gmail.com>
 *
 * This file is part of 3D Chess.
 *
 * 3D Chess is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * 3D Chess is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with 3D Chess.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef _CHESS_H_ 
#define _CHESS_H_

const int boardX = 8;
const int boardY = 8;

enum PIECES{PAWN,KNIGHT,BISHOP,ROOK,QUEEN,KING};

struct BOARD_POINT  
{
    BOARD_POINT(unsigned int _row, unsigned int _col)
    {
        row = _row;
        col = _col;
    }
    
    BOARD_POINT(const BOARD_POINT& toCopy)
    {
        row = toCopy.row;
        col = toCopy.col;
    }
    
    int row;   //j
    int col;   //i
};

struct DIR_VEC
{
    DIR_VEC()
    {
        ;
    }

    DIR_VEC(int newX, int newY)
    {
        x = newX;
        y = newY;
    }

    int x;
    int y;
};

#endif // _PIECE_H_ 
