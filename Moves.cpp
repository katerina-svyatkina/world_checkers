#include "Moves.h"
#include "Misc.h"
#include <algorithm>
//
cMoves::sMove::sMove(cField* from, cField* to, bool toQueen, cField* eat = nullptr)
{
    _pFrom = from;
    _pTo = to;
    _pEat = eat;
    _toQueen = toQueen;
    if (_pEat)  ///шашка еще не должна быть съедена(убрана), когда создаётся cMoves
        _eatCheck = _pEat->GetChecker();
}
cMoves::sMove::~sMove()
{

}
/// -------------------------------------- ///
cMoves::cMoves()
{

}
cMoves::cMoves(cField* from, cField* to, bool toQueen, cField* eat = nullptr)
{
    AddMove(from, to, toQueen, eat);
}
cMoves::~cMoves()
{

}
void cMoves::AddMove(cField* from, cField* to, bool toQueen, cField* eat = nullptr)
{
    _moves.emplace_back(from, to, toQueen, eat); /// not here problem
    //sMove mv(from,to,toQueen,eat);
    //_moves.push_back(mv);
    //Print(std::to_string(from->GetPosition().X) + " " + std::to_string(from->GetPosition().Y) + " - " +
     //     std::to_string(to->GetPosition().X) + " " + std::to_string(to->GetPosition().Y));
}
void cMoves::AddAndMakeMove(cField* from, cField* to, bool toQueen, cField* eat)
{
    AddMove(from, to, toQueen, eat);   // _moves.emplace_back(...) быстрее
    MakeMove(_moves.back());
}
void cMoves::RemoveLastMove(void)
{
    if(_moves.empty()) return;
    _moves.pop_back();
}
void cMoves::RemoveAndUnMakeLastMove(void)
{
    if(_moves.empty()) return;
    MakeUnMove(_moves.back());
    _moves.pop_back();  // RemoveLastMove правильнее
}
cField* const cMoves::GetMoveFrom(void) const
{
    if(_moves.empty()) return nullptr;
    return _moves.front()._pFrom;
}
cField* const cMoves::GetMoveTo(void) const
{
    if(_moves.empty()) return nullptr;
    return _moves.back()._pTo;
}
void cMoves::MakeAllMove(cBoard* pBoard)
{
    if(_moves.empty())
    {
        return;
    }
    bool chkIsBlack = _moves.front()._pFrom->GetCheckerIsBlack(); /// back() ???
    //std::list<cField*>* pMain = chkIsBlack ? pBoard->GetBlackListPtr() : pBoard->GetWhiteListPtr();
    //std::list<cField*>* pEated = chkIsBlack ? pBoard->GetWhiteListPtr() : pBoard->GetBlackListPtr();
    cList<cField*>* pMain = chkIsBlack ? pBoard->GetBlackListPtr() : pBoard->GetWhiteListPtr();
    cList<cField*>* pEated = chkIsBlack ? pBoard->GetWhiteListPtr() : pBoard->GetBlackListPtr();
    /*for(auto sMv : _moves)
    {
        MakeMove(sMv);
        MarkMoveResultsOnCheckersLists(sMv, pMain, pEated);
    }*/
    for (size_t i = 0; i < _moves.size(); ++i)
    {
        MakeMove(_moves[i]);
        MarkMoveResultsOnCheckersLists(_moves[i], pMain, pEated);
    }
}
bool cMoves::MakeAllMove(bool change, cBoard* pBoard)
{   /// итерируемый метод (возвращает false пока постепенно не сделает все ходы)
    static size_t countBtwMove = 1;
    static size_t posMoves = 0;
    if(_moves.empty())
    {
        return true;
    }
    //std::list<cField*>* pMain = nullptr; //= chkIsBlack ? &_Black : &_White;
    //std::list<cField*>* pEated = nullptr; //= chkIsBlack ? &_White : &_Black;
    cList<cField*>* pMain = nullptr; //= chkIsBlack ? &_Black : &_White;
    cList<cField*>* pEated = nullptr; //= chkIsBlack ? &_White : &_Black;
    if (!countBtwMove--)
    {
        countBtwMove = 3;   /// magic digit
        if(pBoard)
        {
            bool chkIsBlack = _moves[posMoves]._pFrom->GetCheckerIsBlack(); /// back() ???
            pMain = chkIsBlack ? pBoard->GetBlackListPtr() : pBoard->GetWhiteListPtr();
            pEated = chkIsBlack ? pBoard->GetWhiteListPtr() : pBoard->GetBlackListPtr();
        }
        MakeMove(_moves[posMoves]);
        if(pBoard)
        {
            MarkMoveResultsOnCheckersLists(_moves[posMoves], pMain, pEated);
            if(change) AddChanges(pBoard, _moves[posMoves]);
        }
        if(++posMoves == _moves.size())
        {
            posMoves = 0;
            return true;
        }
    }
    return false;
    /*if(pBoard)
    {
        bool chkIsBlack = _moves.front()._pFrom->GetCheckerIsBlack(); /// back() ???
        pMain = chkIsBlack ? pBoard->GetBlackListPtr() : pBoard->GetWhiteListPtr();
        pEated = chkIsBlack ? pBoard->GetWhiteListPtr() : pBoard->GetBlackListPtr();
    }
    for(sMove& mv : _moves)
    {
        MakeMove(mv);
        if(pBoard)
        {
            MarkMoveResultsOnCheckersLists(mv, pMain, pEated);
            AddChanges(pBoard, mv);
        }
    }*/
}
void cMoves::MakeAllUnMove(bool change, cBoard* pBoard)
{   /// НЕ итереируемый метод (нужен только для MinMax)
    if(_moves.empty()) return;
    //std::list<cField*>* pMain = nullptr;
    //std::list<cField*>* pEated = nullptr;
    cList<cField*>* pMain = nullptr;
    cList<cField*>* pEated = nullptr;
    if(pBoard)
    {
        bool chkIsBlack = _moves.back()._pTo->GetCheckerIsBlack(); /// ok ????
        pMain = chkIsBlack ? pBoard->GetBlackListPtr() : pBoard->GetWhiteListPtr();
        pEated = chkIsBlack ? pBoard->GetWhiteListPtr() : pBoard->GetBlackListPtr();
    }
    for(int i = _moves.size() - 1; i >= 0; --i)
    {
        MakeUnMove(_moves[i]);
        if(pBoard)
        {
            MarkUnMoveResultsOnCheckersLists(_moves[i], pMain, pEated);
            if(change) AddChanges(pBoard, _moves[i]);
        }
    }
}
bool cMoves::isEmpty(void)
{
    return _moves.empty();
}
size_t cMoves::GetSize(void) const
{
    return _moves.size();
}
void cMoves::MakeMove(sMove& sMv)
{
    if (!sMv._pFrom || !sMv._pTo) return;
    if (!sMv._pFrom->GetChecker() || sMv._pTo->GetChecker()) return;
    CHECK chk = sMv._pFrom->GetChecker();
    if (sMv._toQueen)
    {
        if (chk == WHITECHECK) chk = WHITEQUEEN;
        else if (chk == BLACKCHECK) chk = BLACKQUEEN;
    }
    sMv._pTo->SetChecker(chk);
    sMv._pFrom->SetChecker(NO);
    if (!sMv._pEat) return;
    sMv._eatCheck = sMv._pEat->GetChecker();
    sMv._pEat->SetChecker(NO);
}
void cMoves::MakeUnMove(sMove& sMv)
{
    if (!sMv._pFrom || !sMv._pTo) return;
    if (!sMv._pTo->GetChecker() || sMv._pFrom->GetChecker()) return;
    CHECK chk = sMv._pTo->GetChecker();
    if (sMv._toQueen)
    {
        if (chk == WHITEQUEEN) chk = WHITECHECK;
        else if (chk == BLACKQUEEN) chk = BLACKCHECK;
    }
    sMv._pFrom->SetChecker(chk);
    sMv._pTo->SetChecker(NO);
    if (!sMv._pEat) return;
    sMv._pEat->SetChecker(sMv._eatCheck);
    sMv._eatCheck = NO;
}
//void cMoves::MarkMoveResultsOnCheckersLists(sMove& sMv, std::list<cField*>* pMain, std::list<cField*>* pEated)
void cMoves::MarkMoveResultsOnCheckersLists(sMove& sMv, cList<cField*>* pMain, cList<cField*>* pEated)
{   /// использовать исключительно в паре(после) с MakeMove
    //std::list<cField*>::iterator itMain = std::find(pMain->begin(), pMain->end(), sMv._pFrom); // находим запись про старое местоположение шшашки (итератор)
    cField** ppMain = pMain->find(sMv._pFrom);
    /// auto = std::list<cField*>::iterator
    //Print(std::to_string(reinterpret_cast<DWORD>(*itMain)));
    //*itMain = sMv._pTo; // заносим новое положение
    *ppMain = sMv._pTo; // заносим новое положение
    //Print(std::to_string(reinterpret_cast<DWORD>(*itMain)));
    if(sMv._pEat)    // если есть съеденная шашка
    {   /// иногда пытается удалить пустой итератор !!!!!!!!!
        //std::list<cField*>::iterator itEat = std::find(pEated->begin(), pEated->end(), sMv._pEat);   // находим в списке съедаемую шашку (итератор)
        cField** ppEat = pEated->find(sMv._pEat);
        //if(itEat == pEated->end())
        if(ppEat == nullptr)
        {
            Print("oops");
            return;
        }
        pEated->erase(ppEat);  // удаляем её из списка
    }
}
//void cMoves::MarkUnMoveResultsOnCheckersLists(sMove& sMv, std::list<cField*>* pMain, std::list<cField*>* pEated)
void cMoves::MarkUnMoveResultsOnCheckersLists(sMove& sMv, cList<cField*>* pMain, cList<cField*>* pEated)
{   /// использовать исключительно в паре(после) с MakeUnMove
    //std::list<cField*>::iterator itMain = std::find(pMain->begin(), pMain->end(), sMv._pTo); // находим запись про старое местоположение шашки (итератор)
    cField** ppMain = pMain->find(sMv._pTo);
    /// auto = std::list<cField*>::iterator
    //*itMain = sMv._pFrom; // заносим новое положение
    *ppMain = sMv._pFrom; // заносим новое положение
    if(sMv._pEat)    // если есть съеденная шашка
    {
        pEated->push_back(sMv._pEat);   // возвращаем в список востановленную шашку
        pEated->sort(); /// возможное решение проблемы возвращения списков к первоначальному состоянию
    }
}
void cMoves::AddChanges(cBoard* pBoard, sMove& sMv) //
{
    if(pBoard)
    {
        pBoard->AddChange(sMv._pFrom);
        pBoard->AddChange(sMv._pTo);
        if(sMv._pEat)
        {
            pBoard->AddChange(sMv._pEat);
        }
    }
}
void cMoves::CheckLastMoveReachQueenLine(cBoard* const pBoard)
{
    if(pBoard && !_moves.empty())
    {   /// тут была ошибка, при обращении когда ход уже сделан, в качестве From и To нужно указывать конечное положение шашки
        _moves.back()._toQueen = pBoard->CheckerReachOpositeSide(_moves.back()._pTo, _moves.back()._pTo); /// _pFrom
    }
}
void cMoves::SetFieldsMarkersByMoves(bool mark, bool canFrom, bool canTo, bool selFrom, bool selTo, cBoard* pBoard)
{
    if(!_moves.empty())
    {
        ///Print(std::to_string(_moves.front()._pFrom->GetPosition().X) + " " + std::to_string(_moves.front()._pFrom->GetPosition().Y) + "->" +
        ///      std::to_string(_moves.back()._pTo->GetPosition().X) + " " + std::to_string(_moves.back()._pTo->GetPosition().Y));
        _moves.front()._pFrom->SetCanBeSelected(canFrom);
        _moves.front()._pFrom->SetSelected(selFrom);
        _moves.back()._pTo->SetCanBeSelected(canTo);
        _moves.back()._pTo->SetSelected(selTo);
        /*for(sMove& sMv : _moves)
        {
            sMv._pTo->SetOverJump(mark);
            if(pBoard)
            {
                AddChanges(pBoard, sMv);
            }
        }*/
        for(size_t i = 0; i < _moves.size(); ++i)
        {
            _moves[i]._pTo->SetOverJump(mark);
            if(pBoard)
            {
                AddChanges(pBoard, _moves[i]);
            }
        }
    }
}
void cMoves::ResetFieldsMarkers(cBoard* pBoard)
{
    if(!_moves.empty())
    {
        /*for(sMove& sMv : _moves)
        {
            sMv._pFrom->SetSelected(false);
            sMv._pFrom->SetCanBeSelected(false);
            sMv._pFrom->SetOverJump(false);
            sMv._pTo->SetSelected(false);
            sMv._pTo->SetCanBeSelected(false);
            sMv._pTo->SetOverJump(false);
            AddChanges(pBoard, sMv);
        }*/
        for(size_t i = 0; i < _moves.size(); ++i)
        {
            _moves[i]._pFrom->SetSelected(false);
            _moves[i]._pFrom->SetCanBeSelected(false);
            _moves[i]._pFrom->SetOverJump(false);
            _moves[i]._pTo->SetSelected(false);
            _moves[i]._pTo->SetCanBeSelected(false);
            _moves[i]._pTo->SetOverJump(false);
            AddChanges(pBoard, _moves[i]);

        }
    }
}
