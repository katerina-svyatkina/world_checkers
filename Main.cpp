#pragma comment (lib, "Winmm.lib")
#include <iostream>
#include <locale>
#include <windows.h>
#include "ViewPort.h"
#include "InputPort.h"
#include "Game.h"
#include "Misc.h"
#include <algorithm>
#include "Vector.h" /// temp
#include "List.h"   /// temp
int main()
{
    //std::vector<int> lst {4,5,634,5,67,78,78};
    //std::sort(lst.begin(), lst.end());
   //setlocale(LC_ALL, "Russian");
    //cout << "Привет|" << endl;
    ///------------- TEST cVector -------------------///
    /*cList<int> lst;
    lst.push_back(10);
    lst.push_back(20);
    lst.push_back(30);
    lst.push_back(40);
    //lst.push_back(50);
    //lst.push_back(60);
    std::cout << lst.erase(3) << std::endl;
    std::cout << "front: "<< lst.front() << std::endl;
    std::cout << "back: "<< lst.back() << std::endl;
    std::cout << "find: "<< lst.find(20) << std::endl;
    //lst.erase(4);
    lst.Print();
    getchar();  /// temp*/
    //cField* pFldFrom = new cField({1,1}, true);
    //cField* pFldTo = new cField({2,2}, true);
    //cVector<cMoves>* mvs = new cVector<cMoves>();

    /*cVector<cField>* fld = new cVector<cField>();
    fld->push_back(*pFldFrom);
    COORD pos{3,3};
    fld->emplace_back(pos, true);
    delete fld;*/

    /*cVector<int> vec;
    vec.push_back(1);
    vec.push_back(2);
    vec.push_back(3);
    //std::cout << vec[2] << std::endl;
    vec.print();
    getchar();  /// temp
    vec.reserve(10);
    vec.print();
    getchar();  /// temp
    if(!vec.empty())
    {
        std::cout << vec.back() << " ";
        vec.pop_back();
        std::cout << vec.back() << " " << std::endl;
        //std::cout << vec.consist(2) << " " << std::endl;
    }
    getchar();  /// temp
    cVector<cField*> vec2;
    cField* pFld = new cField;
    vec2.push_back(pFld);
    pFld = new cField;
    vec2.push_back(pFld);
    pFld = new cField;
    vec2.push_back(pFld);
    vec2.print();
    getchar();  /// temp
    if(!vec.empty())
    {
        std::cout << vec2.back() << " ";
        vec2.pop_back();
        std::cout << vec2.back() << " " << std::endl;
    }
    getchar();  /// temp
    */
    ///----------------------------------------------------------///
    cInputPort IP;
    cGame GM(FIELDS, &IP);   //объект игры
    cViewPort VP(120, 50, GM.GetFields()); // объект отображения. Размеры по идее надо высчитывать по размеру поля
    SetViewPortPtr(&VP);    // установка указателя на порт вывода (для Print'ов из Misc.cpp)
    while(!IP.GetKeyPress(VK_ESCAPE))   /// проверка на ESC временная
    {
        GM.Update();
        VP.DrawChanges(GM.GetChanges());
        Sleep(100);
    }
    VP.DropCursor();
    //getchar();
    //getchar();
    //getchar();
    return 0;
}
