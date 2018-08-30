#include "Board.h"

//-----------------------------------------------------------------------------
// Name : board (Constructor)
//-----------------------------------------------------------------------------
board::board()
: m_startSquare (-1, -1), m_targetSqaure (0,0), m_attLoc (0,0)
{
    for (unsigned int i = 0; i < boardX; i++)
        for (unsigned int j = 0; j < boardY; j++)
                m_board[i][j] = nullptr;

    m_currentPawn = nullptr;
    m_prevPawn = nullptr;

    m_kings[BOTTOM] = nullptr;
    m_kings[UPPER] = nullptr;
}

//-----------------------------------------------------------------------------
// Name : board (Destructor)
//-----------------------------------------------------------------------------
board::~board(void)
{
}

//-----------------------------------------------------------------------------
// Name : init ()
//-----------------------------------------------------------------------------
void board::init()
{
    m_currentPlayer = 1;
    m_pieceCount = 0;

    // add black pieces to board
    for (int i = 0; i < 2; i++)
    {
        for (int j = 0; j < boardX; j++)
        {
            createStartingPiece(i,j, BLACK);
        }
    }

    // add white pieces to board
    for (int i = boardY - 2; i < boardY; i++)
    {
        for (int j = 0; j < boardX; j++)
        {
            createStartingPiece(i,j, WHITE);
        }
    }

    m_gameActive = true;
    m_unitPromotion = false;
    m_kingInThreat = false;
}

//-----------------------------------------------------------------------------
// Name : SaveBoardToFile ()
//-----------------------------------------------------------------------------
void board::SaveBoardToFile()
{
    std::ofstream saveFile;

    saveFile.open("board.sav");

    saveFile << m_kingInThreat << "| king in Threat" << "\n";
    saveFile << m_pieceCount << "| piece Count" << "\n";
    saveFile << m_currentPlayer << "| current player" << "\n";

    for (unsigned int i = 0; i < boardY; i++)
    {
        for (unsigned int j = 0; j < boardX; j++)
        { 
            if ( m_board[i][j] != nullptr)
            {
                saveFile << i << "| piece i" << "\n";
                saveFile << j << "| piece j" << "\n";
                saveFile << m_board[i][j]->getColor() << "| piece Color" << "\n";
                saveFile << m_board[i][j]->getType() << "| piece Type" << "\n";
            }
        }
    }

    saveFile.close();
}

//-----------------------------------------------------------------------------
// Name : LoadBoardFromFile ()
//-----------------------------------------------------------------------------
bool board::LoadBoardFromFile()
{
    std::ifstream inputFile;
        
    // open the save file and verify success
    inputFile.open("board.sav");
    if (!inputFile.good())
        return false;
    
    for (unsigned int i = 0; i < boardX; i++)
        for (unsigned int j = 0; j < boardY; j++)
                m_board[i][j] = nullptr;

    m_targetSqaure.col = 0;
    m_targetSqaure.row = 0;

    m_currentPawn = nullptr;
    m_prevPawn    = nullptr;

    m_kings[UPPER]  = nullptr;
    m_kings[BOTTOM] = nullptr;
    
    m_deadPawnsVec[UPPER].clear();
    m_deadPawnsVec[BOTTOM].clear();


    //clearing the pieces
    for (unsigned int j =0; j < 2; j++)
    {
        for (piece* currentPiece : m_pawnsVec[j])
        {
            delete currentPiece;
            currentPiece = nullptr;
        }
        m_pawnsVec[j].clear();
    }
    
    inputFile >> m_kingInThreat;
    inputFile.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    inputFile >> m_pieceCount;
    inputFile.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    inputFile >> m_currentPlayer;
    inputFile.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

    do 
    {
        unsigned int i,j;
        int color,type;

        inputFile >> i;
        inputFile.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        if (inputFile.eof() )
            break;

        inputFile >> j;
        inputFile.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        inputFile >> color;
        inputFile.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        inputFile >> type;
        inputFile.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

        m_board[i][j] = createPiece(color, static_cast<PIECES>(type), BOARD_POINT(j,i));
    } while (!inputFile.eof());

    inputFile.close();
    m_gameActive = true;
    return true;
}

//-----------------------------------------------------------------------------
// Name : validateMove ()
//-----------------------------------------------------------------------------
bool board::validateMove(BOARD_POINT startLoc,BOARD_POINT newLoc)
{
    if ( m_currentPawn == nullptr)
    {
        std::cout << "Error : currentPawn was null.. aborting\n";
        return false;
    }
    
    int dx = startLoc.row - newLoc.row;
    int dy = startLoc.col - newLoc.col;

    if ( m_currentPawn->validateNewPos(dx,dy,startLoc,newLoc,m_board ))
    {
        //TODO: make castling thingy here 
        piece * targetPiece = m_board[newLoc.col][newLoc.row];

        if (targetPiece != nullptr)
        {
            m_prevPawn = targetPiece;
            // soft kill this piece by setting out of the bounds of the board
            m_prevPawn->setBoardPosition(BOARD_POINT(10,10));
        }
        else
        {
            //king was moved to castling pos
            if ( m_currentPawn->getType() == KING && (dx == -2 || dx == 3))
            {
                if (!m_kingInThreat)
                {
                    if (castling(dx, dy))//check if castling is valid 
                    {
                        int x = dx/abs(dx);
                        int y = m_startSquare.col;

                        x = m_startSquare.row - dx -x;

                        BOARD_POINT newRookSquare(x + 2 * (dx / abs(dx)), y);

                        piece * rookPiece = m_board[y][x]; 
                        m_board[newRookSquare.col][newRookSquare.row] = rookPiece;
                        m_pieceMovedSig(rookPiece, rookPiece->getPosition() ,newRookSquare);
                        rookPiece->setBoardPosition(newRookSquare);
                        m_board[y][x] = nullptr;
                    }
                    else
                        return false;
                }
                else
                    return false;
            }
        }

        m_board[newLoc.col][newLoc.row] = m_currentPawn;
        m_currentPawn->setBoardPosition(newLoc);
        m_board[startLoc.col][startLoc.row]  = nullptr;
        
        // if king is under threat the move is illegal
        if(isKingInThreat( m_currentPlayer,false))
        {
            reverseMove();  // reverse the changes done 
            m_prevPawn = nullptr;
            return false;
        }
        else
        {
            //TODO: add here some way to display something that will let the player choose the promotion unit
            if ( m_prevPawn != nullptr)
                killPiece( m_prevPawn,newLoc);

            m_currentPawn->setMoved(true);
            m_prevPawn = nullptr;
            return true;
        }
    }
    
    return false;
}

//-----------------------------------------------------------------------------
// Name : processPress ()
//-----------------------------------------------------------------------------
void board::processPress(BOARD_POINT pressedSqaure)
{
    // we only handle presses if game is still active and there is no units waiting for promotion
    if (m_gameActive && !m_unitPromotion) 
    {        
        if (pressedSqaure.row == m_startSquare.row && pressedSqaure.col == m_startSquare.col)//the square was already selected
            return;

        std::stringstream out,out2,out3,out4;

        out3 << m_startSquare.col;
        out4 << m_startSquare.row;
        out << pressedSqaure.col;
        out2 << pressedSqaure.row;

        m_curStatus = "the pressed square is " + out.str() + ","+out2.str();

        // check if the object is in the board bounds if not we have nothing to do with this object
        if (pressedSqaure.row < boardX && pressedSqaure.col < boardY && pressedSqaure.row >= 0 && pressedSqaure.col >= 0 &&
                m_startSquare.row < boardX && m_startSquare.col < boardY && m_startSquare.row >= 0 && m_startSquare.col >= 0) 
        {
            m_targetSqaure = pressedSqaure;
            // process press and check if the move is valid ?
            if (!validateMove( m_startSquare,pressedSqaure)) 
            {
                //not valid therefore sets startSquare to invalid value to reset piece movement process
                m_startSquare.row = -1;
                m_startSquare.col = -1;

//                 m_attribSquares[ATTACK].clear();
//                 m_attribSquares[MOVE].clear();
//                 m_selectSquare = -1;

                m_curStatus += "\n move was invalid start:" + out3.str() + ","+out4.str() + " target:"+out.str() + ","+out2.str();
            }
            else
            {
//                 m_attribSquares[ATTACK].clear();
//                 m_attribSquares[MOVE].clear();
//                 m_selectSquare = -1;

                if ( m_currentPawn->getType() == PAWN)
                {
                    if ( m_targetSqaure.col == 0 || m_targetSqaure.col == 7)
                        m_unitPromotion = true;
                }
                    
                m_pieceMovedSig( m_currentPawn, m_startSquare, pressedSqaure);
                    
                //the move was valid now ends current player turn 
                if (!m_unitPromotion)//checks if pawn need to be promoted if yes wait till the main program tell us user selection and then proceed to endTurn
                    endTurn();

                m_curStatus +="\n move was valid";
            }
        }            
        // no valid press exist so set the current pawn and startSquare point 
        else 
        {
            m_currentPawn = m_board[pressedSqaure.col][pressedSqaure.row];

            if ( m_currentPawn == nullptr)
                return;

            if ( m_currentPawn->getColor() == m_currentPlayer ) 
            {
                m_startSquare = pressedSqaure;
                m_curStatus += "\n press is Valid";

//                 m_attribSquares[ATTACK].clear();
//                 m_attribSquares[MOVE].clear();
//                 m_selectSquare = startSquare.row + (7 - startSquare.col) * m_numCellsWide;
                markPawnMoves( m_startSquare, m_currentPawn->getColor(), m_currentPlayer );
            }
        }
    }
    
}

//-----------------------------------------------------------------------------
// Name : isKingInThreat ()
//-----------------------------------------------------------------------------
bool board::isKingInThreat(int player,bool getAllAttackers)
{
    int pieceSide,kingSide,maxThreat;
    m_kingInThreat = false;

    if (player == 1)
        kingSide = BOTTOM;
    else
        kingSide = UPPER;

    if (player == 1)
        pieceSide = UPPER;
    else
        pieceSide = BOTTOM;

    if (getAllAttackers)
        maxThreat = 2;
    else
        maxThreat = 1;

    unsigned int i = 0;
    int threat = 0;

    std::vector<piece*> pawnTemp=m_pawnsVec[pieceSide];
    while (i < m_pawnsVec[pieceSide].size() && threat < maxThreat)
    {
        piece * pCurPiece = pawnTemp[i];

        BOARD_POINT curPieceSquare = pCurPiece->getPosition();
        BOARD_POINT curKingSquare  = m_kings[kingSide]->getPosition();
        
        if ( (curPieceSquare.row  > 8 || curPieceSquare.row < 0) || ( curPieceSquare.col > 8 || curPieceSquare.col < 0))
        {
            i++;
            //piece is on invalid location so ignore it ... **piece will be on an invalid location only if it is about to be killed
            continue;
        }

        int dx = curPieceSquare.row - curKingSquare.row;
        int dy = curPieceSquare.col - curKingSquare.col;

        if(pCurPiece->validateNewPos(dx,dy,curPieceSquare,curKingSquare,m_board ))
        {
            threat++;
            if (getAllAttackers)
            {
                m_attLoc.row = dx;
                m_attLoc.col = dy;
            }
        }
        i++;
    }
    
    if (getAllAttackers)
        m_nAttackers=threat;

    if (threat > 0)
    {
        m_kingInThreat = true;
        BOARD_POINT curKingSquare  = m_kings[kingSide]->getPosition();
        //BOARD_POINT curKingSquare  = getPieceSquare(kings[kingSide]);
        return true;
    }
    else
    {
        return false;
    }
    
    return false;
}

//-----------------------------------------------------------------------------
// Name : reverseMove ()
//-----------------------------------------------------------------------------
void board::reverseMove()
{
    m_board[m_targetSqaure.col][m_targetSqaure.row] = m_prevPawn;
    if ( m_prevPawn )
        m_prevPawn->setBoardPosition( m_targetSqaure );
    
    m_board[m_startSquare.col][m_startSquare.row]   = m_currentPawn;
    m_currentPawn->setBoardPosition( m_startSquare );
}

//-----------------------------------------------------------------------------
// Name : isEndGame ()
//-----------------------------------------------------------------------------
bool board::isEndGame(int curretPlayer)
{
    int side;

    if (curretPlayer == WHITE)
        side = BOTTOM;
    else
        side = UPPER;

    BOARD_POINT kingSquare = m_kings[side]->getPosition();
    //BOARD_POINT kingSquare = getPieceSquare(kings[side]);

    if ( m_nAttackers == 1)
    {
        //----------------------------------------------------------------
        //check if we can eat the piece that is attacking               
        //----------------------------------------------------------------
        // getting the position of the pawn that is attacking the king
        m_targetSqaure.row = kingSquare.row + m_attLoc.row;
        m_targetSqaure.col = kingSquare.col + m_attLoc.col;

        //getting pawns under the king color
        std::vector<piece*> pawnTemp = m_pawnsVec[side];     
        unsigned int i = 0;

        //scanning to check if a pawn can eat the attacking pawn
        while (i<m_pawnsVec[side].size())
        {
            //getting the pawn that we checking for right now
            piece* curPawn=pawnTemp[i];
            //BOARD_POINT curPieceSquare,curAttPieceSquare;

            BOARD_POINT curPieceSquare = curPawn->getPosition();
            // getting the square it is current at
            m_startSquare = curPieceSquare;

            //getting the location of the attacker on the board
            BOARD_POINT curAttPieceSquare(kingSquare.row + m_attLoc.row, kingSquare.col + m_attLoc.col);

            //calculating dx dy of curPawn from 
            int dx = curPieceSquare.row - curAttPieceSquare.row;
            //Attacker
            int dy = curPieceSquare.col - curAttPieceSquare.col;

            if(curPawn->validateNewPos(dx,dy,m_startSquare,m_targetSqaure,m_board ))
            {
                // if the move is valid checking to see that the king is not threated from
                // such move if the king is threated than keep scanning 
                // also after the move was done always reverse it in the end
                if(!validateKingThreat(curretPlayer))
                    return false;
            }
            i++;
        }
        //----------------------------------------------------------------
        //END of check if we can eat the attacker                       
        //----------------------------------------------------------------


        //----------------------------------------------------------------
        //checking if a pawn can block the attacker path                
        //----------------------------------------------------------------
        m_targetSqaure.row = kingSquare.row + m_attLoc.row;
        m_targetSqaure.col = kingSquare.col + m_attLoc.col;

        int AtkPawnType = m_board[m_targetSqaure.col][m_targetSqaure.row]->getType();
        int dxxx,dyyy;

        if ( m_attLoc.row != 0)
            dxxx = m_attLoc.row / abs( m_attLoc.row);
        else
            dxxx = 0;

        if ( m_attLoc.col != 0)
            dyyy = m_attLoc.col / abs( m_attLoc.col);
        else
            dyyy=0;

        m_targetSqaure.row = kingSquare.row + dxxx;
        m_targetSqaure.col = kingSquare.col + dyyy;

        // knight king and pawn cannot be blocked
        if(AtkPawnType != KNIGHT && AtkPawnType != PAWN && AtkPawnType != KING) 
        {
            i = 0;
            //scanning to check if a pawn can eat the attacking pawn
            while (i < m_pawnsVec[side].size())
            {
                int j = kingSquare.row + dxxx;
                int k = kingSquare.col + dyyy;

                while (j != (kingSquare.row + m_attLoc.row) || k != (kingSquare.col + m_attLoc.col))
                {
                    //getting the location of the attacker on the board
                    m_targetSqaure.row = j;
                    m_targetSqaure.col = k;

                    piece* curPawn=pawnTemp[i];//getting the pawn that we checking for right now

                    //startSquare = getPieceSquare(curPawn); // getting the square he is current at
                    m_startSquare = curPawn->getPosition(); // getting the square he is current at

                    int dx = m_startSquare.row - m_targetSqaure.row;//calculating dx dy of curPawn from 
                    int dy = m_startSquare.col - m_targetSqaure.col;//Attacker

                    if(curPawn->validateNewPos(dx,dy,m_startSquare,m_targetSqaure,m_board ))
                    {
                        // if the move is valid checking to see that the king is not threated from
                        // such move if the king is threated than keep scanning 
                        // also after the move was done always reverse it in the end
                        if(!validateKingThreat(curretPlayer))
                            return false;
                    }
                    
                    j+=dxxx;
                    k+=dyyy;
                }
                i++;
            }
        }
        //----------------------------------------------------------------
        //END of checking if a pawn can block the attacker path         
        //----------------------------------------------------------------
    }

    //----------------------------------------------------------------
    //check for if the king can flee to other square from the attack
    //----------------------------------------------------------------
    m_startSquare = kingSquare;//getting king square

    //scanning all possible squares that the king can move to 
    for (int dy = -1; dy < 2; dy++)
    {
        for (int dx =- 1; dx < 2; dx++)
        {
            if (dx !=0 || dy != 0)
            {
                BOARD_POINT kingFleeLoc(kingSquare.row - dx, kingSquare.col - dy); 

                if ( (kingFleeLoc.row < 8 && kingFleeLoc.row >=0) && (kingFleeLoc.col < 8 && kingFleeLoc.col >=0 ) )//checking that we are still in board range
                {
                    m_targetSqaure = kingFleeLoc;//getting the flee square

                    if( m_kings[side]->validateNewPos(dx,dy,m_startSquare,m_targetSqaure,m_board ))
                    {
                        // checking that the king can do the move without being threaten
                        if (!validateKingThreat(curretPlayer))
                            return false; 
                    }
                }
            }
        }
    }
    //------------------------------------------------------------------------
    //END of check for if the king can flee to other square from the attack 
    //------------------------------------------------------------------------

    return true;
}

//-----------------------------------------------------------------------------
// Name : validateKingThreat ()
//-----------------------------------------------------------------------------
bool board::validateKingThreat(int curretPlayer)
{
    m_prevPawn    = m_board[m_targetSqaure.col][m_targetSqaure.row];
    m_currentPawn = m_board[m_startSquare.col][m_startSquare.row];

    m_board[m_targetSqaure.col][m_targetSqaure.row] = m_currentPawn;
    //m_pieceMovedSig(currentPawn, targetSqaure);
    m_currentPawn->setBoardPosition( m_targetSqaure );
    m_board[m_startSquare.col][m_startSquare.row]   = nullptr;
    
    if (!isKingInThreat( m_currentPlayer,false))
    {
        reverseMove();
        m_prevPawn = nullptr;
        return false;
    }
    else
    {
        reverseMove();
        m_prevPawn = nullptr;
        return true;
    }

    m_prevPawn = nullptr;
    return true;
}

//-----------------------------------------------------------------------------
// Name : canPawnMove ()
//-----------------------------------------------------------------------------
bool board::canPawnMove(BOARD_POINT pieceSqaure,int color,int curretPlayer)
{
    int dxValues[2];
    int dyValues[2];

    //getting the current Pawn we are checking for square
    m_startSquare = pieceSqaure;
    m_currentPawn = m_board[m_startSquare.col][m_startSquare.row];

    //getting the values of dx should run between in order to cover all possible moves
    m_currentPawn->getDx(dxValues);
    //getting the values of dy should run between in order to cover all possible moves
    m_currentPawn->getDy(dyValues);

    for (int dy = dyValues[0]; dy < dyValues[1]; dy++)//scanning all possible squares that the pawn can move to 
    {
        for (int dx = dxValues[0]; dx < dxValues[1]; dx++)
        {
            if (dx != 0 || dy != 0)
            {
                BOARD_POINT possibleMoveLoc( m_startSquare.row - dx, m_startSquare.col - dy); 

                if ( (possibleMoveLoc.row < boardX && possibleMoveLoc.row >=0) && (possibleMoveLoc.col < boardY && possibleMoveLoc.col >=0 ) )//checking that we are still in board range
                {
                    m_targetSqaure = possibleMoveLoc;//getting the  current possible square
                    if( m_currentPawn->validateNewPos(dx,dy,m_startSquare,m_targetSqaure,m_board ))
                    {
                        // checking that the king can do the move without being threaten
                        if (!validateKingThreat(curretPlayer))
                            return true;
                    }
                }
            }
        }
    }
    
    return false;
}

//-----------------------------------------------------------------------------
// Name : markPawnMoves ()
//-----------------------------------------------------------------------------
void board::markPawnMoves(BOARD_POINT pieceSqaure,int color,int curretPlayer)
{
//     int dxValues[2];
//     int dyValues[2];
// 
//     //getting the current Pawn we are checking for  square
//     m_startSquare = pieceSqaure;
//     m_currentPawn = m_board[m_startSquare.col][m_startSquare.row];
// 
//     //getting the values of dx/dy should run between in order to cover all possible moves
//     m_currentPawn->getDx(dxValues);
//     m_currentPawn->getDy(dyValues);
// 
//     switch ( m_currentPawn->getType())
//     {
//     case PAWN:
//     case KING:
//     case KNIGHT:
//     {
//         //scanning all possible squares that the pawn can move to 
//         for (int dy = dyValues[0]; dy < dyValues[1]; dy++)
//         {
//             for (int dx = dxValues[0]; dx < dxValues[1]; dx++)
//             {
//                 if (dx != 0 || dy != 0)
//                 {
//                     BOARD_POINT possibleMoveLoc; 
//                     possibleMoveLoc.row = startSquare.row - dx;//getting current possible square x,y that the pawn can move to
//                     possibleMoveLoc.col = startSquare.col - dy;
//                     //checking that we are still in board range
//                     if ( (possibleMoveLoc.row < 8 && possibleMoveLoc.row >=0) &&
//                         (possibleMoveLoc.col < 8 && possibleMoveLoc.col >=0 ) )
//                     {
//                         // getting the  current possible square
//                         targetSqaure = possibleMoveLoc;
//                         if( m_currentPawn->validateNewPos(dx,dy,m_startSquare,m_targetSqaure,m_board ))
//                         {
//                             //checking that the king
//                             if (!validateKingThreat(curretPlayer))
//                             {
//                                 if ( m_board[m_targetSqaure.col][m_targetSqaure.row] != nullptr)
//                                 {
//                                     if (SBoard[targetSqaure.col][targetSqaure.row]->getColor() != currentPlayer)
//                                         m_attribSquares[ATTACK].push_back(targetSqaure.row + ( (7 - targetSqaure.col ) * m_numCellsWide));
//                                     else
//                                         m_attribSquares[MOVE].push_back(targetSqaure.row + ( (7 - targetSqaure.col) * m_numCellsWide));
//                                 }
//                                 else
//                                     m_attribSquares[MOVE].push_back(targetSqaure.row + ( ( 7 - targetSqaure.col) * m_numCellsWide));
//                             }
//                         }
//                     }
//                 }
//             }
//         }
// 
//         //king was moved to castling pos
//         if ( m_currentPawn->getType() == KING)
//             //make sure the king is not in threat
//             if (!m_kingInThreat)
//             {
//                 BOARD_POINT possibleMoveLoc; 
//                 possibleMoveLoc.row = startSquare.row - (-2);//getting current possible square x,y that the pawn can move to
//                 possibleMoveLoc.col = startSquare.col - 0;
// 
//                 targetSqaure = possibleMoveLoc;
//                 if (currentPawn->validateNewPos(-2, 0, startSquare, targetSqaure, SBoard) )
//                     if (castling(-2, 0)) //check if castling is valid
//                         m_attribSquares[MOVE].push_back(targetSqaure.row + ( ( 7 - targetSqaure.col) * m_numCellsWide) );
// 
//                 possibleMoveLoc.row = startSquare.row - 3;//getting current possible square x,y that the pawn can move to
//                 possibleMoveLoc.col = startSquare.col - 0;
//                 targetSqaure = possibleMoveLoc;
// 
//                 if (currentPawn->validateNewPos(3, 0, startSquare, targetSqaure, SBoard) )
//                     if (castling(3, 0)) //check if castling is valid
//                         m_attribSquares[MOVE].push_back(targetSqaure.row + ( ( 7 - targetSqaure.col) * m_numCellsWide) );
//             }
// 
//     }break;
// 
//     case BISHOP:
//     {
//         ScanPawnMoves(pieceSqaure,color,curretPlayer, DIR_VEC(1,1));
//         ScanPawnMoves(pieceSqaure,color,curretPlayer, DIR_VEC(-1,-1));
//         ScanPawnMoves(pieceSqaure,color,curretPlayer, DIR_VEC(1,-1));
//         ScanPawnMoves(pieceSqaure,color,curretPlayer, DIR_VEC(-1,1));
//     }break;
// 
//     case ROOK:
//     {
//         ScanPawnMoves(pieceSqaure,color,curretPlayer, DIR_VEC(1,0));
//         ScanPawnMoves(pieceSqaure,color,curretPlayer, DIR_VEC(0,1));
//         ScanPawnMoves(pieceSqaure,color,curretPlayer, DIR_VEC(-1,0));
//         ScanPawnMoves(pieceSqaure,color,curretPlayer, DIR_VEC(0,-1));
//     }break;
// 
//     case QUEEN:
//     {
//         ScanPawnMoves(pieceSqaure,color,curretPlayer, DIR_VEC(1,1));
//         ScanPawnMoves(pieceSqaure,color,curretPlayer, DIR_VEC(-1,-1));
//         ScanPawnMoves(pieceSqaure,color,curretPlayer, DIR_VEC(1,-1));
//         ScanPawnMoves(pieceSqaure,color,curretPlayer, DIR_VEC(-1,1));
// 
//         ScanPawnMoves(pieceSqaure,color,curretPlayer, DIR_VEC(1,0));
//         ScanPawnMoves(pieceSqaure,color,curretPlayer, DIR_VEC(0,1));
//         ScanPawnMoves(pieceSqaure,color,curretPlayer, DIR_VEC(-1,0));
//         ScanPawnMoves(pieceSqaure,color,curretPlayer, DIR_VEC(0,-1));
//     }break;
// 
//     }
}

//-----------------------------------------------------------------------------
// Name : ScanPawnMoves ()
//-----------------------------------------------------------------------------
void board::ScanPawnMoves(BOARD_POINT pieceSqaure,int color,int curretPlayer,DIR_VEC dir)
{
//     BOARD_POINT possibleMoveLoc;
//     // getting the current Pawn we are checking for  square
//     possibleMoveLoc = pieceSqaure;
//     m_startSquare = pieceSqaure;
// 
// 
//     possibleMoveLoc.row = possibleMoveLoc.row + dir.x;
//     possibleMoveLoc.col = possibleMoveLoc.col + dir.y;
// 
//     while( (possibleMoveLoc.row < 8 && possibleMoveLoc.row >=0) && (possibleMoveLoc.col < 8 && possibleMoveLoc.col >=0 ) )
//     {
//         targetSqaure = possibleMoveLoc;//getting the  current possible square
// 
//         int dx = startSquare.row - targetSqaure.row;
//         int dy = startSquare.col - targetSqaure.col;
// 
//         if(currentPawn->validateNewPos(dx,dy,startSquare,targetSqaure,SBoard))
//         {
//             if (!validateKingThreat(curretPlayer))//checking that the king
//             {
//                 if (SBoard[targetSqaure.col][targetSqaure.row] != nullptr)
//                 {
//                     if (SBoard[targetSqaure.col][targetSqaure.row]->getColor() != currentPlayer)
//                     {
//                         m_attribSquares[ATTACK].push_back(targetSqaure.row + ( (7 - targetSqaure.col ) * m_numCellsWide));
//                         break;
//                     }
//                 }
//                 else
//                     m_attribSquares[MOVE].push_back(targetSqaure.row + ( ( 7 - targetSqaure.col) * m_numCellsWide));
//             }
//         }
// 
//         possibleMoveLoc.row = possibleMoveLoc.row + dir.x;
//         possibleMoveLoc.col = possibleMoveLoc.col + dir.y;
//     }
}

//-----------------------------------------------------------------------------
// Name : isDraw ()
//-----------------------------------------------------------------------------
bool board::isDraw(int currentPlayer)
{
    int side;
    if (currentPlayer == 1)
        side = BOTTOM;
    else
        side = UPPER;

    unsigned int i = 0;
    std::vector<piece*> pawnTemp = m_pawnsVec[side];

    while (i < m_pawnsVec[side].size())
    {
        //BOARD_POINT curPieceSquare = getPieceSquare(pawnTemp[i]);
        BOARD_POINT curPieceSquare = pawnTemp[i]->getPosition();
        if(canPawnMove(curPieceSquare,side,currentPlayer))
            return false;
        i++;
    }
    
    return true;
}

//-----------------------------------------------------------------------------
// Name : createPiece ()
//-----------------------------------------------------------------------------
piece* board::createPiece(int playerColor, PIECES pieceType, BOARD_POINT piecePos)
{
    piece* pCurPiece = nullptr;
    
    switch(pieceType)
    {
        case PAWN:
            pCurPiece = new Pawn(playerColor, piecePos);
            break;
            
        case KNIGHT:
            pCurPiece = new knight(playerColor, piecePos);
            break;
                
        case BISHOP:
            pCurPiece = new bishop(playerColor, piecePos);
            break;
                
        case ROOK:
            pCurPiece = new rook(playerColor, piecePos);
            break;
                
        case QUEEN:
            pCurPiece = new queen(playerColor, piecePos);
            break;
                
        case KING:
            pCurPiece = new king(playerColor, piecePos);
            break;
    }

    if (!pCurPiece)
        return nullptr;

    if (playerColor == BLACK)
        m_pawnsVec[UPPER].push_back(pCurPiece);
    else
        m_pawnsVec[BOTTOM].push_back(pCurPiece);
    
    m_pieceCreatedSig(pCurPiece);
    return pCurPiece; 
}
    
    
//-----------------------------------------------------------------------------
// Name : createStartingPiece ()
//-----------------------------------------------------------------------------
bool board::createStartingPiece(int i, int j, int playerColor)
{
    piece* pNewPiece = nullptr;
    BOARD_POINT piecePos(j,i);
    if (i == 1 || i == 6)
        pNewPiece = createPiece(playerColor, PAWN, piecePos);

    if (i == 0 || i == 7)
    {
        if (j == 0 || j == 7)
            pNewPiece = createPiece(playerColor, ROOK, piecePos);

        if (j == 1 || j == 6)
            pNewPiece = createPiece(playerColor, KNIGHT, piecePos);

        if (j == 2 || j == 5 )
            pNewPiece = createPiece(playerColor, BISHOP, piecePos);

        if (j == 3)
            pNewPiece = createPiece(playerColor, QUEEN, piecePos);

        if (j == 4)
        {
            pNewPiece = createPiece(playerColor, KING, piecePos);
            if (playerColor == BLACK)
                m_kings[UPPER] =  (king*)pNewPiece;
            else
                m_kings[BOTTOM] = (king*)pNewPiece;
        }
    }
    
    if(pNewPiece != nullptr)
    {
        m_board[i][j] = pNewPiece;
        return true;
    }
    else
        return false;
}

//-----------------------------------------------------------------------------
// Name : connectToPieceCreated ()
//-----------------------------------------------------------------------------
void board::connectToPieceCreated(const singal_pieceCreated::slot_type& subscriber)
{
    m_pieceCreatedSig.connect(subscriber);
}

//-----------------------------------------------------------------------------
// Name : connectToPieceMoved ()
//-----------------------------------------------------------------------------
void board::connectToPieceMoved(const signal_pieceMoved::slot_type& subscriber)
{
    m_pieceMovedSig.connect(subscriber);
}

//-----------------------------------------------------------------------------
// Name : connectToPieceKilled ()
//-----------------------------------------------------------------------------
void board::connectToPieceKilled(const signal_pieceKilled::slot_type& subscriber)
{
    m_pieceKilledSig.connect(subscriber);
}

//-----------------------------------------------------------------------------
// Name : connectToGameOver ()
//-----------------------------------------------------------------------------
void board::connectToGameOver(const singal_gameover::slot_type& subscriber)
{
    m_gameOverSig.connect(subscriber);
}

//-----------------------------------------------------------------------------
// Name : conntectToEndTurn ()
//-----------------------------------------------------------------------------
void board::conntectToEndTurn( const signal_endTurn::slot_type& subscriber )
{
    m_endTurnSig.connect(subscriber);
}

//-----------------------------------------------------------------------------
// Name : endTurn ()
//-----------------------------------------------------------------------------
void board::endTurn()
{
    m_currentPlayer *=-1;

    if (isKingInThreat( m_currentPlayer,true))
    {
        if(isEndGame( m_currentPlayer ))
        {
            m_gameActive = false;
            m_curStatus = "CheckMate";

            std::string endGameStatus;

            if ( m_currentPlayer == 1)
                endGameStatus = "Black Wins!";
            else
                endGameStatus = "White Wins!";

            m_gameOverSig(endGameStatus);

            int kingSide;

            if ( m_currentPlayer == 1)
                kingSide = BOTTOM;
            else
                kingSide = UPPER;

            BOARD_POINT curKingSquare  = m_kings[kingSide]->getPosition();
            //BOARD_POINT curKingSquare  = getPieceSquare(kings[kingSide]);
            //setThreatSquare(curKingSquare);
        }
        else
        {
            int kingSide;
            if ( m_currentPlayer == 1)
                kingSide = BOTTOM;
            else
                kingSide = UPPER;

            BOARD_POINT curKingSquare  = m_kings[kingSide]->getPosition();
            //BOARD_POINT curKingSquare  = getPieceSquare(kings[kingSide]);
            //setThreatSquare(curKingSquare);
            m_endTurnSig( m_currentPlayer );
        }
    }
    else
    {
        //m_threatSquare = -1;
        if (isDraw( m_currentPlayer ))
        {
            m_gameActive = false;
            m_curStatus = "Draw";

            std::string endGameStatus = "Draw";
            m_gameOverSig(endGameStatus);
        }
        else
        {
            m_endTurnSig( m_currentPlayer );
        }
    }

    m_startSquare.row = -1;
    m_startSquare.col = -1;
}

//-----------------------------------------------------------------------------
// Name : killPiece ()
//-----------------------------------------------------------------------------
void board::killPiece(piece * pPieceToKill, BOARD_POINT pieceSquare)
{
    int side;

    if ( m_currentPlayer == BLACK)
        side = BOTTOM;
    else
        side = UPPER;

    std::vector<piece*> &curVector = m_pawnsVec[side];

    for (unsigned int i = 0; i < curVector.size(); i++)
    {
        if (pPieceToKill == curVector[i])
        {
            //adding to a dead pawns vector and than removing it from the active vec as we don't want to completely lose the pointer to the object
            //pPieceToKill->setObjectHidden(true);
            m_pieceKilledSig(pPieceToKill);
            m_deadPawnsVec[side].push_back(pPieceToKill);
            curVector.erase(curVector.begin()+i);
            break;
        }
    }
}

//-----------------------------------------------------------------------------
// Name : getPieceSquare ()
//-----------------------------------------------------------------------------
// BOARD_POINT board::getPieceSquare(piece * pPiece)
// {
//     BOARD_POINT boardSquare(0,0);
// 
// //     D3DXMATRIX objWorldMat = pPiece->m_mtxWorld;
// //     D3DXVECTOR3 objectPos = D3DXVECTOR3(objWorldMat._41,objWorldMat._42,objWorldMat._43);
// // 
// //     // ( object posX - board posX ) / (stepX * sclaeX) = row coordinate of the square 
// //     boardSquare.row = (objectPos .x - m_pos.x) / (m_stepX * m_meshScale.x); 
// //     boardSquare.col = ( (m_pos.z - objectPos.z) / (m_stepZ * m_meshScale.z ) ) + (boardY - 1) + 0.5;
// 
//     return boardSquare;
// }

//-----------------------------------------------------------------------------
// Name : getBoardStatus ()
//-----------------------------------------------------------------------------
std::string board::getBoardStatus() const
{
    return m_curStatus;
}

//-----------------------------------------------------------------------------
// Name : isBoardActive ()
//-----------------------------------------------------------------------------
bool board::isBoardActive () const
{
    return m_gameActive;
}

//-----------------------------------------------------------------------------
// Name : isUnitPromotion ()
//-----------------------------------------------------------------------------
bool board::isUnitPromotion()  const
{
    return m_unitPromotion;
}

//-----------------------------------------------------------------------------
// Name : getKingThreat ()
//-----------------------------------------------------------------------------
bool board::getKingThreat () const
{
    return m_kingInThreat;
}

//-----------------------------------------------------------------------------
// Name : resetGame ()
//-----------------------------------------------------------------------------
bool board::resetGame()
{
    for (unsigned int i = 0; i < boardX; i++)
        for (unsigned int j = 0; j < boardY; j++)
                m_board[i][j] = nullptr;

    m_targetSqaure.row = 0;
    m_targetSqaure.col = 0;

    m_currentPawn = nullptr;
    m_prevPawn    = nullptr;

    m_kings[BOTTOM] = nullptr;
    m_kings[UPPER]  = nullptr;

    m_deadPawnsVec[UPPER].clear();
    m_deadPawnsVec[BOTTOM].clear();

    //clearing the pieces
    for (unsigned int j =0; j < 2; j++)
    {
        for (unsigned int i = 0; i < m_pawnsVec[j].size(); i++)
        {
            std::vector<piece*>& curPieceVec = m_pawnsVec[j];
            delete curPieceVec[i];
            curPieceVec[i] = nullptr;
        }
    }

    m_pawnsVec[UPPER].clear();
    m_pawnsVec[BOTTOM].clear();

    m_pieceCount = 0;
    for (int i = 0; i < 2; i++)
    {
        for (int j = 0; j < boardX; j++)
        {
            if(!createStartingPiece(i,j, BLACK))
                return false;
        }
    }

    for (int i = boardY - 2; i < boardY; i++)
    {
        for (int j = 0; j < boardX; j++)
        {
            if(!createStartingPiece(i,j, WHITE))
                return false;
        }
    }

    m_currentPlayer = 1;
    m_gameActive = true;
    m_kingInThreat = false;

    return true;
}

//-----------------------------------------------------------------------------
// Name : PromoteUnit ()
//-----------------------------------------------------------------------------
bool board::PromoteUnit(PIECES type)
{
    SIDES curSide;
    int pieceIndex = -1;

    if ( m_currentPawn->getColor() == 1)
        curSide = BOTTOM;
    else
        curSide = UPPER;

    std::vector<piece*>& curPieceVec = m_pawnsVec[curSide];

    if ( m_currentPawn )
    {
        for (unsigned int i = 0; i < curPieceVec.size(); i++)
        {
            if (curPieceVec[i] == m_currentPawn )
            {
                pieceIndex = i;
                curPieceVec.erase(curPieceVec.begin() + pieceIndex);//remove the piece from the vector
                break;
            }
        }

        if (pieceIndex != -1)
        {
            if ( m_currentPawn->getColor() == WHITE)
                pieceIndex += 16; 

            m_currentPawn = nullptr;
            piece* pNewPiece = nullptr;
            if ( m_currentPlayer == WHITE)
                pNewPiece = createPiece(WHITE, type, m_targetSqaure );
            else
                pNewPiece = createPiece(BLACK, type, m_targetSqaure );

            if (pNewPiece)
                m_board[m_targetSqaure.col][m_targetSqaure.row] = pNewPiece;
            else
                return false;
            
            m_unitPromotion = false;
            //the new promoted piece is now part of the board so we can end the player turn
            endTurn();
        }

    }
    else 
        return false;

    return true;
}

//-----------------------------------------------------------------------------
// Name : castling ()
//-----------------------------------------------------------------------------
bool board::castling(int dx , int dy)
{

    int x = dx/abs(dx);
    int y = m_startSquare.col;

    x = m_startSquare.row - dx -x;

    BOARD_POINT newRookSquare(x + 2 * (dx / abs(dx)), y);
//     newRookSquare.col = y;
//     newRookSquare.row = x + 2 * (dx / abs(dx));

    piece * rookPiece = m_board[y][x]; 
    if (rookPiece != NULL)
    {
        if (rookPiece->getType() == ROOK && !rookPiece->isMoved())
        {
            dx = dx / abs(dx);
            for (int i = m_startSquare.row - dx; i != x; i -= dx)
            {
                m_targetSqaure.col = y;
                m_targetSqaure.row = i;
                if ( validateKingThreat( m_currentPlayer ) )
                    return false;
            }
            
            return true;
        }
    }
    
    return false;
} 
