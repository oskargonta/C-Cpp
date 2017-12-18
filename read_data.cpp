#include "read_data.h"

bool checkFile(ifstream& plik)
{
    if(!plik.is_open())
    {
        return true;
    }
    return false;
}

void getLines(ifstream& plik, vector<string>& lines)
{
    string linia;
    while(!plik.eof())
    {
        getline(plik, linia);
        if(linia != "")
          lines.push_back(linia);
    }
}

void divideNumberBySpace(string& linia, vector<int>& lineInVector)
{
    istringstream iss(linia);
    while(iss)
    {
        string subs;
        iss >> subs;
        if(subs != "")
            lineInVector.push_back(atoi(subs.c_str()));
    }
}
