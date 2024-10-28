#include "MinMax.h"
#include "Misc.h"
#include <string>
/// ������� Evalution � ���������
const int CostPerChecker = 100;
const int CostPerQueen = 500;
const double CoefPerLine = 0.1;
const double CoefPerBorder = -0.1;
const int WinAdd = 5000; /// not used

cMinMax::cMinMax(cBoard* pBoard)
{
    _pBoard = pBoard;
    _maxPosY = FIELDS - 1;
}
cMinMax::~cMinMax()
{

}
int cMinMax::CalcEvalution()
{
    return CalcEvalutionForSide(false) - CalcEvalutionForSide(true); // ����� ������ ������ � ����� ������
}
int cMinMax::CalcEvalutionForSide(bool isBlack)
{   /// ������ ������ (��������� �������) ��� ����� �� ������
    int ret = 0;
    cList<cField*>* pCheckers = isBlack ? _pBoard->GetBlackListPtr() : _pBoard->GetWhiteListPtr();// ���������� ������ ��� ������ ��� ����� �����
    int maxLine = isBlack ? _maxPosY : 0;
    int dirLine = isBlack ? -1 : 1;
    for(cField** ppFld = pCheckers->begin(); ppFld != nullptr; ppFld = pCheckers->next())
    {
        int tmp = (*ppFld)->GetCheckerIsQueen() ? CostPerQueen : CostPerChecker; // ��������� ��������� �����/�����
        if(tmp == CostPerChecker) // ���� ����� �������, ��������� ������ ��������
        {
            const COORD& pos = (*ppFld)->GetPosition();
            tmp *= 1.0 + (dirLine * pos.Y + maxLine) * CoefPerLine; // ��������� ��� ������� �� ���������
            if (pos.X == 0 || pos.X == _maxPosY || pos.Y == 0 || pos.Y == _maxPosY) // �������� �� ��������� "� ������"
            {
                tmp *= 1.0 + CoefPerBorder; // ���������
            }
        }
        ret += tmp;
    }
    return ret;
}

int cMinMax::MinMax(cNTree* pNTree, cGame* pGame, int alfa, int beta, CHECK side)
{
    if (pNTree->ReachMaxDeep() || pNTree->isLoose()) return pNTree->GetNodeValue();/// ���� �������� ���������� �������, ��� ��������� �� ���������� �������� ����
    if(side == NO) side = pGame->GetSide(); // ���� �� ������� �������, �� ����� �������� �� ����
    bool black = (side == BLACKCHECK); //
    side = black ? WHITECHECK : BLACKCHECK; // ����������� ������� ��� ����������� ����������� ������� MinMax
    int ret = black ? INT_MAX : INT_MIN; // ������� � ������������ ���������� �����������/���������� ��������� �����
    //std::list<cMoves>* pMoves = pNTree->GetNodeMovesListPtr(); // ��������� ������ ��������� ����������� ��������� ����
    cList<cMoves>* pMoves = pNTree->GetNodeMovesListPtr(); // ��������� ������ ��������� ����������� ��������� ����
    //for (std::list<cMoves>::iterator it = pMoves->begin(); it != pMoves->end(); ++it) // ���������� ������ �����������
    for(cMoves* pMv = pMoves->begin(); pMv != nullptr; pMv = pMoves->next())
    {
        //cMoves* pMv = &(*it); //�������� ��������� �� ������� ������
        pMv->MakeAllMove(pGame->GetBoardPtr()); // ������ ����
        if(!(pNTree->AddNodeAndCurrentUp(pMv, CalcEvalution()))) break; // ��������� ���� � ��������� �� ���� ����, ���� �� �������� ����, ��������� ����
        int eval;
        pGame->FindAllPosibleMoves(side, pNTree->GetNodeMovesListPtr()); // ��������� ������ ��������� ����� � ��������� ����
        eval = MinMax(pNTree, pGame, alfa, beta, side);// ����������� ����� ��� ������ ����
        ret = black ? min(ret, eval) : max(ret, eval); // ������������ ��� ������������� ������������ �������� �� �����
        pMv->MakeAllUnMove(false, pGame->GetBoardPtr()); // ���������� ���� �����
        pNTree->CurrentDown();  //���������� �� ������� ����
        /// �����-���� ��������
        if(black)   beta = min(beta, ret); // ��������� beta
        else        alfa = max(alfa, ret); // ��������� alfa
        //PrintE(std::to_string(pNTree->GetNodeDeep())+":"+std::to_string(ret)+" ", BLUE);
        //PrintE(std::to_string(alfa)+":"+std::to_string(beta)+" ", BLUE);
        if(beta < alfa)
        {
            pruned++; /// ������� ���������� ������
            //PrintE(std::to_string(pNTree->GetNodeDeep())+":"+std::to_string(ret)+" ", BLUE);
            //PrintE(std::to_string(alfa)+":"+std::to_string(beta)+" ", BLUE);
            break;
        }
    }
    //pNTree->NodePrint();
    return pNTree->SetNodeValue(ret);// ���������� � ����������
}
cMoves* cMinMax::GetBestMoves(cNTree* pNTree)
{
    if(!pNTree)
    {
        return nullptr;
    }
    //std::list<cMoves*> bestMoves;
    cList<cMoves*> bestMoves;
    pNTree->FillBestMovesList(&bestMoves);
    if(bestMoves.empty())
    {
        return nullptr;
    }
    srand(timeGetTime());
    int rnd = rand() % bestMoves.size();
    cMoves** ppRnd = bestMoves.begin();
    while (rnd--)
    {
        ppRnd = bestMoves.next();
        //--rnd;
    }
    PrintE(std::to_string(pruned), BLUE);
    return *ppRnd;
}
