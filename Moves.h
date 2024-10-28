#ifndef MOVES_H_INCLUDED
#define MOVES_H_INCLUDED
//#include <vector>
#include "Vector.h"
//#include <list>
#include "List.h"
#include "Field.h"
#include "Board.h"
class cMoves    // класс цепочек перемещений
{
    struct sMove    // структура, которая как бы класс
    {
        sMove(cField* from, cField* to, bool toQueen, cField* eat);
        ~sMove();
        cField* _pFrom;
        cField* _pTo;
        cField* _pEat;
        CHECK _eatCheck = NO;
        bool _toQueen;  // превращается ли шашка в процессе хода в дамку
    };
public:
    cMoves();
    cMoves(cField* from, cField* to, bool toQueen, cField* eat); // создать объект с одним елементом
    ~cMoves();
    // методы БЕЗ внесения изменений в списки шашек и перечень изменений (для проверок возможных перемещений)
    void AddMove(cField* from, cField* to, bool toQueen, cField* eat);
    void AddAndMakeMove(cField* from, cField* to, bool toQueen, cField* eat);
    void RemoveLastMove(void);
    void RemoveAndUnMakeLastMove(void);
    size_t GetSize(void) const;
    cField* const GetMoveFrom(void) const;
    cField* const GetMoveTo(void) const;
    void MakeAllMove(cBoard* pBoard);
    bool MakeAllMove(bool change = false, cBoard* pBoard = nullptr); // с изменениями в списках, если указана доска
    void MakeAllUnMove(bool change = false, cBoard* pBoard = nullptr);
    bool isEmpty(void);
    void CheckLastMoveReachQueenLine(cBoard* const pBoard); // проверка достижения шашкой противоположной границы последним перемещением (в дамки)
    void SetFieldsMarkersByMoves(bool mark, bool canFrom, bool canTo, bool selFrom, bool selTo, cBoard* pBoard = nullptr);
    void ResetFieldsMarkers(cBoard* pBoard = nullptr);
private:
    void MakeMove(sMove& sMv);  // БЕЗ внесения изменений в списки шашек и в перечень изменений
    void MakeUnMove(sMove& sMv);
    // спец методы внесения изменений в списки шашек
    //void MarkMoveResultsOnCheckersLists(sMove& sMv, std::list<cField*>* pMain, std::list<cField*>* pEated);
    //void MarkUnMoveResultsOnCheckersLists(sMove& sMv, std::list<cField*>* pMain, std::list<cField*>* pEated);
    void MarkMoveResultsOnCheckersLists(sMove& sMv, cList<cField*>* pMain, cList<cField*>* pEated);
    void MarkUnMoveResultsOnCheckersLists(sMove& sMv, cList<cField*>* pMain, cList<cField*>* pEated);
    void AddChanges(cBoard* pBoard, sMove& sMv); // добавить изменения в перечень изменений (cBoard._Changes)
    //void SetFieldsMarkersByMoves(bool mark, bool selFrom, bool selTo);
    //std::vector<sMove> _moves;  // хранилище цепочки перемещений (1 ход для простых, и несколько для сложных взятий)
    cVector<sMove> _moves;
};
#endif // MOVES_H_INCLUDED
