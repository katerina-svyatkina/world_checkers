#ifndef INPUTPORT_H_INCLUDED
#define INPUTPORT_H_INCLUDED
#include <windows.h>
class cInputPort
{
public:
    cInputPort();
    ~cInputPort();
    bool GetKeyPress(int key);
private:
    HANDLE _hConsole;
};

#endif // INPUTPORT_H_INCLUDED
