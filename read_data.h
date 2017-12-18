#ifndef READ_DATA_H
#define READ_DATA_H

#include "library.h"
// Sprawdzenie poprawności pliku
bool checkFile(ifstream& plik);
// Wczytanie danych z pliku do vectora stringów - każda linia, element w vectorze
void getLines(ifstream& plik, vector<string>& lines);
// Konfwersja stringów na int, liczby oddzielane spacjami
void divideNumberBySpace(string& linia, vector<int>& lineInVector);

#endif // READ_DATA_H
