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
 
