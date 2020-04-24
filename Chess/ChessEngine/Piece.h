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

#ifndef _PIECE_H_
#define _PIECE_H_

#include "Chess.h"

// the base class to all the chess pieces
class piece
{
public:

    //-------------------------------------------------------------------------
    // Constructors & Destructors for This Class.
    //-------------------------------------------------------------------------
    piece(int playerColor, BOARD_POINT _boardPoistion);
    virtual ~piece(void);

    //-------------------------------------------------------------------------
    // Pure Virtual functions of this class
    //-------------------------------------------------------------------------
    virtual bool  validateNewPos(int dx,int dy, BOARD_POINT startSqaure, BOARD_POINT targetSquare,piece * SBoard[boardY][boardX])=0; 
    // returns the piece type PAWN,KNIGHT,BISHOP,ROOK,QUEEN,KING
    virtual int   getType() = 0; 

    //-------------------------------------------------------------------------
    // misc Functions
    //-------------------------------------------------------------------------
    bool isPieceInWay(BOARD_POINT startSquare, BOARD_POINT targetSquare, DIR_VEC dirVec, piece * sBoard[boardY][boardX]);
    bool isMoved();

    //-------------------------------------------------------------------------
    // Set methods for this class.
    //-------------------------------------------------------------------------
    void setMoved(bool newStatus);
    void setBoardPosition(BOARD_POINT newPosition);

    //-------------------------------------------------------------------------
    // Get methods for this class
    //-------------------------------------------------------------------------
    int getColor();
    BOARD_POINT getPosition();
    void getDx(int inputArray[]);
    void getDy(int inputArray[]);

protected:
    BOARD_POINT boardPoistion;
    int color;
    bool m_onStartPoint;
    int dx[2];
    int dy[2];
};

#endif // _PIECE_H_
 
