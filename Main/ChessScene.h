#ifndef _CHESSSCENE_H_
#define _CHESSSCENE_H_

#include "../Scene.h"
#include "../ChessEngine/Board.h"
#include "../GUI/DialogUI.h"
#include "pawnsDef.h"

class ChessScene : public Scene
{
public:
    ChessScene(DialogUI& promotionDialog);
    ~ChessScene();
    
    virtual void InitObjects();
    
    void newGame();
    void loadGame();
    
    virtual bool handleMouseEvent(MouseEvent event, const ModifierKeysStates &modifierStates);
    virtual Object *PickObject(Point& cursor, int& faceCount, int &meshIndex);
    virtual void Drawing();
    
    void onChessPieceCreated(piece* pPiece);
    void onChessPieceMoved(piece* pPiece, BOARD_POINT pieceOldBoardPoint, BOARD_POINT pieceNewBoardPoint);
    void onChessPieceKilled(piece* pPiece);
    void onPromotionSelected(ButtonUI* selectedPieceButton);
    
    void highLightSquares();
    void hightlightBoardSquare(Point squareToHightlight, GLuint attributeID);
    
    Point getPickedSquare(int facePicked, int meshPickedIndex);
    void deletePieceObject(Point pieceBoardPoint);
    
    static BOARD_POINT pointToBoardPoint(Point pt);
    static Point boardPointToPoint(BOARD_POINT pt);
    
private:
    board* gameBoard;
    Object* boardObject;
    Object* frameSquareObject;
    
    GLuint m_blackAttribute;
    GLuint m_blueAttribute;
    GLuint m_redAttribute;
    GLuint m_yellowAttribute;
    
    int m_lastIndex;
    int pieceObjects[8][8];
    
    DialogUI& m_promotionDialog;
    
    static const int nCellHigh = 8;
    static const int nCellWide = 8;

};

#endif // _CHESSSCENE_H_ 
