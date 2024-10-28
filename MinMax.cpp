#include "MinMax.h"
#include "Misc.h"
#include <string>
/// формула Evalution и константы
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
    return CalcEvalutionForSide(false) - CalcEvalutionForSide(true); // сумма оценок черной и белой сторон
}
int cMinMax::CalcEvalutionForSide(bool isBlack)
{   /// расчёт оценки (оценочная функция) для одной из сторон
    int ret = 0;
    cList<cField*>* pCheckers = isBlack ? _pBoard->GetBlackListPtr() : _pBoard->GetWhiteListPtr();// используем список или чёрных или белых шашек
    int maxLine = isBlack ? _maxPosY : 0;
    int dirLine = isBlack ? -1 : 1;
    for(cField** ppFld = pCheckers->begin(); ppFld != nullptr; ppFld = pCheckers->next())
    {
        int tmp = (*ppFld)->GetCheckerIsQueen() ? CostPerQueen : CostPerChecker; // начальная стоимость шашки/дамки
        if(tmp == CostPerChecker) // если шашка обычная, добавляем всякие поправки
        {
            const COORD& pos = (*ppFld)->GetPosition();
            tmp *= 1.0 + (dirLine * pos.Y + maxLine) * CoefPerLine; // множитель для позиции по вертикали
            if (pos.X == 0 || pos.X == _maxPosY || pos.Y == 0 || pos.Y == _maxPosY) // проверка на положения "у стенки"
            {
                tmp *= 1.0 + CoefPerBorder; // множитель
            }
        }
        ret += tmp;
    }
    return ret;
}

int cMinMax::MinMax(cNTree* pNTree, cGame* pGame, int alfa, int beta, CHECK side)
{
    if (pNTree->ReachMaxDeep() || pNTree->isLoose()) return pNTree->GetNodeValue();/// если достигли предельной глубины, или проиграли то возвращаем значение узла
    if(side == NO) side = pGame->GetSide(); // если не указана сторона, то берем текушуюю из игры
    bool black = (side == BLACKCHECK); //
    side = black ? WHITECHECK : BLACKCHECK; // переключаем сторону для последующих рекурсивных вызовов MinMax
    int ret = black ? INT_MAX : INT_MIN; // заносим в возвращаемую переменную максимально/минимально возможное число
    //std::list<cMoves>* pMoves = pNTree->GetNodeMovesListPtr(); // указатель списка возможных перемещений текущкего узла
    cList<cMoves>* pMoves = pNTree->GetNodeMovesListPtr(); // указатель списка возможных перемещений текущкего узла
    //for (std::list<cMoves>::iterator it = pMoves->begin(); it != pMoves->end(); ++it) // перебираем список перемещений
    for(cMoves* pMv = pMoves->begin(); pMv != nullptr; pMv = pMoves->next())
    {
        //cMoves* pMv = &(*it); //получаем указатель на элемент списка
        pMv->MakeAllMove(pGame->GetBoardPtr()); // далаем ходы
        if(!(pNTree->AddNodeAndCurrentUp(pMv, CalcEvalution()))) break; // добавляем узел и переходим на этот узел, если не создался узел, прерываем цикл
        int eval;
        pGame->FindAllPosibleMoves(side, pNTree->GetNodeMovesListPtr()); // заполняем список возможных ходов в созданном узле
        eval = MinMax(pNTree, pGame, alfa, beta, side);// рекурсивный вызов для нового узла
        ret = black ? min(ret, eval) : max(ret, eval); // минимизируем или максимизируем возвращаемое значение по цвету
        pMv->MakeAllUnMove(false, pGame->GetBoardPtr()); // откатываем ходы назад
        pNTree->CurrentDown();  //спускаемся на уровень ниже
        /// альфа-бета отсечние
        if(black)   beta = min(beta, ret); // обновляем beta
        else        alfa = max(alfa, ret); // обновляем alfa
        //PrintE(std::to_string(pNTree->GetNodeDeep())+":"+std::to_string(ret)+" ", BLUE);
        //PrintE(std::to_string(alfa)+":"+std::to_string(beta)+" ", BLUE);
        if(beta < alfa)
        {
            pruned++; /// подсчёт отсеченных ветвей
            //PrintE(std::to_string(pNTree->GetNodeDeep())+":"+std::to_string(ret)+" ", BLUE);
            //PrintE(std::to_string(alfa)+":"+std::to_string(beta)+" ", BLUE);
            break;
        }
    }
    //pNTree->NodePrint();
    return pNTree->SetNodeValue(ret);// запоминаем и возвращаем
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
