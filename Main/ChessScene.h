#ifndef _CHESSSCENE_H_
#define _CHESSSCENE_H_

#include "../Scene.h"

class ChessScene : public Scene
{
public:
    ChessScene();
    
    virtual void InitObjects();
    
    virtual bool handleMouseEvent(MouseEvent event, const ModifierKeysStates &modifierStates);
    virtual Object *PickObject(Point& cursor, int& faceCount, int &meshIndex);
    virtual void Drawing();
    
    Point getPickedSquare(int facePicked, int meshPickedIndex);
    static Point pointToBoardPoint(Point pt);
    static Point boardPointToPoint(Point pt);
    
private:
    static const int nCellHigh = 8;
    static const int nCellWide = 8;

};

#endif // _CHESSSCENE_H_ 
