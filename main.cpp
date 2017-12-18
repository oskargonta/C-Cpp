#include "mainwindow.h"


int main(int argc, char *argv[])
{

    QApplication a(argc, argv);
    MainWindow w;
    w.show();



    //podaj liczbe iteracji i podaj nazwy pliku
    // Wywołujesz następnie funkcje,ktora znajduje najlepszy harmonogram i oblicza dla niego Cmax


    // Teraz wyświatlamy int best_Cmax, vector<int>best_C, <vector>best_PI
    //                    --> dostęp do nich FFS.best_Cmax, FFS.best_C, FFS.best_PI
    // best_Cmax - jedna liczba - czas zakończenia wszystkich zadać
    // best_C - vector z momentami zakończeń kolejnych zadań, indeksowany od 1, best_C[n] - moment zakończenia n-tego zadania
    //          Wyświetlił bym to jako tablicę na początku. Pierwsza kolumna - numer zadania. Druga kolumna - moment zakończenia
    //          Nr.zad   | Moment zakończenia[s]
    //            1      |       10
    //            2      |       50
    // best_PI - vector z najlepszą permutacją. Kolejne maszyny oddzielone zerami. Wartości w wektorze to numery operacji.
    //           0 1 5 6 7 0 2 10 15 0 3 8 9 22 0 101 201 202 0
    // Też bym to na początku wyświetlił wierszami:
    // Dla liczby maszyn na stanowisku ms=2
    // Stanowisko 1 Maszyna 1 operacje: 1 5 6 7
    // Stanowisko 1 Maszyna 2 operacje: 2 10 15
    // Stanowisko 2 Maszyna 1 operacje: 3 8 9 22
    // Stanowisko 2 Maszyna 2 operacje: 101 201 202
    // jezeli trafi na 0 to zwiekszyc l maszyn i przejsc do kolejnego wiersza
    //jezeli l maszyn jest %liczba maszyn na stanowisku (ms) == 0 to zwiększ nr stanowiska.



    return a.exec();
}







