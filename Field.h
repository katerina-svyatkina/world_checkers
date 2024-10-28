#ifndef FIELD_H_INCLUDED
#define FIELD_H_INCLUDED
#include <windows.h>
//
const SHORT FIELDS = 10;
COORD& operator+= (COORD& first, const COORD& second); // объ€вление переопределени€ опаратора (реализаци€ в Field.cpp)
//
enum CHECK
{
    NO = 0,
    WHITECHECK,
    BLACKCHECK,
    WHITEQUEEN,
    BLACKQUEEN
};

class cField
{
public:
    cField();
    cField(const COORD& pos, bool black);
    ~cField();
    const COORD& GetPosition(void) const;
    void SetPosition(const COORD& pos); /// position in Fields(Board) coordinates (0...10) FIELDS
    void SetPosition(SHORT x, SHORT y);
    void SetBlack(bool black);
    bool GetBlack(void) const;
    void SetChecker(CHECK check);
    CHECK GetChecker(void) const;
    bool GetCheckerIsBlack(void) const;
    bool GetCheckerIsQueen(void) const;
    bool GetCanBeSelected(void) const;
    void SetCanBeSelected(bool sel);
    bool GetSelected(void) const;
    void SetSelected(bool sel);
    void SetOverJump(bool jump);
    bool GetOverJump(void) const;
private:
    COORD _pos; // координаты пол€
    bool _black;    // цвет пол€ (black = true, white = false)
    ///cChecker _checker;  /// ??? not used
    CHECK _checkerIs = NO;  // наличие шашки (нет, бела€, черна€, ...)
    bool _blackChecker;
    bool _queen;
    bool _canBeSelected = false;    //визулизаци€, что на поле можно переместитьс€
    bool _selected = false; // визуализаци€, что поле выбрано
    bool _overJump = false; // визуализаци€, что поле €вл€етс€ промежуточным при серии прыжков
};
#endif // FIELD_H_INCLUDED
