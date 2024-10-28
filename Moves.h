#ifndef MOVES_H_INCLUDED
#define MOVES_H_INCLUDED
//#include <vector>
#include "Vector.h"
//#include <list>
#include "List.h"
#include "Field.h"
#include "Board.h"
class cMoves    // ����� ������� �����������
{
    struct sMove    // ���������, ������� ��� �� �����
    {
        sMove(cField* from, cField* to, bool toQueen, cField* eat);
        ~sMove();
        cField* _pFrom;
        cField* _pTo;
        cField* _pEat;
        CHECK _eatCheck = NO;
        bool _toQueen;  // ������������ �� ����� � �������� ���� � �����
    };
public:
    cMoves();
    cMoves(cField* from, cField* to, bool toQueen, cField* eat); // ������� ������ � ����� ���������
    ~cMoves();
    // ������ ��� �������� ��������� � ������ ����� � �������� ��������� (��� �������� ��������� �����������)
    void AddMove(cField* from, cField* to, bool toQueen, cField* eat);
    void AddAndMakeMove(cField* from, cField* to, bool toQueen, cField* eat);
    void RemoveLastMove(void);
    void RemoveAndUnMakeLastMove(void);
    size_t GetSize(void) const;
    cField* const GetMoveFrom(void) const;
    cField* const GetMoveTo(void) const;
    void MakeAllMove(cBoard* pBoard);
    bool MakeAllMove(bool change = false, cBoard* pBoard = nullptr); // � ����������� � �������, ���� ������� �����
    void MakeAllUnMove(bool change = false, cBoard* pBoard = nullptr);
    bool isEmpty(void);
    void CheckLastMoveReachQueenLine(cBoard* const pBoard); // �������� ���������� ������ ��������������� ������� ��������� ������������ (� �����)
    void SetFieldsMarkersByMoves(bool mark, bool canFrom, bool canTo, bool selFrom, bool selTo, cBoard* pBoard = nullptr);
    void ResetFieldsMarkers(cBoard* pBoard = nullptr);
private:
    void MakeMove(sMove& sMv);  // ��� �������� ��������� � ������ ����� � � �������� ���������
    void MakeUnMove(sMove& sMv);
    // ���� ������ �������� ��������� � ������ �����
    //void MarkMoveResultsOnCheckersLists(sMove& sMv, std::list<cField*>* pMain, std::list<cField*>* pEated);
    //void MarkUnMoveResultsOnCheckersLists(sMove& sMv, std::list<cField*>* pMain, std::list<cField*>* pEated);
    void MarkMoveResultsOnCheckersLists(sMove& sMv, cList<cField*>* pMain, cList<cField*>* pEated);
    void MarkUnMoveResultsOnCheckersLists(sMove& sMv, cList<cField*>* pMain, cList<cField*>* pEated);
    void AddChanges(cBoard* pBoard, sMove& sMv); // �������� ��������� � �������� ��������� (cBoard._Changes)
    //void SetFieldsMarkersByMoves(bool mark, bool selFrom, bool selTo);
    //std::vector<sMove> _moves;  // ��������� ������� ����������� (1 ��� ��� �������, � ��������� ��� ������� ������)
    cVector<sMove> _moves;
};
#endif // MOVES_H_INCLUDED
