#ifndef VIEWPORT_H_INCLUDED
#define VIEWPORT_H_INCLUDED
#include <iostream>
#include <Windows.h>
//#include <vector>
#include "Vector.h"
#include "Field.h"
//#include "Game.h"
enum COLOR
{
    BLACK = 0,
    DARKBLUE,
    DARKGREEN,
    DARKCYAN,
    DARKRED,
    DARKMAGENTA,
    DARKYELLOW,
    DARKWHITE,
    GRAY,
    BLUE,
    GREEN,
    CYAN,
    RED,
    MAGENTA,
    YELLOW,
    WHITE
};

class cViewPort
{
public:
    cViewPort(SHORT x, SHORT y, SHORT fields);    // ������ � �������� �� ������ (x) � ���-�� ����� (y)
    ~cViewPort();
    void DrawGameBoard(cField** const ppBoard);
    void DropCursor(void);  /// TEMP metod
    //void DrawChanges(std::vector<cField*>& changes);
    void DrawChanges(cVector<cField*>& changes);
    void PrintAtPos(SHORT x, SHORT y, const std::string& str, COLOR color = WHITE);

private:
    HANDLE _hConsole;
    CONSOLE_SCREEN_BUFFER_INFO _ScreenInfo;
    CONSOLE_CURSOR_INFO _CursorInfo;
    CONSOLE_FONT_INFO _FontInfo;
    COORD _maxSize;
    COORD _currentSize;
    SHORT _fields;
    void SetColors(const COLOR text, const COLOR ground); // ��������� ����� ������ � ����
    COLOR GetBlackOrWhite(bool black);
    void ResetColors(void); // ����� ������ �� ���������
    void SetCursorPosition(SHORT x, SHORT y);  // ��������� ������� ������ � �������� ������� (� ��������)
    void SetCursorPosition(const COORD& pos);  // ������ ������
    COORD TranslateFieldPosToScreenPos(const COORD& fieldPos); // ������� ��������� ������� �� ����� (0...9) � �������� ����������
    void DrawBorder(void);
    void DrawField(const COORD& pos, const COLOR color, bool can, bool sel, bool jump); // ���������� ������ ����� � �������� ������, ��������� ����� � �������
    void DrawFieldWithChecker(const cField& fld); ///
    void DrawChecker(const COORD& pos, bool queen, bool big, COLOR color);
    void DrawMiddleOfChecker(const COORD& pos, const COLOR text, const COLOR ground, const char edge, const char mid);   // sym - ������ ��� ���������� ������� ������ �����
};
#endif // VIEWPORT_H_INCLUDED
