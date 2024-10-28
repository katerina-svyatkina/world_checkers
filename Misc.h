#ifndef MISC_H_INCLUDED
#define MISC_H_INCLUDED
#include "ViewPort.h"

void Print(const std::string& str);
void PrintE(const std::string& str, COLOR color = WHITE);
void PrintAt(SHORT x, SHORT y, const std::string& str);
void SetViewPortPtr(cViewPort* ptr);

#endif // MISC_H_INCLUDED
