#ifndef GAME_H_INCLUDED
#define GAME_H_INCLUDED
#include <array>
#include <list>
#include "List.h"
//#include <vector>
#include "Vector.h"
#include <windows.h>
#include "InputPort.h"
#include "Field.h"
#include "Moves.h"
#include "Board.h"
//#include "MinMax.h"
#include "NTree.h"
//
class cGame
{
public:
    cGame(SHORT fields, cInputPort* const pIp);
    ~cGame();
    //std::vector<cField*>& GetChanges(void);
    cVector<cField*>& GetChanges(void);
    SHORT GetFields(void);
    void Update(void);
    CHECK GetSide(void);
    cBoard* GetBoardPtr(void);
    //bool FindAllPosibleMoves(CHECK side, std::list<cMoves>* pMovesLst);
    bool FindAllPosibleMoves(CHECK side, cList<cMoves>* pMovesLst);
private:
    cBoard _Board;
    //cMinMax* _pMinMax = nullptr;
    cNTree* _pNTree = nullptr;
    SHORT _fields;      // размер пол€
    SHORT _checkers;    // кол-во шашек одного цвета
    CHECK _side; // активна€ сторона
    cInputPort* _pInPort = nullptr;
    bool KeyPress(int key);
    // game status
    bool _gameStart = true;
    bool _gameRun = false;
    bool _gameEnd = false;
    //
    bool _whiteIsHuman = true;  // true - player is human, false - program
    bool _blackIsHuman = false;  //
    //
    bool IsHuman(CHECK side = NO);
    void GameStart(void);
    bool GameRun(void);
    int GameEnd(void);
    void ChangeSide(void);
    //bool FindAllPosibleMoves(CHECK side, std::list<cMoves>* pMovesLst);
    //void FindSimpleMoves(cField* const pFld, std::list<cMoves>* pMovesLst); // находит список простых перемещений дл€ одной шашки
    void FindSimpleMoves(cField* const pFld, cList<cMoves>* pMovesLst); // находит список простых перемещений дл€ одной шашки
    //void FindAllSimpleMoves(CHECK side, std::list<cMoves>* pMovesLst);
    void FindAllSimpleMoves(CHECK side, cList<cMoves>* pMovesLst);
    //void FindAllSimpleMoves(CHECK side, std::list<std::list<cMoves>>* pAllMovesLst); // зполнение списка(по кол-ву шашек) списков (перемещений) дл€ указанной стороны
    void FindAllSimpleMoves(CHECK side, std::list<cList<cMoves>>* pAllMovesLst); // зполнение списка(по кол-ву шашек) списков (перемещений) дл€ указанной стороны
    //size_t FindJumpingMoves(cField* const pFld, std::list<cMoves>* pMovesLst);  // заполн€ет список ходов со вз€тием, возвращает размер ћј —»ћјЋ№Ќќ√ќ хода дл€ этой шашки
    size_t FindJumpingMoves(cField* const pFld, cList<cMoves>* pMovesLst);  // заполн€ет список ходов со вз€тием, возвращает размер ћј —»ћјЋ№Ќќ√ќ хода дл€ этой шашки
    //void FindAllJumpingMoves(CHECK side, std::list<cMoves>* pMovesLst);
    void FindAllJumpingMoves(CHECK side, cList<cMoves>* pMovesLst);
    //void FindAllJumpingMoves(CHECK side, std::list<std::list<cMoves>>* pAllMovesLst, bool mark = true);
    void FindAllJumpingMoves(CHECK side, std::list<cList<cMoves>>* pAllMovesLst, bool mark = true);
    //void RemoveShortJumpingMoves(std::list<std::list<cMoves>>* pAllMovesLst, size_t maxJumps = 0);
    void RemoveShortJumpingMoves(std::list<cList<cMoves>>* pAllMovesLst, size_t maxJumps = 0);
    //cMoves* SelectMoves(std::list<std::list<cMoves>>& sAllMovesLst, std::list<std::list<cMoves>>::iterator& sItMoves);
    cMoves* SelectMoves(std::list<cList<cMoves>>& sAllMovesLst, std::list<cList<cMoves>>::iterator& sItMoves);
    //cMoves* SelectMinMax(std::list<std::list<cMoves>>& sAllMovesLst, cNTree* pNTree);
    cMoves* SelectMinMax(std::list<cList<cMoves>>& sAllMovesLst, cNTree* pNTree);
    //bool AddNodeToNTree(cNTree* pNTree);
    bool CheckEndGame(bool isEmpty);
    bool CheckDraw(void);
    bool OnlyQueenOnSide(CHECK side);
    bool CheckersInDrawOnSide(CHECK side);
    //std::list<cField*>* GetCheckersSidePtr(CHECK side);
    cList<cField*>* GetCheckersSidePtr(CHECK side);
};

#endif // GAME_H_INCLUDED
