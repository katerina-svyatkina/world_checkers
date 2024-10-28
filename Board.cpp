#include "Board.h"
#include "Misc.h"
#include <algorithm>
cBoard::sBoardData::sBoardData()
{
    _ppBoardArr = new cField* [FIELDS];// массив указателей на одномерные массивы
    _ppBoardArr[0] = new cField[FIELDS * FIELDS]; // в нулевой позиции создаем массив на все поля
    for(SHORT i = 1; i < FIELDS; i++)
    {
        _ppBoardArr[i] = _ppBoardArr[0] + i * FIELDS; // остальным позициям присваиваем адреса из массива со смещением
    }
}
cBoard::sBoardData::~sBoardData()
{

}
cBoard::cBoard()
{

}
cBoard::~cBoard()
{

}
void cBoard::AddChange(cField* pFld)
{
    if(!_Changes.empty())
    {
        //if(std::find(_Changes.begin(), _Changes.end(), pFld) != _Changes.end())
        //{
        //    return;
        //}
        if(_Changes.consist(pFld) != _Changes.size()) //
        {
            return;
        }
    }
    //Print(std::to_string(pFld->GetPosition().X) + " " + std::to_string(pFld->GetPosition().Y));
        ///      std::to_string(_moves.back()._pTo->GetPosition().X) + " " + std::to_string(_moves.back()._pTo->GetPosition().Y));
    _Changes.push_back(pFld);
}
//std::vector<cField*>& cBoard::GetChanges(void)
cVector<cField*>& cBoard::GetChanges(void)
{
    return _Changes;
}
cList<cField*>* cBoard::GetWhiteListPtr(void)
{
    return &(_boardData._White);
}
cList<cField*>* cBoard::GetBlackListPtr(void)
{
    return &(_boardData._Black);
}
cField* cBoard::GetFieldPtr(const COORD& pos)
{
    return GetFieldPtr(pos.X, pos.Y);
}
cField* cBoard::GetFieldPtr(SHORT x, SHORT y)
{
    return (x < 0 || x >= FIELDS || y < 0 || y >= FIELDS) ? nullptr : &(_boardData._ppBoardArr[x][y]);
}
void cBoard::AddFieldToList(cField* pFld, bool black)
{
    if(black)
    {
        _boardData._Black.push_back(pFld);
    }
    else
    {
        _boardData._White.push_back(pFld);
    }
}
void cBoard::PlaceCheckers(bool black, SHORT num)
{
    SHORT from = black ? FIELDS - 1 : 0;
    SHORT to = black ? -1 : FIELDS;
    SHORT dir = black ? -1 : 1;
    CHECK chk = black ? BLACKCHECK : WHITECHECK;
    for (SHORT y = from; y != to; y += dir)
    {
        for(SHORT x = from; x != to; x += dir)
        {
            cField* pFld = GetFieldPtr(x,y);
            if (pFld->GetBlack())
            {
                pFld->SetChecker(chk);
                AddChange(pFld); // запомнили изменение
                AddFieldToList(pFld, black);///
                --num;  // уменьшили счётчик кол-ва шашек к размещению
            }
        }
        if(num <= 0) break; // если всё разместили, заканчиваем перебирать поле
    }
}
void cBoard::SaveBoardData(void)
{
    _boardDataSave = _boardData;    /// по идее внутренние контейнеры должны скопироваться (нужно проверять)
}
void cBoard::RestoreBoardData(void)
{
    _boardData = _boardDataSave;
}
void cBoard::ScanFreeFieldsOnDirection(cField* const pFld, const COORD& dir, cList<cField*>* pOutLst)
{
    ScanFreeFieldsOnDirection(pFld, dir, pFld->GetCheckerIsQueen(), pOutLst);
}
void cBoard::ScanFreeFieldsOnDirection(cField* const pFld, const COORD& dir, bool queen, cList<cField*>* pOutLst)
{   /// Working (Refactoring - do...while ?)
    COORD pos = pFld->GetPosition();
    SHORT deep = queen ? 10 : 1; // если обычная шашка, глубина сканирования 1 клетка, для дамки 10 (с запасом)
    for (SHORT i = 0; i < deep; ++i)
    {
        ///pos += dir;
        cField* pFree = GetFieldPtr((pos += dir)); /// проверить
        if (!pFree || pFree->GetChecker())  // если указатель на поле пустой(вышли за границы доски), или на этом поле есть шашка
        {
            return;
        }
        pOutLst->push_back(pFree);
    }
}
cField* cBoard::ScanOpositeCheckerOnDirection(cField* const pFld, const COORD& dir)
{   /// working
    /// вероятно требует оптимизации - много точек выхода?. Пока идей нет.
    COORD pos = pFld->GetPosition();
    SHORT deep = pFld->GetCheckerIsQueen() ? 10 : 1;
    for (SHORT i = 0; i < deep; ++i)    /// (Refactoring - do...while ?)
    {
        pos += dir;
        cField* pOposite = GetFieldPtr(pos);
        if (!pOposite)  // если нет указателя (вышли за пределы поля)
        {
            return nullptr; //break;
        }
        if (pOposite->GetChecker()) // если встретили шашку
        {
            if (pOposite->GetCheckerIsBlack()!= pFld->GetCheckerIsBlack())  // и она другого цвета
            {
                return pOposite; // нашли
            }
            return nullptr; //break;
        }
    }
    return nullptr;
}
bool cBoard::CheckerReachOpositeSide(cField* const pFldFrom, cField* const pFldTo)
{
    if(!pFldFrom || !pFldTo) return false;  // проверка на НЕ пустые входные данные
    if(pFldFrom->GetCheckerIsQueen()) return false; // если шашка уже в дамках
    SHORT reachPosY = pFldFrom->GetCheckerIsBlack() ? 0 : FIELDS - 1; // номер строки, которой нужно достичь шашке для превращения в дамку, в зависимости от цвета
    if(pFldTo->GetPosition().Y == reachPosY) return true; // если достигли противоположной строки (позиции по Y)
    return false;
}
bool cBoard::CheckDraw(void)
{
    CHECK side = WHITECHECK;
    {
        if (OnlyQueenOnSide(side) || OnlyQueenOnSide(side = BLACKCHECK)) // если у одной из сторон осталась единственная шашка и это дамка
        {
            side = (side == WHITECHECK) ? BLACKCHECK : WHITECHECK; // переключаемся на другую сторону
            if (CheckersInDrawOnSide(side))  // если у другой стороны состав шашек "ничья"
                return true;
        }
    }
    //
    return false;
}
bool cBoard::OnlyQueenOnSide(CHECK side)
{
    cList<cField*>* pChkLst = (side == BLACKCHECK) ? GetBlackListPtr() : GetWhiteListPtr();//GetCheckersSidePtr(side);
    if (pChkLst->size() == 1)
    {
        if (pChkLst->front()->GetCheckerIsQueen())
            return true;
    }
    return false;
}
bool cBoard::CheckersInDrawOnSide(CHECK side)
{
    cList<cField*>* pChkLst = (side == BLACKCHECK) ? GetBlackListPtr() : GetWhiteListPtr();
    size_t sz = pChkLst->size();
    if (sz > 0 && sz < 4)    // если шашек от 1 до 3
    {
        //for(cField* pFld : *pChkLst)
        for (cField** ppFld = pChkLst->begin(); ppFld != nullptr; ppFld = pChkLst->next())
        {
            if ((*ppFld)->GetCheckerIsQueen()) // и хотя бы одна из них дамка
                return true;    // выполнено условие "ничья" для данной стороны
        }
    }
    return false;
}