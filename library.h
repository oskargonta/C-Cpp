#ifndef LIBRARY_H
#define LIBRARY_H


#define SIZE_PERM_LIST 15
#include <QApplication>
#include <iostream>
#include <sstream>
#include <vector>
#include <list>
#include <fstream>
#include <string>
#include <algorithm>
#include <ctime>



using namespace std;

struct mix_struct
{
    int Cmax=0;
    int oper_nr=0;               // operacja ze ścieżki krycznej
    int distance_to_oper_2=0;    // odległość w wektorze PI
    int type_of_move=0; // 1 - na lewo, 2 - na prawo, 3 - na inna maszyne lewo 4 - na inna maszyne prawo
};
struct permitted_pos
{
    int x1=0;       // pozycja operacji 1
    int x2=0;       // pozycja operacji 2
    int index_m=0;  // index maszyny na której operacja 1 nie moze byc przed operacją 2
};

#endif // LIBRARY_H
