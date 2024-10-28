#ifndef MINMAX_H_INCLUDED
#define MINMAX_H_INCLUDED
//#include "Field.h"
//#include "Board.h"
//#include "NTree.h"
#include <list>
#include "List.h"
#include "Game.h"
//class cGame;
class cMinMax
{
public:
    cMinMax(cBoard* pBoard);
    ~cMinMax();
    int CalcEvalution(void);
    int MinMax(cNTree* pNTree, cGame* pGame, int alfa, int beta, CHECK side);
    cMoves* GetBestMoves(cNTree* pNTree);
    int pruned = 0;
private:
    cBoard* _pBoard;
    //std::list<cMoves*> _pBestMoves;
    cList<cMoves*> _pBestMoves;
    int _maxPosY;
    int CalcEvalutionForSide(bool isBlack);
    //int MinMax(cNTree* pNTree, cGame* pGame, CHECK side);
};

#endif // MINMAX_H_INCLUDED
