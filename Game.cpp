#include "Game.h"
#include <string>
#include "Misc.h"
#include "MinMax.h"
const SHORT infoX = 15;
const SHORT infoY = 35;
const int maxDeep = 4;/// максимальная глубина просчёта ходов алгоритма MinMax !!!! НЕ рекомендуется ставить больше 10 !!!!
cMinMax* _pMinMax = nullptr;
cGame::cGame(SHORT fields, cInputPort* const pIp)
{
    if (pIp) _pInPort = pIp;
    _side = NO;
    if (fields < 4) fields = 4; // ограничения по размеру поля
    if (fields > 12) fields = 12;
    if (fields % 2) fields = (fields / 2) * 2;  // приведение к четному числу клеток
    _fields = fields;
    _checkers = ((_fields - 2) / 2) * (_fields / 2);    // высчитываем кол-во шашек одного цвета
    // заполнение клеток доски
    bool black = false;  // первая клетка черная
    for(SHORT x = 0; x < _fields; ++x)
    {
        for(SHORT y = 0; y < _fields; ++y)
        {
            cField* pFld = _Board.GetFieldPtr(x, y);
            pFld->SetPosition(x,y);
            pFld->SetBlack(black);
            black = !black;
            _Board.AddChange(pFld);  /// при любом изменении необходимо запоминать элемент, в котором произошли изменения
        }
        black = !black;
    }
    _pMinMax = new cMinMax(&_Board);
    //_pTree = new cXTree;
}
cGame::~cGame()
{
    if(_pMinMax) delete _pMinMax;
    if(_pNTree) delete _pNTree;
}
bool cGame::KeyPress(int key)
{
    return _pInPort->GetKeyPress(key);
}
//std::vector<cField*>& cGame::GetChanges()
cVector<cField*>& cGame::GetChanges()
{
    return _Board.GetChanges();
}
SHORT cGame::GetFields(void)
{
    return _fields;
}
void cGame::Update(void)
{
    /// do something
    if (_gameStart)
    {
        GameStart();
        _gameStart = false;
        _gameRun = true;
    }
    if ((_gameRun = GameRun()))
    {

    }
}
CHECK cGame::GetSide(void)
{
    return _side;
}
cBoard* cGame::GetBoardPtr(void)
{
    return &_Board;
}
bool cGame::IsHuman(CHECK side)
{
    if(!side) side = _side;
    if(side == WHITECHECK) return _whiteIsHuman;
    return _blackIsHuman;
}
void cGame::GameStart(void)
{
    _Board.PlaceCheckers(true,_checkers);
    _Board.PlaceCheckers(false,_checkers);
    _side = BLACKCHECK;
    ///_pMinMax = new cMinMax(_White, _Black, _fields);
    ChangeSide();
}
bool cGame::GameRun()
{
    // states
    static bool moveable = true;
    static bool select = false;
    static bool moved = false;
    //
    //static std::list<std::list<cMoves>> Moves;
    static std::list<cList<cMoves>> Moves;
    //static std::list<std::list<cMoves>>:: iterator itMoves;
    static std::list<cList<cMoves>>:: iterator itMoves;
    //
    static cMoves* pMove;
    if(moveable)
    {
        Moves.clear();
        FindAllJumpingMoves(_side, &Moves);
        if(Moves.empty())
        {
            FindAllSimpleMoves(_side, &Moves);
            //if(Moves.empty())
                //return false;
        }
        ///
        /// Видимо тут проверка на конец игры / ничью
        if (CheckEndGame(Moves.empty()))
            return false;
        ///
        ///
        itMoves = Moves.begin();
        moveable = false;
        select = true;
    }
    if(select)
    {
        if(IsHuman())
        {
            pMove = SelectMoves(Moves, itMoves);
        }
        else
        {
            if(_pNTree) delete _pNTree;
            _pNTree = new cNTree(&Moves, _pMinMax->CalcEvalution());
            _pNTree->SetMaxDeep(maxDeep);
            /// MinMax
            pMove = SelectMinMax(Moves, _pNTree);
        }
        if (pMove)
        {
            moved = true;
            select = false;
        }
    }
    if(moved)
    {
        if(pMove->MakeAllMove(true, &_Board))
        {
            ChangeSide();
            moved = false;
            moveable = true;
        }
    }
    return true;
}
bool cGame::CheckEndGame(bool isEmpty)
{
    ///
    if(isEmpty)
    {
        std::string win = (_side == WHITECHECK) ? "Black " : "White ";
        win += "WIN !";
        PrintAt(infoX, infoY, win);
        return true; /// конец игры, т.к. нет доступных ходов (все шашки съели или заблокированы перемещения)
    }
    if(CheckDraw())
    {
        std::string win = "Pease WIN !"; /// ничья - "Победила Дружба !"
        PrintAt(infoX, infoY, win);
        return true;
    }
    return false;
}
bool cGame::CheckDraw(void)
{
    CHECK side = WHITECHECK;
    {
        if(OnlyQueenOnSide(side) || OnlyQueenOnSide(side = BLACKCHECK)) // если у одной из сторон осталась единственная шашка и это дамка
        {
            side = (side == WHITECHECK) ? BLACKCHECK : WHITECHECK; // переключаемся на другую сторону
            if(CheckersInDrawOnSide(side))  // если у другой стороны состав шашек "ничья"
                return true;
        }
    }
    //
    return false;
}
bool cGame::OnlyQueenOnSide(CHECK side)
{
    //std::list<cField*>* pChkLst = GetCheckersSidePtr(side);
    cList<cField*>* pChkLst = GetCheckersSidePtr(side);
    if(pChkLst->size() == 1)
    {
        if(pChkLst->front()->GetCheckerIsQueen())
            return true;
    }
    return false;
}
bool cGame::CheckersInDrawOnSide(CHECK side)
{
    //std::list<cField*>* pChkLst = GetCheckersSidePtr(side);
    cList<cField*>* pChkLst = GetCheckersSidePtr(side);
    size_t sz = pChkLst->size();
    if(sz > 0 && sz < 4)    // если шашек от 1 до 3
    {
        //for(cField* pFld : *pChkLst)
        for(cField** ppFld = pChkLst->begin(); ppFld != nullptr; ppFld = pChkLst->next())
        {
            if((*ppFld)->GetCheckerIsQueen()) // и хотя бы одна из них дамка
                return true;    // выполнено условие "ничья" для данной стороны
        }
    }
    return false;
}
cList<cField*>* cGame::GetCheckersSidePtr(CHECK side)
{
    return (side == WHITECHECK) ? _Board.GetWhiteListPtr() : _Board.GetBlackListPtr();
}
//cMoves* cGame::SelectMoves(std::list<std::list<cMoves>>& sAllMovesLst, std::list<std::list<cMoves>>::iterator& sItMoves)
cMoves* cGame::SelectMoves(std::list<cList<cMoves>>& sAllMovesLst, std::list<cList<cMoves>>::iterator& sItMoves)
{
    static bool selChecker = true;  // сосстояние выбора шашки, которой ходить
    static bool selMoveTo = false;  // состояние выбора места куда ходить выбранной шашкой
    /// !!!!!!! переделать БЕЗ итераторов для запоминания предыдущего состояния !!!!!!!
    //static std::list<cMoves>* pMoveLst = nullptr;
    static cList<cMoves>* pMoveLst = nullptr;
    static cMoves* pMoves = nullptr;
    //std::list<std::list<cMoves>>::iterator sItPrev = sItMoves;  /// если static - иногда не обновляются статусы, если обычный - постоянная перерисовка
    std::list<cList<cMoves>>::iterator sItPrev = sItMoves;  /// если static - иногда не обновляются статусы, если обычный - постоянная перерисовка
    //static std::list<cMoves>::iterator itMoveTo;// обязательно static !
    static cMoves* pMoveTo = nullptr;// обязательно static !
    cMoves* pMovePrev; /// если static - иногда не обновляются статусы, если обычный - постоянная перерисовка
    bool left;
    bool right;
    bool enter;
    bool backspace; // не используется (отмена выбора)
    if(selChecker)
    {
        left  = KeyPress(VK_LEFT  );
        right = KeyPress(VK_RIGHT );
        enter = KeyPress(VK_RETURN);
        sItPrev = sItMoves;
        if(left)
        {
            if(++sItMoves == sAllMovesLst.end())
            {
                sItMoves = sAllMovesLst.begin();
            }
        }
        else if(right)
        {
            if(sItMoves == sAllMovesLst.begin())
            {
                sItMoves = sAllMovesLst.end();
            }
            --sItMoves;
        }
        else if(enter)
        {
            /// unmark all
            //for(std::list<cMoves>& sMvLst : sAllMovesLst)
            for(cList<cMoves>& sMvLst : sAllMovesLst)
            {
                //for(cMoves& sMv : sMvLst)
                for(cMoves* pMv = sMvLst.begin(); pMv != nullptr; pMv = sMvLst.next())
                {
                    pMv->SetFieldsMarkersByMoves(false, false, false, false, false, &_Board);
                }
            }
            cList<cMoves>& mvlst = (*sItMoves);
            //pMoveTo = mvlst.begin();
            for(cMoves* pMv = (*sItMoves).begin(); pMv != nullptr; pMv = (*sItMoves).next())    // выделение полей куда можно переместить шашку
            {
                pMv->SetFieldsMarkersByMoves(true, false, false, false, false, &_Board);
            }
            pMoveTo = mvlst.begin();
            selChecker = false;
            selMoveTo = true;
        }
        if(sItMoves != sItPrev)// если было перемещение
        {
            //for(cMoves& sMv : *sItPrev)
            for(cMoves* pMv = (*sItPrev).begin(); pMv != nullptr; pMv = (*sItPrev).next())
            {
                pMv->SetFieldsMarkersByMoves(false, true, false, false, false, &_Board); // убираем выделения прошлой шашки
            }
        }
        /// нужно для начального выделения первого элемента списка
        if(pMoveLst != &(*sItMoves)) // если текущая выбранная шашка НЕ равна прошлой
        {
            sItPrev = sItMoves; // запоминаем текущий тератор в прошлый
            //for(cMoves& sMv : *sItMoves)
            for(cMoves* pMv = (*sItMoves).begin(); pMv != nullptr; pMv = (*sItMoves).next())
            {
                pMv->SetFieldsMarkersByMoves(true, true, false, true, false, &_Board); // помечаем изменения
            }
            pMoveLst = &(*sItMoves); // запоминаем текущую шашку
        }
    }
    if(selMoveTo)
    {
        left  = KeyPress(VK_LEFT  );
        right = KeyPress(VK_RIGHT );
        enter = KeyPress(VK_RETURN);
        pMovePrev = pMoveTo;
        if(left)
        {
            //if(++itMoveTo == (*sItMoves).end())
            //{
            //    itMoveTo = (*sItMoves).begin();
            //}
            cList<cMoves>& mvlst = (*sItMoves);
            pMoveTo = mvlst.next(); /// problem here?
            if(pMoveTo == nullptr)
            {
                pMoveTo = (*sItMoves).begin();
            }
        }
        else if(right)
        {
            //if(itMoveTo == (*sItMoves).begin())
            //{
            //    itMoveTo = (*sItMoves).end();
            //}
            //--itMoveTo;
            pMoveTo = (*sItMoves).prev(); /// problem here?
            if(pMoveTo == nullptr)
            {
                pMoveTo = (*sItMoves).last();
            }
        }
        else if(enter)
        {
            //for(std::list<cMoves>& lst : sAllMovesLst)
            for(cList<cMoves>& lst : sAllMovesLst)
            {
                //for(cMoves& sMv : lst)
                for(cMoves* pMv = lst.begin(); pMv != nullptr; pMv = lst.next())
                {
                    pMv->ResetFieldsMarkers(&_Board);
                }
            }
            selChecker = true;
            selMoveTo = false;
            pMoveLst = nullptr;
            pMoves = nullptr;
            return pMoveTo;
        }
        if(pMovePrev != pMoveTo)
        {
            pMovePrev->SetFieldsMarkersByMoves(true, true, false, false, false, &_Board);
        }
        /// нужно для начального выделения первого элемента списка
        if(pMoves != pMoveTo)
        {
            //(*itMoveTo).SetFieldsMarkersByMoves(true, true, false, false, true, &_Board);
            //itMovePrev = itMoveTo;
            //pMoves = &(*itMoveTo);
            pMoveTo->SetFieldsMarkersByMoves(true, true, false, false, true, &_Board);
            pMovePrev = pMoveTo;
            pMoves = pMoveTo;
        }
    }
    return nullptr;
}
void cGame::ChangeSide(void)
{
    std::string side;
    if (_side == BLACKCHECK)
    {
        _side = WHITECHECK;
        side = "White ";
    }
    else
    {
        _side = BLACKCHECK;
        side = "Black ";
    }
    PrintAt(infoX, infoY, side + "turn");
    ///Print(std::to_string(_pMinMax->CalcEvalution()));
}
//bool cGame::FindAllPosibleMoves(CHECK side, std::list<cMoves>* pMoves)
bool cGame::FindAllPosibleMoves(CHECK side, cList<cMoves>* pMoves)
{
    pMoves->clear();
    FindAllJumpingMoves(side, pMoves);
    if(pMoves->empty())
    {
        FindAllSimpleMoves(side, pMoves);
        if(pMoves->empty())
        {
            return false;
        }
    }
    return true;
}
//void cGame::FindSimpleMoves(cField* const pFld, std::list<cMoves>* pMovesLst)
void cGame::FindSimpleMoves(cField* const pFld, cList<cMoves>* pMovesLst)
{
    if(!pFld || !pFld->GetChecker()) return;
    COORD dir;
    bool queen = pFld->GetCheckerIsQueen();
    bool black = pFld->GetCheckerIsBlack();
    SHORT fromY = queen ? -1 : (black ? -1 : 1); // отсечение лишних направлений, для
    SHORT toY = queen ? 2 : (black ? 0 : 2);     // обычной шашки
    //std::list<cField*> MovesToFields;
    cList<cField*> MovesToFields;
    for(dir.X = -1; dir.X < 2; dir.X += 2)  // двойной цикл по диагональным направлениям
    {
        for(dir.Y = fromY; dir.Y < toY; dir.Y += 2)// dir.X, dir.Y могут быть -1 или +1
        {
            _Board.ScanFreeFieldsOnDirection(pFld, dir, &MovesToFields);
        }
    }
    if (!MovesToFields.empty()) /// не сильно обязательная проверка (теоретически)
    {
        //for(cField* pFldTo : MovesToFields)
        for(cField** ppFldTo = MovesToFields.begin(); ppFldTo!= nullptr; ppFldTo = MovesToFields.next())
        {
            pMovesLst->emplace_back(pFld, *ppFldTo, _Board.CheckerReachOpositeSide(pFld, *ppFldTo), nullptr); // создание объекта внутри списка (без копирования)
            //pMovesLst->push_back(cMoves(pFld, *ppFldTo, _Board.CheckerReachOpositeSide(pFld, *ppFldTo), nullptr));
        }
    }
}
//void cGame::FindAllSimpleMoves(CHECK side, std::list<cMoves>* pMovesLst)
void cGame::FindAllSimpleMoves(CHECK side, cList<cMoves>* pMovesLst)
{
    if(!side) side = _side;
    //std::list<cField*>* pCheckLst = (side == WHITECHECK) ? _Board.GetWhiteListPtr() : _Board.GetBlackListPtr();   // выбор списка
    cList<cField*>* pCheckLst = (side == WHITECHECK) ? _Board.GetWhiteListPtr() : _Board.GetBlackListPtr();   // выбор списка
    for(cField** ppFld = pCheckLst->begin(); ppFld != nullptr; ppFld = pCheckLst->next())
    {
        FindSimpleMoves(*ppFld, pMovesLst);
    }
}
//void cGame::FindAllSimpleMoves(CHECK side, std::list<std::list<cMoves>>* pAllMovesLst)
void cGame::FindAllSimpleMoves(CHECK side, std::list<cList<cMoves>>* pAllMovesLst)
{
    if(!side) side = _side;
    //std::list<cField*>* pCheckLst = (side == WHITECHECK) ? _Board.GetWhiteListPtr() : _Board.GetBlackListPtr();   // выбор списка
    cList<cField*>* pCheckLst = (side == WHITECHECK) ? _Board.GetWhiteListPtr() : _Board.GetBlackListPtr();   // выбор списка
    pAllMovesLst->emplace_back();  // создаем хотя бы один пустой список
    for(cField** ppFld = pCheckLst->begin(); ppFld != nullptr; ppFld = pCheckLst->next())   // перебираем список шашек выбранного цвета
    {
        FindSimpleMoves(*ppFld, &(pAllMovesLst->back())); // ищем для каждой шашки(поля) список возможных простых перемещений
        if(!(pAllMovesLst->back().empty()))    // если этот список НЕ пустой
        {
            pAllMovesLst->back().front().SetFieldsMarkersByMoves(false, true, false, false, false, &_Board);
            ///cField* pTmp = pAllMovesLst->back().front().GetMoveFrom(); /// mark field here
            ///pTmp->SetCanBeSelected(true);   /// mark field // !!!! по логике - изменения полей sel/can/jump надо делать методом класса cMoves !!!!!???
            //_Changes.push_back(pTmp);       /// place to changes vector
            ///_Board.AddChange(pTmp);
            pAllMovesLst->emplace_back();  // создаём еще пустой список
        }
    }
    pAllMovesLst->pop_back();  // удаляем пустой список с последней итерации цикла
}
//size_t cGame::FindJumpingMoves(cField* const pFld, std::list<cMoves>* pMovesLst)
size_t cGame::FindJumpingMoves(cField* const pFld, cList<cMoves>* pMovesLst)
{
    static cMoves tmpMoves;   // статичный список для временного хранения списка прыжков
    size_t maxJumps = 0;
    COORD dir;
    //tmpMoves.emplace_back();
    /// НЕТ отсечения направлений для обычной шашки, т.к. она может "есть" во все стороны
    bool isJump = false;    // индикатор, что за время циклов найдётся хотя бы одно взятие(прыжок)
    for(dir.X = -1; dir.X < 2; dir.X += 2)  // двойной цикл по диагональным направлениям
    {
        for(dir.Y = -1; dir.Y < 2; dir.Y += 2)// dir.X, dir.Y могут быть -1 или +1
        {
            cField* pOposite = _Board.ScanOpositeCheckerOnDirection(pFld, dir); // ищем ближайшую шашку другого цвета
            if (pOposite)
            {
                //std::list<cField*> MovesToFields;
                cList<cField*> MovesToFields;
                MovesToFields.clear();
                _Board.ScanFreeFieldsOnDirection(pOposite, dir, pFld->GetCheckerIsQueen(), &MovesToFields); // ищем пустые поля за противоположной шашкой(куда можно прыгнуть)
                if(!MovesToFields.empty())  // если этот список НЕ пустой
                {
                    isJump = true;  // есть взятие
                    //for(cField* pFldTo : MovesToFields) // перебираем список
                    for(cField** ppFldTo = MovesToFields.begin(); ppFldTo != nullptr; ppFldTo = MovesToFields.next() )
                    {

                        size_t sz;
                        tmpMoves.AddAndMakeMove(pFld, *ppFldTo, false, pOposite);// делаем перемещение шашек на доске
                        if(tmpMoves.GetSize() > 10)
                        {
                            sz = tmpMoves.GetSize();
                        }
                        size_t jumps = FindJumpingMoves(*ppFldTo, pMovesLst); // ищем новый возможные прыжки(взятия) для поля, куда перепрыгнула шашка
                        if(jumps > maxJumps) maxJumps = jumps;
                        tmpMoves.RemoveAndUnMakeLastMove();   // откатываем ход
                    }
                }
            }
        }
    }
    if(!isJump)    // если в текущей позиции у шашки НЕТ ни одного взятия
    {
        if(!tmpMoves.isEmpty())   // и если список НЕ пустой (НЕТ выриантов движения со взятием)
        {
            tmpMoves.CheckLastMoveReachQueenLine(&_Board);
            pMovesLst->push_back(tmpMoves);    // то заносим наш временный список в конец списка списков. (копирование содержания)
            maxJumps = tmpMoves.GetSize();
        }
    }
    return maxJumps;
}
//void cGame::FindAllJumpingMoves(CHECK side, std::list<cMoves>* pMovesLst)
void cGame::FindAllJumpingMoves(CHECK side, cList<cMoves>* pMovesLst)
{
    //std::list<std::list<cMoves>> tmpLst;
    std::list<cList<cMoves>> tmpLst;
    FindAllJumpingMoves(side, &tmpLst, false);
    if(!tmpLst.empty())
    {
        //for(std::list<cMoves>& lst : tmpLst)
        for(cList<cMoves>& lst : tmpLst)
        {
            //pMovesLst->splice(pMovesLst->end(), lst); // переместить элементы из списка lst в список pMoveLst. список lst становится пустым
            pMovesLst->splice(lst); // переместить элементы из списка lst в список pMoveLst. список lst становится пустым
        }
    }
}
//void cGame::FindAllJumpingMoves(CHECK side, std::list<std::list<cMoves>>* pAllMovesLst, bool mark)
void cGame::FindAllJumpingMoves(CHECK side, std::list<cList<cMoves>>* pAllMovesLst, bool mark)
{
    if(!side) side = _side;
    //std::list<cField*>* pCheckLst = (side == WHITECHECK) ? _Board.GetWhiteListPtr() : _Board.GetBlackListPtr();   // выбор списка
    cList<cField*>* pCheckLst = (side == WHITECHECK) ? _Board.GetWhiteListPtr() : _Board.GetBlackListPtr();   // выбор списка
    size_t maxJumps = 0;
    pAllMovesLst->emplace_back();  // создаем хотя бы один пустой список цепочек
    for(cField** ppFld = pCheckLst->begin(); ppFld != nullptr; ppFld = pCheckLst->next())   // перебираем список шашек выбранного цвета
    {
        size_t jumps = FindJumpingMoves(*ppFld, &(pAllMovesLst->back())); // ищем для каждой шашки(поля) список возможных цепочек перемещений со взятием
        if(jumps > maxJumps) maxJumps = jumps; // запоминаем максимально длинную цепочку прыжков
        if(!(pAllMovesLst->back().empty()))    // если этот список НЕ пустой
        {
            if(mark)
            {
                cField* pTmp = pAllMovesLst->back().front().GetMoveFrom(); /// mark field here
                pTmp->SetCanBeSelected(true);   /// mark field !!!!!!!! remake to cMoves method !!!!!!!!!!!!
                _Board.AddChange(pTmp);       /// place to changes vector
            }
            pAllMovesLst->emplace_back();  // создаём еще пустой список
        }
    }
    pAllMovesLst->pop_back();  // удаляем пустой список с последней итерации цикла
    size_t sz = pAllMovesLst->size();
    if(sz > 1)
    {
        RemoveShortJumpingMoves(pAllMovesLst, maxJumps);  // удаляем ходы НЕ максимальной длинны
    }
    //RemoveShortJumpingMoves(pAllMovesLst, maxJumps);  // удаляем ходы НЕ максимальной длинны
}
//void cGame::RemoveShortJumpingMoves(std::list<std::list<cMoves>>* pAllMovesLst, size_t maxJumps)
void cGame::RemoveShortJumpingMoves(std::list<cList<cMoves>>* pAllMovesLst, size_t maxJumps)
{
    //size_t maxJumps = 0;
    if(!maxJumps) // если не указано максимальное число
    {
        //for(std::list<cMoves>& lst : *pAllMovesLst) //вычисляем максимальное кол-во взятий за ход из возможных
        for(cList<cMoves>& lst : *pAllMovesLst) //вычисляем максимальное кол-во взятий за ход из возможных
        {
            //for(cMoves& sMv : lst)
            for(cMoves* pMv = lst.begin(); pMv != nullptr; pMv = lst.next())
            {
                size_t sz = pMv->GetSize();
                if(pMv->GetSize() > maxJumps) maxJumps = pMv->GetSize();
            }

        }
    }
    //std::list<std::list<cMoves>>::iterator itListTo = pAllMovesLst->end();
    //std::list<std::list<cMoves>>::iterator itList = pAllMovesLst->begin();
    std::list<cList<cMoves>>::iterator itListTo = pAllMovesLst->end();
    std::list<cList<cMoves>>::iterator itList = pAllMovesLst->begin();
    //for(std::list<std::list<cMoves>>::iterator itList = pAllMovesLst->begin(); itList != itListTo; )  /// изначально было it != to
    while(itList != itListTo)
    {
        if(itList == pAllMovesLst->end()) break; ///
        cMoves* to = nullptr;
        cMoves* it;// = (*itList).begin();
        //for(std::list<cMoves>::iterator it = (*itList).begin(); it != to;)
        //while(it != to)
        for(it = (*itList).begin(); it != nullptr;)
        {
            if(it == reinterpret_cast<cMoves*>(0xfeeefeee))
            {
                int bug = 0;
            }
            size_t sz = it->GetSize(); /// !!!!!!!!!! problem may be here !!!!!!!!!!!!!!

            if(sz != maxJumps)// если размер списка прыжков НЕ равен максимальному возможному кол-во взятий
            {
                it = (*itList).erase(it);// удаляем его из списка списков и присваиваем итератору новое значение (вроде следующей позиции)
                //to = (*itList).end();
                if(it == reinterpret_cast<cMoves*>(0xfeeefeee))
                {
                    int bug = 0;
                }
            }
            else
            {
                //++it;// иначе просто передвигаем итератор на следующий элемент(список)
                cList<cMoves>& mvlst = (*itList);
                it = mvlst.next();
                if(it == reinterpret_cast<cMoves*>(0xfeeefeee))
                {
                    int bug = 0;
                }
            }
        }
        if((*itList).empty())// если получили пустой список
        {
            itList = pAllMovesLst->erase(itList);//удаляем его
            itListTo = pAllMovesLst->end();
        }
        else
        {
            ++itList;
        }
    }
}
//cMoves* cGame::SelectMinMax(std::list<std::list<cMoves>>& sAllMovesLst, cNTree* pNTree)
cMoves* cGame::SelectMinMax(std::list<cList<cMoves>>& sAllMovesLst, cNTree* pNTree)

{
    DWORD startTime = timeGetTime();
    _pMinMax->MinMax(pNTree, this, INT_MIN, INT_MAX, _side);
    //pNTree->PrintTree();
    //for(std::list<cMoves>& lst : sAllMovesLst)
    for(cList<cMoves>& lst : sAllMovesLst)
    {
        //for(cMoves& sMv : lst)
        for(cMoves* pMv = lst.begin(); pMv != nullptr; pMv = lst.next())
        {
            pMv->ResetFieldsMarkers(&_Board);
        }
    }
    DWORD endTime = timeGetTime();
    DOUBLE exexTime = (endTime - startTime)/1000.0;
    PrintAt(5,45, "Variants : " + std::to_string(pNTree->GetVariations()) + " - Time : " + std::to_string(exexTime));
    //PrintAt(5,46, "Construct : " + std::to_string(pNTree->_construct) + " - Destruct : " + std::to_string(pNTree->_destruct));
    return _pMinMax->GetBestMoves(pNTree);
}

