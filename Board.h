#ifndef BOARD_H_INCLUDED
#define BOARD_H_INCLUDED
//#include <vector>
#include "Vector.h"
//#include <list>
#include "List.h"
#include "Field.h"
//
//const size_t FIELDS = 10;
//
class cBoard
{
    struct sBoardData
    {
        sBoardData();
        ~sBoardData();
        cField** _ppBoardArr;
        //std::list<cField*> _White;  //
        //std::list<cField*> _Black;  //
        cList<cField*> _White;  //
        cList<cField*> _Black;  //
    };
public:
    cBoard();
    ~cBoard();
    void AddChange(cField* pFld);
    //std::vector<cField*>& GetChanges(void);
    cVector<cField*>& GetChanges(void);
    //std::list<cField*>* GetWhiteListPtr(void);
    //std::list<cField*>* GetBlackListPtr(void);
    cList<cField*>* GetWhiteListPtr(void);
    cList<cField*>* GetBlackListPtr(void);
    cField* GetFieldPtr(const COORD& pos);
    cField* GetFieldPtr(SHORT x, SHORT y);
    void AddFieldToList(cField* pFld, bool black);
    void PlaceCheckers(bool black, SHORT num);
    void SaveBoardData(void);
    void RestoreBoardData(void);
    //void ScanFreeFieldsOnDirection(cField* const pFld, const COORD& dir, std::list<cField*>* pOutLst);
    //void ScanFreeFieldsOnDirection(cField* const pFld, const COORD& dir, bool queen, std::list<cField*>* pOutLst);
    void ScanFreeFieldsOnDirection(cField* const pFld, const COORD& dir, cList<cField*>* pOutLst);
    void ScanFreeFieldsOnDirection(cField* const pFld, const COORD& dir, bool queen, cList<cField*>* pOutLst);
    cField* ScanOpositeCheckerOnDirection(cField* const pFld, const COORD& dir);
    bool CheckerReachOpositeSide(cField* const pFldFrom, cField* const pFldTo); // проверка достигла ли шашка противоположного края
    bool CheckDraw(void);
    bool OnlyQueenOnSide(CHECK side);
    bool CheckersInDrawOnSide(CHECK side);
private:
    sBoardData _boardData;
    sBoardData _boardDataSave;
    //std::vector<cField*> _Changes;
    cVector<cField*> _Changes;
};

#endif // BOARD_H_INCLUDED
