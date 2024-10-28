#include "Field.h"
/// переопределение оператора. Реализация.
COORD& operator+= (COORD& first, const COORD& second)
{
    first.X += second.X;
    first.Y += second.Y;
    return first;
}
///
cField::cField()
{
    cField({0,0}, false);   /// корректно конструктор явно вызывать из конструктора?
    //_pos = {0,0};
    //_black = false;
}
cField::cField(const COORD& pos, bool black)
{
    SetPosition(pos);
    _black = black;
}
cField::~cField()
{

}
const COORD& cField::GetPosition(void) const
{
    return _pos;
}
void cField::SetPosition(const COORD& pos)  /// поменять местами содержимое обоих SetPosition
{
    _pos = pos;
    if (_pos.X < 0) _pos.X = 0;
    else if (_pos.X >= FIELDS) _pos.X = FIELDS - 1;
    if (_pos.Y < 0) _pos.Y = 0;
    else if (_pos.Y >= FIELDS) _pos.Y = FIELDS - 1;
}
void cField::SetPosition(SHORT x, SHORT y)
{
    SetPosition({x,y});
}
void cField::SetBlack(bool black)
{
    _black = black;
}
bool cField::GetBlack(void) const
{
    return _black;
}
void cField::SetChecker(CHECK check)    /// может быть const CHECK
{
    _checkerIs = check;
    _blackChecker = (check == BLACKCHECK || check == BLACKQUEEN);
    _queen = (check == WHITEQUEEN || check == BLACKQUEEN);
}
CHECK cField::GetChecker(void) const
{
    return _checkerIs;
}
bool cField::GetCheckerIsBlack(void) const
{
    return _blackChecker;
}
bool cField::GetCheckerIsQueen(void) const
{
    return _queen;
}
bool cField::GetCanBeSelected(void) const
{
    return _canBeSelected;
}
void cField::SetCanBeSelected(bool sel)
{
    _canBeSelected = sel;
}
bool cField::GetSelected(void) const
{
    return _selected;
}
void cField::SetSelected(bool sel)
{
    _selected = sel;
}
void cField::SetOverJump(bool jump)
{
    _overJump = jump;
}
bool cField::GetOverJump(void) const
{
    return _overJump;
}
