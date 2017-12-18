#ifndef FLEXFLOWSHOP_H
#define FLEXFLOWSHOP_H

#include "library.h"

class FlexFlowShop
{
   // n-liczba zadań, s-liczba stanowisk,
   // ms - liczba maszyn na stanowisku, Cmax - czas zakończenia zadań
    int n,s,ms,Cmax;
   // p - czasy wykonania kolejnych operacji, indeks numer operacji,
   //     indeksowane od 1
   // PI - permutacje, kolejność wykonywanie operacji na maszynach,
   //      kod oznacza numer operacji, dane maszyny oddzielone 0.
   //      0 2 5 7 0 6 7 91 0 - druga operacja na pierwszej maszynie,
   //      potem 5 operacja itd. Operacje numerowane kolejno - 1 2 3 4 5
   //       1-1 operacja pierwszego zadania, 2-2 operacja 1 zadania,
   //       3-1 operacja drugiego zadania.
   // C - czasy zakończenia kolejnych operacji, indeks numer operacji,
   //     indeksowane od 1
   // P - tablica poprzedników technologicznych, indeks numer operacji,
   //     wartosc to indeks poprzednika technologicznego,
   // PH - tablica z punktmai styków (do wyszukiwania ścieżki krytycznej),
   //      indeksowane od 1, indeksy i wartosci to numery operacji. Indeks
   //      to operacja po lewej stronie punktu styku, wartość to operacja
   //      po prawej stronie styku
   // cPath - ścieżka krytyczna zawiera numery operacji.
   // stanBlock - oznaczenie bloków krytycznych w ścieżce krytycznej,
   // 1 - poczatek bloku, 0 - srodek , 2 - koniec bloku.
   vector<int> p,PI,C,P,PH,cPath,stenBlock;
   // mix_Cmax - wektor z wyznaczonymi C_max dla zamiany pozycji operacji
   // krytycnych
   vector <mix_struct> mix_Cmax;
   // perm_list - lista zabronionych pozycji. Element {x1,x2} = {2,3}, oznacza iż operacja nr 2
   //             nie może znaleźć się przed operacją 3
   vector <permitted_pos> perm_list;
public:
    vector<int> best_C, best_PI;
    int  best_Cmax;
    FlexFlowShop();
    bool Init_FlexFlowShop(string);
    void Disp_p() { for(int& num : p) cout << num << " "; }
    void Disp_PI() { for(int& num : PI) cout << num << " "; }
    void Disp_P() { for(int& num : P) cout << num << " "; }
    void Disp_C() { for(int& num : C) cout << num << " "; }
    void Disp_Cmax() { cout << Cmax;}
    void Init_P_table();
    void Calc_C();
    void max_C(){ Cmax=*max_element(C.begin(),C.end());}
    void find_critical_path();
    int return_Cmax(){ return Cmax;}
    void mix_critical_oper();
    bool find_best_perm(string,int inter);
    bool find_best_move();
    bool is_perm_valid();
    int index_m(int oper);
    int getMs() { return ms; }


};

#endif // FLEXFLOWSHOP_H
