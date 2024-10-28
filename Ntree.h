#ifndef NTREE_H_INCLUDED
#define NTREE_H_INCLUDED
#include "Moves.h"
#include <list>
#include "List.h"
#include <windows.h>
class cNode /// ����� �� �� �������� ���
{
public:
    //cNode(int val, cField* from, cField* to, bool toQueen, cField* eat = nullptr);
    cNode(int val, cMoves* pMoves);
    ~cNode();
//private:
    int value;
    //int min; /// �� ������������
    //int max; /// �� ������������
    cMoves* pMoves = nullptr;
    cNode* pPrev = nullptr;
    //std::list<cMoves> nextMovesLst; /// ������ ��������� ����������� !!! ���� ���� ����� �� �� ������
    //std::list<cNode*> nextNodesLst; // ������ ��������� �������� ����� (�� ����������� ����� ����� �����������)
    cList<cMoves> nextMovesLst; /// ������ ��������� ����������� !!! ���� ���� ����� �� �� ������
    cList<cNode*> nextNodesLst; // ������ ��������� �������� ����� (�� ����������� ����� ����� �����������)
    size_t deep = 0;
    bool win = false;
};
class cNTree
{
public:
    cNTree();
    //cNTree(std::list<std::list<cMoves>>* lstLstMoves, int val);
    cNTree(std::list<cList<cMoves>>* lstLstMoves, int val);
    ~cNTree();
    /// ! ����� ! ���� �� ������ (�� ���������) �������� pNode, ����� ����������� ������� ����.
    cNode* AddNode(cMoves* pMoves, int val, cNode* pNode = nullptr); /// �������� �� ��������� !!!! �� �����, ����� ������� �������!!! ��������� ����������
    cNode* AddNode(cMoves* pMoves, cNode* pNode = nullptr); // ��������, ���� �������� ���� �� ��������
    cNode* AddNodeAndCurrentUp(cMoves* pMoves, int val); // �������� ���� � �������� ���� � ������� � ������
    bool AddAllNodesFromInnerMovesList(cNode* pNode = nullptr); // �������� ��� ���� � ����������� ��������� �� ������ ����� /// �� ������������
    void SaveMovesList(void);
    void Clear(void);
    cNode* CurrentUp(cMoves* pMoves);
    cNode* CurrentDown(cMoves* pMoves = nullptr);
    cNode* GetCurrentNodePtr(void);
    //sNode* CurrentJump(cField* pField);
    //sNode* Find(sNode* pNodeFrom, cField* pField);
    void PrintTree(void);
    size_t GetVariations(void); /// temp
    size_t GetMaxDeep(void);
    void SetMaxDeep(size_t mxDp);
    bool ReachMaxDeep(void);
    size_t GetNodeDeep(cNode* pNode = nullptr);
    int GetNodeValue(cNode* pNode = nullptr);
    int SetNodeValue(int val, cNode* pNode = nullptr);
    bool isLoose(void);
    //std::list<cMoves>* GetNodeMovesListPtr(cNode* pNode = nullptr);
    cList<cMoves>* GetNodeMovesListPtr(cNode* pNode = nullptr);
    //void FillBestMovesList(std::list<cMoves*>* pMovesLst);
    void FillBestMovesList(cList<cMoves*>* pMovesLst);
    void NodePrint(cNode* pNode = nullptr);
    size_t _construct = 0;  /// temp
    size_t _destruct = 0;   /// temp
private:
    void Delete(cNode* pNode);
    void PrintNode(cNode* pNode);
    cNode* _pNodes = nullptr;
    //sNode* _pStart = nullptr;
    cNode* _pCurrent = nullptr;
    size_t _maxDeep = 0;
    size_t _variation = 0;/// temp
};

#endif // NTREE_H_INCLUDED
