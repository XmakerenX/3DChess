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

#ifndef _CHESSSCENE_H_
#define _CHESSSCENE_H_

#include <Render/Scene.h>
#include "ChessEngine/Board.h"
#include <Render/GUI/DialogUI.h>
#include "pawnsDef.h"

class ChessScene : public Scene
{
public:
    enum class RotationMode{Infinite, ReturnToWhite, ReturnToBlack, RotateCW, RotateCCW, Stoped};
    
    ChessScene(DialogUI& promotionDialog, DialogUI& gameOverDialog);
    ~ChessScene();
    
    virtual void InitScene(int width, int height, const glm::vec3& cameraPos, const glm::vec3& cameraLookat);
    virtual void InitObjects();
    
    void newGame();
    void loadGame();
    
    virtual bool handleMouseEvent(MouseEvent event, const ModifierKeysStates &modifierStates);
    virtual Object *PickObject(Point& cursor, int& faceCount, int &meshIndex);
    virtual void Drawing(double frameTimeDelta);
    
    void setCameraRotationMode(RotationMode newMode);
    void setCameraRotaion(bool enabled);
    
    void onChessPieceCreated(piece* pPiece);
    void onChessPieceMoved(piece* pPiece, BOARD_POINT pieceOldBoardPoint, BOARD_POINT pieceNewBoardPoint);
    void onChessPieceKilled(piece* pPiece);
    void onPromotionSelected(ButtonUI* selectedPieceButton);
    void ShowGameOver(std::string gameOverStatus);
    
    void highLightSquares();
    void hightlightBoardSquare(Point squareToHightlight, GLuint attributeID);
    
    Point getPickedSquare(int facePicked, int meshPickedIndex);
    void deletePieceObject(Point pieceBoardPoint);
    
    static BOARD_POINT pointToBoardPoint(Point pt);
    static Point boardPointToPoint(BOARD_POINT pt);
    
private:
    void RotateCamera(float frameTimeDelta);
    bool m_cameraRotation;
    RotationMode m_rotationMode;
    float m_rotationAngle;
    float m_endAngle;
    
    board* gameBoard;
    Object* boardObject;
    Object* frameSquareObject;
    
    GLuint m_blackAttribute;
    GLuint m_blueAttribute;
    GLuint m_redAttribute;
    GLuint m_redEyesAttribute;
    GLuint m_yellowAttribute;
    
    int m_lastIndex;
    int pieceObjects[8][8];
    
    DialogUI& m_promotionDialog;
    DialogUI& m_gameOverDialog;
    
    static const int nCellHigh = 8;
    static const int nCellWide = 8;

};

#endif // _CHESSSCENE_H_ 
