#include "Misc.h"
cViewPort* _ptr = nullptr;
SHORT infoX = 65;   /// позицию надо расчитать от размера основного поля + добавить макс длинну выводимой информации
SHORT infoY = 0;

void Print(const std::string& str)
{
    if(_ptr)
    {
        _ptr->PrintAtPos(infoX, infoY++, str); /// проверку на макс длинну и разбивку строки на несколько
                                                /// и хранение выводимых строк в контейнере для записи лога?
        if(infoY == 49)
        {
            infoX += 20;
            infoY = 0;
        }
    }
}
void PrintE(const std::string& str, COLOR color)
{
    if(_ptr)
    {
        _ptr->PrintAtPos(infoX, infoY, str, color); /// проверку на макс длинну и разбивку строки на несколько
        infoX += str.length();                                        /// и хранение выводимых строк в контейнере для записи лога?
        if (infoX > 110)
        {
            infoX = 65;
            infoY++;
            if(infoY == 49) infoY = 0;
        }
    }
}
void PrintAt(SHORT x, SHORT y, const std::string& str)
{
    if(_ptr)
    {
        _ptr->PrintAtPos(x, y, str);
    }
}
void SetViewPortPtr(cViewPort* ptr)
{
    _ptr = ptr;
}
