#include "NTree.h"
#include "Misc.h"
#include <string>
const size_t MINDEEP = 2;
const size_t MAXDEEP = 10;
//
cNode::cNode(int val, cMoves* pMoves)
{
    value = val;
    this->pMoves = pMoves;
}
cNode::~cNode()
{
    //nextMovesLst.clear();
    //nextNodesLst.clear();
}
cNTree::cNTree()
{

}
//cNTree::cNTree(std::list<std::list<cMoves>>* pLstLstMoves, int val)
cNTree::cNTree(std::list<cList<cMoves>>* pLstLstMoves, int val)
{
    _pNodes = new cNode(val, nullptr);
    _construct++;
    _pCurrent = _pNodes;
    //for(std::list<cMoves>& lst : *pLstLstMoves)
    for(cList<cMoves>& lst : *pLstLstMoves)
    {
        //for(cMoves& sMv : lst)
        for(cMoves* pMv = lst.begin(); pMv != nullptr; pMv = lst.next())
        {
            _pNodes->nextMovesLst.push_back(*pMv); // нельзя перемещать (использовать splice), т.к. нельзя изменять входной список списков ????
        }
    }
}
cNTree::~cNTree()
{
    Clear();
    //PrintAt(5,46, "Construct : " + std::to_string(_construct) + " - Destruct : " + std::to_string(_destruct));
}
void cNTree::Clear()
{
    if(_pNodes) Delete(_pNodes);
}
cNode* cNTree::AddNode(cMoves* pMoves, int val, cNode* pNode)
{
    if(!pNode) pNode = _pCurrent;
    if(pNode->deep == _maxDeep) return nullptr;
    cNode* pNewNode = new(std::nothrow) cNode(val, pMoves);   // оператор new вернет nullptr если не сможет создать новый объект
    if(pNewNode)
    {
        (pNewNode->deep = pNode->deep)++;
        _variation++;
        _construct++;
        //PrintE(std::to_string(pNewNode->deep)+":"+std::to_string(pNewNode->value)+" "+std::to_string(pMoves->GetMoveTo()->GetPosition().X)
         //      +"-"+std::to_string(pMoves->GetMoveTo()->GetPosition().Y)+"|");
        pNewNode->pPrev = pNode;
        pNode->nextNodesLst.push_back(pNewNode);
    }
    return pNewNode;
}
cNode* cNTree::AddNode(cMoves* pMoves, cNode* pNode)
{
    return AddNode(pMoves, 0, pNode);
}
cNode* cNTree::AddNodeAndCurrentUp(cMoves* pMoves, int val)
{
    cNode* pNewNode = AddNode(pMoves, val, _pCurrent);
    if(!pNewNode) return nullptr;
    return _pCurrent = pNewNode;
}
bool cNTree::AddAllNodesFromInnerMovesList(cNode* pNode)
{
    if(!pNode) pNode = _pCurrent;
    if(pNode->nextMovesLst.empty()) return false;
    //for(cMoves* pMv : pNode->nextMovesLst)
    //for(std::list<cMoves>::iterator it = pNode->nextMovesLst.begin(); it != pNode->nextMovesLst.end(); ++it)
    for(cMoves* it = pNode->nextMovesLst.begin(); it != nullptr; it = pNode->nextMovesLst.next())
    {
        //if(!AddNode(&(*it), pNode)) return false;
        if(!AddNode(it, pNode)) return false;
    }
    return true;
}
cNode* cNTree::CurrentUp(cMoves* pMoves)
{
    if(_pCurrent->nextMovesLst.empty()) return nullptr;
    //for(auto pNode : _pCurrent->nextNodesLst) ///
    for(cNode** ppNode = _pCurrent->nextNodesLst.begin(); ppNode != nullptr; ppNode = _pCurrent->nextNodesLst.next())
    {
        if((*ppNode)->pMoves == pMoves)
        {
            _pCurrent = *ppNode;
            return *ppNode;
        }
    }
    return nullptr;
}
cNode* cNTree::CurrentDown(cMoves* pMoves)
{
    if(_pCurrent->pPrev)
    {
        if(pMoves)
        {
            if(pMoves != _pCurrent->pPrev->pMoves)
            {
                return nullptr;
            }
        }
        _pCurrent = _pCurrent->pPrev;
    }
    return _pCurrent;
}
cNode* cNTree::GetCurrentNodePtr(void)
{
    if(!_pCurrent) _pCurrent = _pNodes;
    return _pCurrent;
}
size_t cNTree::GetVariations(void)
{
    return _variation;
}
size_t cNTree::GetMaxDeep(void)
{
    return _maxDeep;
}
void cNTree::SetMaxDeep(size_t mxDp)
{
    _maxDeep = mxDp;
    if(_maxDeep <  MINDEEP) _maxDeep =  MINDEEP;/// избавиться от магических чисел
    if(_maxDeep > MAXDEEP) _maxDeep = MAXDEEP;/// избавиться от магических чисел
}
bool cNTree::ReachMaxDeep(void)
{
    if(!_pCurrent) return false;
    return _maxDeep == _pCurrent->deep;
}
size_t cNTree::GetNodeDeep(cNode* pNode)
{
    if(!pNode) pNode = _pCurrent;
    return pNode->deep;
}
int cNTree::GetNodeValue(cNode* pNode)
{
    if(!pNode) pNode = _pCurrent;
    //PrintE(std::to_string(pNode->deep)+":"+std::to_string(pNode->value)+" "+std::to_string(pNode->pMoves->GetMoveTo()->GetPosition().X)
               //+"-"+std::to_string(pNode->pMoves->GetMoveTo()->GetPosition().Y)+"|", GREEN);
    return pNode->value;
}
int cNTree::SetNodeValue(int val, cNode* pNode)
{
    if(!pNode) pNode = _pCurrent;
    return pNode->value = val;
}
bool cNTree::isLoose(void)
{
    return _pCurrent->nextMovesLst.empty();
}
//std::list<cMoves>* cNTree::GetNodeMovesListPtr(cNode* pNode)
cList<cMoves>* cNTree::GetNodeMovesListPtr(cNode* pNode)
{
    if(!pNode) pNode = _pCurrent;
    return &(pNode->nextMovesLst);
}
//void cNTree::FillBestMovesList(std::list<cMoves*>* pMovesLst)
void cNTree::FillBestMovesList(cList<cMoves*>* pMovesLst)
{
    if(_pNodes)
    {
        //for(auto pNode : _pNodes->nextNodesLst)
        for(cNode** ppNode = _pNodes->nextNodesLst.begin(); ppNode != nullptr; ppNode = _pNodes->nextNodesLst.next())
        {
            if((*ppNode)->value == _pNodes->value)
            {
                PrintE(std::to_string((*ppNode)->deep)+":"+std::to_string((*ppNode)->value)+" ", RED);
                pMovesLst->push_back((*ppNode)->pMoves);
            }
        }
    }
}
void cNTree::Delete(cNode* pNode)
{
    if(pNode && !(pNode->nextNodesLst.empty()))
    {
        //for(auto pNodeNext : pNode->nextNodesLst)
        for(cNode** ppNodeNext = pNode->nextNodesLst.begin(); ppNodeNext != nullptr; ppNodeNext = pNode->nextNodesLst.next())
        {
            Delete(*(ppNodeNext));
        }
    }
    _destruct++;
    delete pNode;
}
void cNTree::PrintTree(void)
{
    PrintNode(_pNodes);
}
void cNTree::PrintNode(cNode* pNode)
{
    if(pNode && !(pNode->nextNodesLst.empty()))
    {
        //for(auto pNodeNext : pNode->nextNodesLst)
        for(cNode** ppNodeNext = pNode->nextNodesLst.begin(); ppNodeNext != nullptr; ppNodeNext = pNode->nextNodesLst.next())
        {
            PrintNode(*ppNodeNext);
        }
    }
    PrintE(std::to_string(pNode->deep)+":"+std::to_string(pNode->value)+" ", GREEN);
    if(!pNode->pMoves) PrintE("pMoves", RED);
    else
    {
        if(!pNode->pMoves->GetMoveFrom()) PrintE("MoveFrom", RED);
        else
        {
            PrintE(std::to_string(pNode->pMoves->GetMoveFrom()->GetPosition().X)+"-"
                +std::to_string(pNode->pMoves->GetMoveFrom()->GetPosition().Y)+"=", GREEN);
        }
        if(!pNode->pMoves->GetMoveTo()) PrintE("MoveTo", RED);
        else
        {
            PrintE(std::to_string(pNode->pMoves->GetMoveTo()->GetPosition().X)+"-"
                +std::to_string(pNode->pMoves->GetMoveTo()->GetPosition().Y)+"|", GREEN);
        }
    }
}
void cNTree::NodePrint(cNode* pNode)
{
    if(!pNode) pNode = _pCurrent;
    PrintE(std::to_string(pNode->deep)+":"+std::to_string(pNode->value)+" ", GREEN);
    if(!pNode->pMoves) PrintE("pMoves", RED);
    else
    {
        if(!pNode->pMoves->GetMoveFrom()) PrintE("MoveFrom", RED);
        else
        {
            PrintE(std::to_string(pNode->pMoves->GetMoveFrom()->GetPosition().X)+"-"
                +std::to_string(pNode->pMoves->GetMoveFrom()->GetPosition().Y)+"=", GREEN);
        }
        if(!pNode->pMoves->GetMoveTo()) PrintE("MoveTo", RED);
        else
        {
            PrintE(std::to_string(pNode->pMoves->GetMoveTo()->GetPosition().X)+"-"
                +std::to_string(pNode->pMoves->GetMoveTo()->GetPosition().Y)+"|", GREEN);
        }
    }
}
