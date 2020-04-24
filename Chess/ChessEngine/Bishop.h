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

#ifndef _BISHOP_H_
#define _BISHOP_H_

#include "Piece.h"

class bishop : public piece
{
public:
    bishop(int playerColor, BOARD_POINT _boardPoistion);
    ~bishop(void);
    virtual bool validateNewPos(int dx,int dy, BOARD_POINT startSqaure, BOARD_POINT targetSquare,piece * SBoard[boardY][boardX]);
    int   getType();

private:
    static PIECES type;
};

#endif // _BISHOP_H_
 
