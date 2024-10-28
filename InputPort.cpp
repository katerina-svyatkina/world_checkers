#include "InputPort.h"
cInputPort::cInputPort()
{
    _hConsole = GetStdHandle(STD_OUTPUT_HANDLE); // for future use (if try mouse)
}
cInputPort::~cInputPort()
{

}
bool cInputPort::GetKeyPress(int key)
{
    return GetAsyncKeyState(key) & 0x01;
}
