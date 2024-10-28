#include "Game.h"
#include <string>
#include "Misc.h"
#include "MinMax.h"
const SHORT infoX = 15;
const SHORT infoY = 35;
const int maxDeep = 4;/// ������������ ������� �������� ����� ��������� MinMax !!!! �� ������������� ������� ������ 10 !!!!
cMinMax* _pMinMax = nullptr;
cGame::cGame(SHORT fields, cInputPort* const pIp)
{
    if (pIp) _pInPort = pIp;
    _side = NO;
    if (fields < 4) fields = 4; // ����������� �� ������� ����
    if (fields > 12) fields = 12;
    if (fields % 2) fields = (fields / 2) * 2;  // ���������� � ������� ����� ������
    _fields = fields;
    _checkers = ((_fields - 2) / 2) * (_fields / 2);    // ����������� ���-�� ����� ������ �����
    // ���������� ������ �����
    bool black = false;  // ������ ������ ������
    for(SHORT x = 0; x < _fields; ++x)
    {
        for(SHORT y = 0; y < _fields; ++y)
        {
            cField* pFld = _Board.GetFieldPtr(x, y);
            pFld->SetPosition(x,y);
            pFld->SetBlack(black);
            black = !black;
            _Board.AddChange(pFld);  /// ��� ����� ��������� ���������� ���������� �������, � ������� ��������� ���������
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
        /// ������ ��� �������� �� ����� ���� / �����
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
        return true; /// ����� ����, �.�. ��� ��������� ����� (��� ����� ����� ��� ������������� �����������)
    }
    if(CheckDraw())
    {
        std::string win = "Pease WIN !"; /// ����� - "�������� ������ !"
        PrintAt(infoX, infoY, win);
        return true;
    }
    return false;
}
bool cGame::CheckDraw(void)
{
    CHECK side = WHITECHECK;
    {
        if(OnlyQueenOnSide(side) || OnlyQueenOnSide(side = BLACKCHECK)) // ���� � ����� �� ������ �������� ������������ ����� � ��� �����
        {
            side = (side == WHITECHECK) ? BLACKCHECK : WHITECHECK; // ������������� �� ������ �������
            if(CheckersInDrawOnSide(side))  // ���� � ������ ������� ������ ����� "�����"
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
    if(sz > 0 && sz < 4)    // ���� ����� �� 1 �� 3
    {
        //for(cField* pFld : *pChkLst)
        for(cField** ppFld = pChkLst->begin(); ppFld != nullptr; ppFld = pChkLst->next())
        {
            if((*ppFld)->GetCheckerIsQueen()) // � ���� �� ���� �� ��� �����
                return true;    // ��������� ������� "�����" ��� ������ �������
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
    static bool selChecker = true;  // ���������� ������ �����, ������� ������
    static bool selMoveTo = false;  // ��������� ������ ����� ���� ������ ��������� ������
    /// !!!!!!! ���������� ��� ���������� ��� ����������� ����������� ��������� !!!!!!!
    //static std::list<cMoves>* pMoveLst = nullptr;
    static cList<cMoves>* pMoveLst = nullptr;
    static cMoves* pMoves = nullptr;
    //std::list<std::list<cMoves>>::iterator sItPrev = sItMoves;  /// ���� static - ������ �� ����������� �������, ���� ������� - ���������� �����������
    std::list<cList<cMoves>>::iterator sItPrev = sItMoves;  /// ���� static - ������ �� ����������� �������, ���� ������� - ���������� �����������
    //static std::list<cMoves>::iterator itMoveTo;// ����������� static !
    static cMoves* pMoveTo = nullptr;// ����������� static !
    cMoves* pMovePrev; /// ���� static - ������ �� ����������� �������, ���� ������� - ���������� �����������
    bool left;
    bool right;
    bool enter;
    bool backspace; // �� ������������ (������ ������)
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
            for(cMoves* pMv = (*sItMoves).begin(); pMv != nullptr; pMv = (*sItMoves).next())    // ��������� ����� ���� ����� ����������� �����
            {
                pMv->SetFieldsMarkersByMoves(true, false, false, false, false, &_Board);
            }
            pMoveTo = mvlst.begin();
            selChecker = false;
            selMoveTo = true;
        }
        if(sItMoves != sItPrev)// ���� ���� �����������
        {
            //for(cMoves& sMv : *sItPrev)
            for(cMoves* pMv = (*sItPrev).begin(); pMv != nullptr; pMv = (*sItPrev).next())
            {
                pMv->SetFieldsMarkersByMoves(false, true, false, false, false, &_Board); // ������� ��������� ������� �����
            }
        }
        /// ����� ��� ���������� ��������� ������� �������� ������
        if(pMoveLst != &(*sItMoves)) // ���� ������� ��������� ����� �� ����� �������
        {
            sItPrev = sItMoves; // ���������� ������� ������� � �������
            //for(cMoves& sMv : *sItMoves)
            for(cMoves* pMv = (*sItMoves).begin(); pMv != nullptr; pMv = (*sItMoves).next())
            {
                pMv->SetFieldsMarkersByMoves(true, true, false, true, false, &_Board); // �������� ���������
            }
            pMoveLst = &(*sItMoves); // ���������� ������� �����
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
        /// ����� ��� ���������� ��������� ������� �������� ������
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
    SHORT fromY = queen ? -1 : (black ? -1 : 1); // ��������� ������ �����������, ���
    SHORT toY = queen ? 2 : (black ? 0 : 2);     // ������� �����
    //std::list<cField*> MovesToFields;
    cList<cField*> MovesToFields;
    for(dir.X = -1; dir.X < 2; dir.X += 2)  // ������� ���� �� ������������ ������������
    {
        for(dir.Y = fromY; dir.Y < toY; dir.Y += 2)// dir.X, dir.Y ����� ���� -1 ��� +1
        {
            _Board.ScanFreeFieldsOnDirection(pFld, dir, &MovesToFields);
        }
    }
    if (!MovesToFields.empty()) /// �� ������ ������������ �������� (������������)
    {
        //for(cField* pFldTo : MovesToFields)
        for(cField** ppFldTo = MovesToFields.begin(); ppFldTo!= nullptr; ppFldTo = MovesToFields.next())
        {
            pMovesLst->emplace_back(pFld, *ppFldTo, _Board.CheckerReachOpositeSide(pFld, *ppFldTo), nullptr); // �������� ������� ������ ������ (��� �����������)
            //pMovesLst->push_back(cMoves(pFld, *ppFldTo, _Board.CheckerReachOpositeSide(pFld, *ppFldTo), nullptr));
        }
    }
}
//void cGame::FindAllSimpleMoves(CHECK side, std::list<cMoves>* pMovesLst)
void cGame::FindAllSimpleMoves(CHECK side, cList<cMoves>* pMovesLst)
{
    if(!side) side = _side;
    //std::list<cField*>* pCheckLst = (side == WHITECHECK) ? _Board.GetWhiteListPtr() : _Board.GetBlackListPtr();   // ����� ������
    cList<cField*>* pCheckLst = (side == WHITECHECK) ? _Board.GetWhiteListPtr() : _Board.GetBlackListPtr();   // ����� ������
    for(cField** ppFld = pCheckLst->begin(); ppFld != nullptr; ppFld = pCheckLst->next())
    {
        FindSimpleMoves(*ppFld, pMovesLst);
    }
}
//void cGame::FindAllSimpleMoves(CHECK side, std::list<std::list<cMoves>>* pAllMovesLst)
void cGame::FindAllSimpleMoves(CHECK side, std::list<cList<cMoves>>* pAllMovesLst)
{
    if(!side) side = _side;
    //std::list<cField*>* pCheckLst = (side == WHITECHECK) ? _Board.GetWhiteListPtr() : _Board.GetBlackListPtr();   // ����� ������
    cList<cField*>* pCheckLst = (side == WHITECHECK) ? _Board.GetWhiteListPtr() : _Board.GetBlackListPtr();   // ����� ������
    pAllMovesLst->emplace_back();  // ������� ���� �� ���� ������ ������
    for(cField** ppFld = pCheckLst->begin(); ppFld != nullptr; ppFld = pCheckLst->next())   // ���������� ������ ����� ���������� �����
    {
        FindSimpleMoves(*ppFld, &(pAllMovesLst->back())); // ���� ��� ������ �����(����) ������ ��������� ������� �����������
        if(!(pAllMovesLst->back().empty()))    // ���� ���� ������ �� ������
        {
            pAllMovesLst->back().front().SetFieldsMarkersByMoves(false, true, false, false, false, &_Board);
            ///cField* pTmp = pAllMovesLst->back().front().GetMoveFrom(); /// mark field here
            ///pTmp->SetCanBeSelected(true);   /// mark field // !!!! �� ������ - ��������� ����� sel/can/jump ���� ������ ������� ������ cMoves !!!!!???
            //_Changes.push_back(pTmp);       /// place to changes vector
            ///_Board.AddChange(pTmp);
            pAllMovesLst->emplace_back();  // ������ ��� ������ ������
        }
    }
    pAllMovesLst->pop_back();  // ������� ������ ������ � ��������� �������� �����
}
//size_t cGame::FindJumpingMoves(cField* const pFld, std::list<cMoves>* pMovesLst)
size_t cGame::FindJumpingMoves(cField* const pFld, cList<cMoves>* pMovesLst)
{
    static cMoves tmpMoves;   // ��������� ������ ��� ���������� �������� ������ �������
    size_t maxJumps = 0;
    COORD dir;
    //tmpMoves.emplace_back();
    /// ��� ��������� ����������� ��� ������� �����, �.�. ��� ����� "����" �� ��� �������
    bool isJump = false;    // ���������, ��� �� ����� ������ ������� ���� �� ���� ������(������)
    for(dir.X = -1; dir.X < 2; dir.X += 2)  // ������� ���� �� ������������ ������������
    {
        for(dir.Y = -1; dir.Y < 2; dir.Y += 2)// dir.X, dir.Y ����� ���� -1 ��� +1
        {
            cField* pOposite = _Board.ScanOpositeCheckerOnDirection(pFld, dir); // ���� ��������� ����� ������� �����
            if (pOposite)
            {
                //std::list<cField*> MovesToFields;
                cList<cField*> MovesToFields;
                MovesToFields.clear();
                _Board.ScanFreeFieldsOnDirection(pOposite, dir, pFld->GetCheckerIsQueen(), &MovesToFields); // ���� ������ ���� �� ��������������� ������(���� ����� ��������)
                if(!MovesToFields.empty())  // ���� ���� ������ �� ������
                {
                    isJump = true;  // ���� ������
                    //for(cField* pFldTo : MovesToFields) // ���������� ������
                    for(cField** ppFldTo = MovesToFields.begin(); ppFldTo != nullptr; ppFldTo = MovesToFields.next() )
                    {

                        size_t sz;
                        tmpMoves.AddAndMakeMove(pFld, *ppFldTo, false, pOposite);// ������ ����������� ����� �� �����
                        if(tmpMoves.GetSize() > 10)
                        {
                            sz = tmpMoves.GetSize();
                        }
                        size_t jumps = FindJumpingMoves(*ppFldTo, pMovesLst); // ���� ����� ��������� ������(������) ��� ����, ���� ������������ �����
                        if(jumps > maxJumps) maxJumps = jumps;
                        tmpMoves.RemoveAndUnMakeLastMove();   // ���������� ���
                    }
                }
            }
        }
    }
    if(!isJump)    // ���� � ������� ������� � ����� ��� �� ������ ������
    {
        if(!tmpMoves.isEmpty())   // � ���� ������ �� ������ (��� ��������� �������� �� �������)
        {
            tmpMoves.CheckLastMoveReachQueenLine(&_Board);
            pMovesLst->push_back(tmpMoves);    // �� ������� ��� ��������� ������ � ����� ������ �������. (����������� ����������)
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
            //pMovesLst->splice(pMovesLst->end(), lst); // ����������� �������� �� ������ lst � ������ pMoveLst. ������ lst ���������� ������
            pMovesLst->splice(lst); // ����������� �������� �� ������ lst � ������ pMoveLst. ������ lst ���������� ������
        }
    }
}
//void cGame::FindAllJumpingMoves(CHECK side, std::list<std::list<cMoves>>* pAllMovesLst, bool mark)
void cGame::FindAllJumpingMoves(CHECK side, std::list<cList<cMoves>>* pAllMovesLst, bool mark)
{
    if(!side) side = _side;
    //std::list<cField*>* pCheckLst = (side == WHITECHECK) ? _Board.GetWhiteListPtr() : _Board.GetBlackListPtr();   // ����� ������
    cList<cField*>* pCheckLst = (side == WHITECHECK) ? _Board.GetWhiteListPtr() : _Board.GetBlackListPtr();   // ����� ������
    size_t maxJumps = 0;
    pAllMovesLst->emplace_back();  // ������� ���� �� ���� ������ ������ �������
    for(cField** ppFld = pCheckLst->begin(); ppFld != nullptr; ppFld = pCheckLst->next())   // ���������� ������ ����� ���������� �����
    {
        size_t jumps = FindJumpingMoves(*ppFld, &(pAllMovesLst->back())); // ���� ��� ������ �����(����) ������ ��������� ������� ����������� �� �������
        if(jumps > maxJumps) maxJumps = jumps; // ���������� ����������� ������� ������� �������
        if(!(pAllMovesLst->back().empty()))    // ���� ���� ������ �� ������
        {
            if(mark)
            {
                cField* pTmp = pAllMovesLst->back().front().GetMoveFrom(); /// mark field here
                pTmp->SetCanBeSelected(true);   /// mark field !!!!!!!! remake to cMoves method !!!!!!!!!!!!
                _Board.AddChange(pTmp);       /// place to changes vector
            }
            pAllMovesLst->emplace_back();  // ������ ��� ������ ������
        }
    }
    pAllMovesLst->pop_back();  // ������� ������ ������ � ��������� �������� �����
    size_t sz = pAllMovesLst->size();
    if(sz > 1)
    {
        RemoveShortJumpingMoves(pAllMovesLst, maxJumps);  // ������� ���� �� ������������ ������
    }
    //RemoveShortJumpingMoves(pAllMovesLst, maxJumps);  // ������� ���� �� ������������ ������
}
//void cGame::RemoveShortJumpingMoves(std::list<std::list<cMoves>>* pAllMovesLst, size_t maxJumps)
void cGame::RemoveShortJumpingMoves(std::list<cList<cMoves>>* pAllMovesLst, size_t maxJumps)
{
    //size_t maxJumps = 0;
    if(!maxJumps) // ���� �� ������� ������������ �����
    {
        //for(std::list<cMoves>& lst : *pAllMovesLst) //��������� ������������ ���-�� ������ �� ��� �� ���������
        for(cList<cMoves>& lst : *pAllMovesLst) //��������� ������������ ���-�� ������ �� ��� �� ���������
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
    //for(std::list<std::list<cMoves>>::iterator itList = pAllMovesLst->begin(); itList != itListTo; )  /// ���������� ���� it != to
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

            if(sz != maxJumps)// ���� ������ ������ ������� �� ����� ������������� ���������� ���-�� ������
            {
                it = (*itList).erase(it);// ������� ��� �� ������ ������� � ����������� ��������� ����� �������� (����� ��������� �������)
                //to = (*itList).end();
                if(it == reinterpret_cast<cMoves*>(0xfeeefeee))
                {
                    int bug = 0;
                }
            }
            else
            {
                //++it;// ����� ������ ����������� �������� �� ��������� �������(������)
                cList<cMoves>& mvlst = (*itList);
                it = mvlst.next();
                if(it == reinterpret_cast<cMoves*>(0xfeeefeee))
                {
                    int bug = 0;
                }
            }
        }
        if((*itList).empty())// ���� �������� ������ ������
        {
            itList = pAllMovesLst->erase(itList);//������� ���
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

