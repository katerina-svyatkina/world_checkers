#include "ViewPort.h"
#include <string>
//
const SHORT FIELD_SIZE_X = 6;   // ������� 3! ������ 5 ��� ��������� ������������ ������ ����
const SHORT FIELD_SIZE_Y = 3;   // ������� 3! ������������� �� �������� ������ !!!
const SHORT FIELD_OFFSET_X = 1; // ������� 1 ��� �������
const SHORT FIELD_OFFSET_Y = 1; // ������� 1 ��� �������
const SHORT FIELD_INFO_X = 30;  // ���-�� �������� �� X ��� ������ ����������
const SHORT FIELD_INFO_Y = 10;  // ���-�� �������� �� Y ��� ������ ����������
const COLOR WHITEFIELD = COLOR::DARKWHITE;
const COLOR BLACKFIELD = COLOR::GRAY;
//const SHORT FIELDS = 10;
//
///typedef BOOL (WINAPI * LPGetConsoleFontInfo)(HANDLE, BOOL, CONSOLE_FONT_INFO*);
//
///LPGetConsoleFontInfo GetConsoleFontInfo = reinterpret_cast<LPGetConsoleFontInfo>(GetProcAddress(GetModuleHandle("kernel32.dll"), "GetCurrentConsoleFont"));
cViewPort::cViewPort(SHORT x, SHORT y, SHORT fields)
{
    _fields = fields;
    _hConsole = GetStdHandle(STD_OUTPUT_HANDLE); // �������� ���������� ���������� ������������ ������
    GetConsoleScreenBufferInfo(_hConsole, &_ScreenInfo); // �������� ���� � �������
    GetConsoleCursorInfo(_hConsole, &_CursorInfo);  // ���� �� �������. �� ����� �����
    ///BOOL try = GetConsoleFontInfo(_hConsole, FALSE, &_FontInfo);

    _maxSize = GetLargestConsoleWindowSize(_hConsole);  // ����������� ������������ ��������� ������ ������� (� ��������)
    SHORT minX = _fields * FIELD_SIZE_X + FIELD_OFFSET_X + FIELD_INFO_X;
    SHORT minY = _fields * FIELD_SIZE_Y + FIELD_OFFSET_Y + FIELD_INFO_Y;
    if (x < minX) x = minX;
    if (y < minY) y = minY;
    if (_maxSize.X <= x) x = _maxSize.X - 1;    // ������������ ������ ����
    if (_maxSize.Y <= y) y = _maxSize.Y - 1;
    SMALL_RECT ConsoleRect{0,0,--x,--y};    // ���������� ����������� 0....MAX-1
    COORD BufSize{++x, ++y}; // ������ ������
    _currentSize.X = x;
    _currentSize.Y = y;
    _CursorInfo.bVisible = false; // ������ ����������� ��������� �������
    SetConsoleCursorInfo(_hConsole, &_CursorInfo);
    SetConsoleScreenBufferSize(_hConsole, BufSize);
    SetConsoleWindowInfo(_hConsole, TRUE, &ConsoleRect);
    SetConsoleOutputCP(872);    //������� �������� � �������������� ��� ����� - 866, 855
    DrawBorder();
}
cViewPort::~cViewPort()
{
    //SetConsoleOutputCP(1251);
}
void cViewPort::DrawBorder(void)
{
    constexpr char c1 = 0xC9; /// ��� � ����� ���� �������� ������������� � ���������������� ������� (��. ������� ��������� 866 ��� 872 ��� 845?)
    std::string str2(_fields * FIELD_SIZE_X, 0xCD);
    constexpr char c3 = 0xBB;
    SetColors(COLOR::GREEN, COLOR::DARKGREEN);  /// ��������� ���������� ����� �������
    COORD pos;
    for (pos.Y = FIELD_OFFSET_Y; pos.Y < (_fields * FIELD_SIZE_Y + FIELD_OFFSET_Y); ++pos.Y)
    {
        (pos.X = FIELD_OFFSET_X)--;
        SetCursorPosition(pos);
        constexpr char ch = 0xBA;
        std::cout << ch;
        (pos.X+= _fields * FIELD_SIZE_X)++;
        SetCursorPosition(pos);
        std::cout << ch;
    }
    pos = {(FIELD_OFFSET_X - 1), (FIELD_OFFSET_Y - 1)};
    SetCursorPosition(pos);
    std::cout << c1 << str2 << c3;   // �������������� ������� (����)
    pos.Y = FIELD_OFFSET_Y + _fields * FIELD_SIZE_Y;
    SetCursorPosition(pos);
    constexpr char open = 0xC8;
    constexpr char close = 0xBC;
    std::cout << open << str2 << close;  // �������������� ������� (���)
    ResetColors();
}
void cViewPort::DrawGameBoard(cField** const ppBoard)
{
    DrawBorder();
    for(SHORT x = 0; x < _fields; ++x)
    {
        for(SHORT y = 0; y < _fields; ++y)
        {
            DrawFieldWithChecker(ppBoard[x][y]);
        }
    }
}
//void cViewPort::DrawChanges(std::vector<cField*>& changes)
void cViewPort::DrawChanges(cVector<cField*>& changes)
{
    while (!changes.empty())
    {
        size_t sz = changes.size();
        DrawFieldWithChecker(*(changes.back()));
        changes.pop_back();
    }
}
void cViewPort::PrintAtPos(SHORT x, SHORT y, const std::string& str, COLOR color)
{
    GetConsoleScreenBufferInfo(_hConsole, &_ScreenInfo);
    SetCursorPosition(x,y);
    SetColors(color, BLACK);
    std::cout << str;
    SetCursorPosition(_ScreenInfo.dwCursorPosition);
    ResetColors();
}
void cViewPort::DrawFieldWithChecker(const cField& fld)
{
    COORD tmp = TranslateFieldPosToScreenPos(fld.GetPosition());
    DrawField(tmp, GetBlackOrWhite(fld.GetBlack()),
              fld.GetCanBeSelected(), fld.GetSelected(), fld.GetOverJump());
    CHECK chk = fld.GetChecker();
    if (chk)
    {
        //DrawChecker(tmp, (chk == WHITEQUEEN || chk == BLACKQUEEN),
                    //true, (chk == WHITEQUEEN || chk == WHITECHECK) ? WHITE : BLACK);
        DrawChecker(tmp, fld.GetCheckerIsQueen(), true, fld.GetCheckerIsBlack() ? BLACK : WHITE);
    }
}
void cViewPort::SetColors(const COLOR text, const COLOR ground)
{
    //WORD attr = ground * 16 + text;   // ������� ����� ���������
    WORD attr = (ground << 4) + text;   // ����� ��������� ��������
    SetConsoleTextAttribute(_hConsole, attr);
}
COLOR cViewPort::GetBlackOrWhite(bool black)
{
    return black ? BLACKFIELD : WHITEFIELD;
}
void cViewPort::ResetColors(void)
{
    SetConsoleTextAttribute(_hConsole, _ScreenInfo.wAttributes);
}
void cViewPort::SetCursorPosition(SHORT x, SHORT y)
{
    SetCursorPosition({x,y});
}
void cViewPort::SetCursorPosition(const COORD& pos)
{
    //_ScreenInfo.dwCursorPosition = pos;
    SetConsoleCursorPosition(_hConsole, pos);
}
void cViewPort::DrawField(const COORD& pos, const COLOR color, bool can, bool sel, bool jump)
{   /// !!!!!!! �� �������� !!!!!!!!!!!!!
    COORD tmp = pos;
    std::string str(FIELD_SIZE_X, ' '); // �������� ������ �� ��������� ����� ��������
    SetColors(color,color);
    for (; tmp.Y < pos.Y + FIELD_SIZE_Y; ++tmp.Y)
    {
        SetCursorPosition(tmp);
        std::cout << str;
    }
    if (can || sel || jump) // ������ ������� ���������� �������
    {
        constexpr char ch = 0xB1;
        COLOR front;    /// �������� �������� �������� ����� ���������� can/sel/jump
        COLOR back = BLACK;
        if(sel) front = RED;
        if(can)
        {
            if(sel) back = DARKGREEN;
            else front = DARKGREEN;
        }
        if (jump)
        {
            if(sel) back = DARKRED;
            if(can) back = DARKCYAN;
            front = GRAY;
        }
        SetColors(back, front);
        tmp = pos;
        SetCursorPosition(tmp);
        std::cout << ch;
        tmp.X += FIELD_SIZE_X - 1;
        SetCursorPosition(tmp);
        std::cout << ch;
        tmp.Y += FIELD_SIZE_Y - 1;
        SetCursorPosition(tmp);
        std::cout << ch;
        tmp.X = pos.X;
        SetCursorPosition(tmp);
        std::cout << ch;
    }
    ResetColors();
}
void cViewPort::DrawChecker(const COORD& pos, bool queen, bool big, COLOR color)
{
    COORD tmp = pos;
    SetColors(color, BLACKFIELD);
    if (FIELD_SIZE_Y < 3 || FIELD_SIZE_X < 3) big = false;
    if (big)
    {
        constexpr char up = 0xDC;
        constexpr char dn = 0xDF;
        constexpr char vert = 0xDB;
        std::string str(FIELD_SIZE_X - 2, up);
        tmp.X++;
        SetCursorPosition(tmp);
        std::cout << str;
        (--tmp.Y += FIELD_SIZE_Y);
        str = std::string(FIELD_SIZE_X - 2, dn);
        SetCursorPosition(tmp);
        std::cout << str;
        DrawMiddleOfChecker(pos, color, BLACKFIELD, vert, (queen ? ' ' : 0xDB));    /// ����� �� ����� �� ������ �����, ����� ������� � ���������������� ��� ����
        /*char qw = queen ? ' ' : 0xDB;
        str = std::string(FIELD_SIZE_X - 2, qw);
        (tmp.Y = pos.Y)++;
        tmp.X = pos.X;
        for (SHORT y = 0 ; y < (FIELD_SIZE_Y - 2); ++y)
        {
            SetCursorPosition(tmp);
            std::cout << vert << str << vert;
            tmp.Y++;
        }*/
    }
    else // ���� �� �����-�� �����������, ���� ������ ������ ����� 3�3
    {
        char qw = queen ? 0x01 : 0x02;
        constexpr SHORT centrX = FIELD_SIZE_X / 2;
        constexpr SHORT centrY = FIELD_SIZE_Y / 2;
        tmp.X += centrX;
        tmp.Y += centrY;
        SetCursorPosition(tmp);
        std::cout << qw;
    }
    ResetColors();
}
void cViewPort::DrawMiddleOfChecker(const COORD& pos, const COLOR text, const COLOR ground, const char edge, const char mid)
{
    COORD tmp = pos;
    SetColors(text, ground);
    std::string str(FIELD_SIZE_X - 2, mid);
    (tmp.Y = pos.Y)++;
    tmp.X = pos.X;
    for (SHORT y = 0 ; y < (FIELD_SIZE_Y - 2); ++y)
    {
        SetCursorPosition(tmp);
        std::cout << edge << str << edge;
        tmp.Y++;
    }
    ResetColors();
}
void cViewPort::DropCursor(void)
{
    SetCursorPosition(0, _currentSize.Y - 5);
}
COORD cViewPort::TranslateFieldPosToScreenPos(const COORD& fieldPos)
{
    //return {static_cast<SHORT>(FIELD_OFFSET_X + ((_blackUp ? (_fields - 1 - fieldPos.X) : fieldPos.X)) * FIELD_SIZE_X),
    //        static_cast<SHORT>(FIELD_OFFSET_Y + ((_blackUp ? (_fields - 1 - fieldPos.Y) : fieldPos.Y)) * FIELD_SIZE_Y)};
    return {static_cast<SHORT>(FIELD_OFFSET_X + (_fields - 1 - fieldPos.X) * FIELD_SIZE_X),
            static_cast<SHORT>(FIELD_OFFSET_Y + (_fields - 1 - fieldPos.Y) * FIELD_SIZE_Y)};
}

