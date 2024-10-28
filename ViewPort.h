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
    cViewPort(SHORT x, SHORT y, SHORT fields);    // размер в символах на строку (x) и кол-во строк (y)
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
    void SetColors(const COLOR text, const COLOR ground); // установка цвета текста и фона
    COLOR GetBlackOrWhite(bool black);
    void ResetColors(void); // сброс цветов на дефолтные
    void SetCursorPosition(SHORT x, SHORT y);  // установка курсора вывода в экранную позицию (в символах)
    void SetCursorPosition(const COORD& pos);  // другая версия
    COORD TranslateFieldPosToScreenPos(const COORD& fieldPos); // перевод координат позиции на доске (0...9) в экранные координаты
    void DrawBorder(void);
    void DrawField(const COORD& pos, const COLOR color, bool can, bool sel, bool jump); // нарисовать клетку доски в экранной позици, заданного цвета и свойств
    void DrawFieldWithChecker(const cField& fld); ///
    void DrawChecker(const COORD& pos, bool queen, bool big, COLOR color);
    void DrawMiddleOfChecker(const COORD& pos, const COLOR text, const COLOR ground, const char edge, const char mid);   // sym - символ для заполнения области внутки шашки
};
#endif // VIEWPORT_H_INCLUDED
