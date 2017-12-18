#include "flexflowshop.h"
#include "read_data.h"

FlexFlowShop::FlexFlowShop()
{
    n=s=ms=Cmax=0;
    p.push_back(0);   // Wymagane do poprawnego działania algorytmu
    //PI.push_back(0);
    P.push_back(0);
    C.push_back(0);
    PH.push_back(0);
}
bool FlexFlowShop::Init_FlexFlowShop(string path)
{
    vector<int> tmp;
    vector<string> lines;
    // Wczytanie danych z pliku
    ifstream plik;
    plik.open(path);
    if(checkFile(plik)) return true;
    getLines(plik,lines);
    plik.close();
    // Podział danych
    divideNumberBySpace(lines[0],tmp);
    divideNumberBySpace(lines[1],p);
    divideNumberBySpace(lines[2],PI);
    n = tmp[0];
    s = tmp[1];
    ms = tmp[2];
    this->Init_P_table();
    return false;
}

void FlexFlowShop::Init_P_table()
{
    for(int i=1;i<=n*s;++i)
    {
        if((i-1)%s == 0)
            P.push_back(0);
        else
            P.push_back(i-1);
    }
    PH.resize(n*s+1);
    C.resize(n*s+1);
}

int FlexFlowShop::index_m(int oper)
{
    int i=0;

    // *Sprawdzać tylko na indeksy na danym stanowisku ! A nie w całej permutacji
    for(vector<int>::iterator it = PI.begin();
        (*it) != oper ; ++it)
    {
        if((*it) == 0)
            i=i+1;
    }
    return i;
}

void FlexFlowShop::Calc_C()
{
    C.clear();
    C.resize(n*s+1);
    PH.clear();
    PH.resize(n*s+1);

    for(int i=1; i<=(n*s+ms*s);++i)
    {
        if(PI[i] != 0)
        {
            if(C[PI[i-1]] >= C[P[PI[i]]] )
            {
                C[PI[i]] = C[PI[i-1]] + p[PI[i]];
                // Punkt Styku
                PH[PI[i]] = PI[i-1];  // Poprzednik maszynowy operacji PI[i]
            }
            else
            {
                C[PI[i]] = C[P[PI[i]]] + p[PI[i]];
                PH[PI[i]] = P[PI[i]]; // Poprzednik technologiczny operacji PI[i]
            }
            //            Wersja bez if, bez tworzenia tablicy z punktami styków.
            //            C[PI[i]]= max(C[PI[i-1]],C[P[PI[i]]]) +  p[PI[i]];
        }
    }

}
void FlexFlowShop::find_critical_path()
{
    cPath.clear();
    int index = distance(C.begin(),max_element(C.begin(),C.end()));
    while(PH[index] != 0)
    {
        cPath.push_back(index);
        index=PH[index];
    }
    reverse(cPath.begin(),cPath.end());
    stenBlock.resize(cPath.size());
    // Pierwsza operacje w pierwszym bloku oznaczamy 0
    stenBlock[0]=0;
    // Może pętla bez -1
    for(unsigned int i=1; i<cPath.size()-1;++i)
    {
        // Jeżeli poprzednikiem jest poprzednik technologiczny
        // To wystarczy, bo w tym problemie np. operacja 4 zadania 1
        // musi byc wykonana na innym stanowisku niz operacja 3 zadania 1
        // W przypadku dwóch innych zadań, nigdy ostatnia operacja zadania 1
        // nie bedzie poprzednikiem pierwszej operacji zadania 2
        if((cPath[i-1] == P[cPath[i]]) )
        {
            stenBlock[i] =  1;
            stenBlock[i-1]= 2;
        }
        else
        {
            stenBlock[i]=0;
        }
    }
    // Moze pętla bez -1
    // Oznaczenie blokow krytycznych z pojedynczym elementem liczba 3
    for(unsigned int i=1; i<stenBlock.size()-1;++i)
    {
        if((stenBlock[i-1]==2 || stenBlock[i-1]==3)&& (stenBlock[i]==2))
            stenBlock[i] =3;
    }

}

bool FlexFlowShop::is_perm_valid()
{

    int perm_oper_m=0, perm_oper_2_m=0;
    int perm_oper_PI_index=0, perm_oper_2_PI_index=0;
    for(vector<permitted_pos>::iterator it2 = perm_list.begin();
        it2 != perm_list.end() ; ++it2)
    {
        // Pozycja w wektorze PI operacji z listy zabronien
        perm_oper_m = index_m((*it2).x1);
        perm_oper_2_m = index_m((*it2).x2);
        // Jesli są na tej samej maszynie, która jest w liście zakazów
        if( (perm_oper_m == perm_oper_2_m) && (perm_oper_m==(*it2).index_m) )
        {
            perm_oper_PI_index=distance(PI.begin(),find(PI.begin(),PI.end(),(*it2).x1));
            perm_oper_2_PI_index=distance(PI.begin(),find(PI.begin(),PI.end(),(*it2).x2));
            if(perm_oper_PI_index<=perm_oper_2_PI_index)
            {
                return 0;
            }
        }
    }
    // Permutacja dozwolona
    return 1;
}


void FlexFlowShop::mix_critical_oper()
{
    mix_struct oper_info;
    vector<int>::iterator oper_PI_index ;
    vector<int>::iterator oper_2_PI_index ;
    int j=0;
    int oper_st=0;         // nr stanowiska operacji
    int left_limit=0;      // do iteracji w obrębie tego samego stanowiska
    int right_limit=0;

    mix_Cmax.clear();

    // Dla wszystkich operacji w ścieżce krytycznej
    for(unsigned int i=0; i<cPath.size();++i)
    {
        // Przemieszenie operacji (w obrębie tego samego stanowiska):
        // * Dla bloków z jedną operacja nie robimy nic
        if(stenBlock[i] != 3)
        {
            // Index operacji krytycznej w wektorze PI
            oper_PI_index =find(PI.begin(),PI.end(),cPath[i]);
            if((*oper_PI_index) != cPath[i])
                printf("Blad w wyszukaniu elementu krytycznego w wektorze PI");

            // * Nie zamieniamy operacji wewnątrz jednego bloku, Cmax się nie zmieni
            //   ale przed blok lub za blok operacje można przerzucić
            // 1. Przemieszczenie na lewo w wektorze permutacji ( na tej samej maszynie)
            //      * z wyjątkiem pierwszego bloku
            //      * z wyjątkiem pierwszej operacji w bloku
            if(stenBlock[i] !=1)
            {
                j=1;
                while( (*(oper_PI_index - j)) != 0)
                {
                    // Zapisanie informacji
                    oper_info.type_of_move=1;
                    oper_info.oper_nr = *(oper_PI_index);     // nr. operacji krytycznej
                    oper_info.distance_to_oper_2 = -j;
                    // Wykonanie ruchu
                    oper_PI_index=PI.insert(oper_PI_index-j,oper_info.oper_nr);
                    oper_PI_index=PI.erase(oper_PI_index+j+1);
                    // Sprawdzenie czy ruch dozwolony
                    if(is_perm_valid())
                    {
                        // Wyznaczenie momentów zakończeń dla nowej permutacji
                        Calc_C();
                        // Wyliczenie Cmax
                        oper_info.Cmax = *max_element(C.begin(),C.end());
                        // Dodanie ruchu do listy możliwych
                        mix_Cmax.push_back(oper_info);
                    }
                    // Powrót do orginalnej permutacji
                    oper_PI_index=PI.erase(oper_PI_index-j-1);
                    oper_PI_index=PI.insert(oper_PI_index+j, oper_info.oper_nr);

                    ++j;
                }
            }
            // 2. Przemieszczenie na prawo w wektorze permutacji (na tej samej maszynie)
            //      * z wyjątkiem ostatniej operacji w bloku
            //      * z wyjątkiem ostatniego bloku
            if(stenBlock[i]!=2)
            {
                j=1;
                while( (*(oper_PI_index + j)) != 0)
                {

                    // Zapisanie informacji
                    oper_info.type_of_move=2;
                    oper_info.oper_nr = *(oper_PI_index);     // nr. operacji krytycznej
                    oper_info.distance_to_oper_2 = j;
                    // Wykonanie ruchu
                    oper_PI_index=PI.insert(oper_PI_index+j+1,oper_info.oper_nr);
                    oper_PI_index=PI.erase(oper_PI_index-j-1);
                    // Sprawdzenie czy ruch dozwolony
                    if(is_perm_valid())
                    {
                        // Wyznaczenie momentów zakończeń dla nowej permutacji
                        Calc_C();
                        // Wyliczenie Cmax
                        oper_info.Cmax = *max_element(C.begin(),C.end());
                        // Dodanie ruchu do listy możliwych
                        mix_Cmax.push_back(oper_info);
                    }
                    // Powrót do orginalnej permutacji
                    oper_PI_index=PI.erase(oper_PI_index+j);
                    oper_PI_index=PI.insert(oper_PI_index-j, oper_info.oper_nr);
                    ++j;
                }
            }

            // 3. Na inną maszynę (w obrębie tego samego stanowiska)
            oper_st = (distance(PI.begin(),oper_PI_index))/(ms+n) +1;  // numer stanowiska akt. operacji krytycznej
            left_limit = (oper_st-1)*(n+ms);    // granice w obrębie tego samego stanowiska
            right_limit= (oper_st)*(n+ms);
            j=1;
            // Maszyny po lewej (w wektorze permutacji PI)
            while((*(oper_PI_index - j)) != 0) ++j; // Przesunięcie na kolejną maszynę
            while((distance(PI.begin(),oper_PI_index)-j) > (left_limit))
            {

                // Zapisanie informacji
                oper_info.oper_nr = *(oper_PI_index);     // nr. operacji krytycznej
                oper_info.distance_to_oper_2 = -j; // nr. operacji, z którą zamieniamy
                oper_info.type_of_move=3;

                // Wykonanie ruchu
                oper_PI_index=PI.insert(oper_PI_index-j,oper_info.oper_nr);
                oper_PI_index=PI.erase(oper_PI_index+j+1);
                // Sprawdzenie czy ruch dozwolony
                if(is_perm_valid())
                {
                    // Wyznaczenie momentów zakończeń dla nowej permutacji
                    Calc_C();
                    // Wyliczenie Cmax
                    oper_info.Cmax = *max_element(C.begin(),C.end());
                    // Dodanie ruchu do listy możliwych
                    mix_Cmax.push_back(oper_info);
                }
                // Powrót do orginalnej permutacji
                oper_PI_index=PI.erase(oper_PI_index-j-1);
                oper_PI_index=PI.insert(oper_PI_index+j, oper_info.oper_nr);
                ++j;
            }
            // Maszyny po prawej (w wektorze permutacji PI) ale wstawiamy element przed operacje
            j=1;

            while((*(oper_PI_index + j))!= 0) ++j; // Przesunięcie na kolejną maszynę
            while((distance(PI.begin(),oper_PI_index)+j) < (right_limit))
            {

                // Zapisanie informacji
                oper_info.oper_nr = *(oper_PI_index);     // nr. operacji krytycznej
                oper_info.distance_to_oper_2 = j; // nr. operacji, z którą zamieniamy
                oper_info.type_of_move=4;

                // Wykonanie ruchu
                oper_PI_index=PI.insert(oper_PI_index+j+1,oper_info.oper_nr);
                oper_PI_index=PI.erase(oper_PI_index-j-1);
                // Sprawdzenie czy ruch dozwolony
                if(is_perm_valid())
                {
                    // Wyznaczenie momentów zakończeń dla nowej permutacji
                    Calc_C();
                    // Wyliczenie Cmax
                    oper_info.Cmax = *max_element(C.begin(),C.end());
                    // Dodanie ruchu do listy możliwych
                    mix_Cmax.push_back(oper_info);
                }
                // Powrót do orginalnej permutacji
                oper_PI_index=PI.erase(oper_PI_index+j);
                oper_PI_index=PI.insert(oper_PI_index-j, oper_info.oper_nr);
                ++j;
            }
        }
    }
}



bool FlexFlowShop::find_best_move()
{

    // Indeksy operacji w wektorze permutacji PI
    int oper_PI_index;
    int oper_2_PI_index;

    int Cmax_tmp=INT32_MAX;               // wartosc tymczasowa Cmax
    vector<mix_struct>::iterator it_pos;  // pozycja najlepszego mozliwego ruchu
    permitted_pos el; // struktura do dodawania do listy zabronień


    // Wyszukanie najlepszego z dozwolonych ruchów
    for(vector<mix_struct>::iterator it = mix_Cmax.begin();
        it != mix_Cmax.end() ; ++it)
    {
        if((*it).Cmax < Cmax_tmp)
        {
           // Zapamiętanie najlepszego ruchu
           Cmax_tmp = (*it).Cmax;
           it_pos=it;
        }
    }



    // Wykonanie najlepszego ruchu, jeśli znaleziono jakikolwiek
    if(Cmax_tmp != INT32_MAX)
    {
        oper_PI_index =distance(PI.begin(),find(PI.begin(),PI.end(),(*it_pos).oper_nr));
        oper_2_PI_index = oper_PI_index+(*it_pos).distance_to_oper_2;

        // Zabronienie na maszynie aktualnej, przed wykonaniem ruchu
        // np. w przypadku zmiany maszyny operacja nie będzie już mogła wrócić
        el.index_m = index_m((*it_pos).oper_nr);


        switch((*it_pos).type_of_move)
        {
        // Operacja na lewo
        case 1:
            // Dodanie do listy zabronien
            el.x1=PI[oper_PI_index-1]; el.x2=(*it_pos).oper_nr;
            perm_list.insert(perm_list.begin(),el);

            PI.insert(PI.begin()+oper_2_PI_index,(*it_pos).oper_nr);
            PI.erase(PI.begin()+oper_PI_index+1);
            break;
//         Operacja na prawobest
        case 2:
            // Dodanie do listy zabronien
            el.x1=(*it_pos).oper_nr; el.x2=PI[oper_PI_index+1];
            perm_list.insert(perm_list.begin(),el);

            PI.insert(PI.begin()+oper_2_PI_index+1, (*it_pos).oper_nr);
            PI.erase(PI.begin()+oper_PI_index);
            break;
            // Zmiana maszyny na lewo
        case 3:
            // Tutaj dodać PI[oper_PI_index-1]!=0  ?
            el.x1=PI[oper_PI_index-1]; el.x2=(*it_pos).oper_nr;
            perm_list.insert(perm_list.begin(),el);
            // Tutaj dodać PI[oper_PI_index+1]!=0  ?
            el.x1=(*it_pos).oper_nr; el.x2=PI[oper_PI_index+1];
            perm_list.insert(perm_list.begin(),el);

            PI.insert(PI.begin()+oper_2_PI_index,(*it_pos).oper_nr);
            PI.erase(PI.begin()+oper_PI_index+1);
            break;
            // Zmiana maszyny na prawo
        case 4:
            // Tutaj dodać PI[oper_PI_index-1]!=0  ?
            el.x1=PI[oper_PI_index-1]; el.x2=(*it_pos).oper_nr;
            perm_list.insert(perm_list.begin(),el);
            // Tutaj dodać PI[oper_PI_index+1]!=0  ?
            el.x1=(*it_pos).oper_nr; el.x2=PI[oper_PI_index+1];
            perm_list.insert(perm_list.begin(),el);

            PI.insert(PI.begin()+oper_2_PI_index+1, (*it_pos).oper_nr);
            PI.erase(PI.begin()+oper_PI_index);
            break;
        }
    }
    // Lista możliwych ruchów pusta zakończ działanie
    else
    {
        return 1;
    }

    // Ograniczenie rozmiaru listy zabronien
    while(perm_list.size() >= SIZE_PERM_LIST)
        perm_list.pop_back();

    return 0;
}
// * DODAC ILOŚC ITERACJI
bool FlexFlowShop::find_best_perm(string path,int iter)
{
    int i=0;
    vector<int> Cmax_tab;
    best_Cmax=INT32_MAX;
    // Wczytanie danych
    if(Init_FlexFlowShop(path))
        return 1;
    do
    {
        Calc_C();              // Obliczenie momentów zakończenia zadań
        max_C();               // Wyznaczenie Cmax
        Cmax_tab.push_back(Cmax);
        // zapamiętanie najlepszej permutacji, momentów zakończenia, Cmax
        if(Cmax<best_Cmax)
        {
            best_Cmax = Cmax;
            best_C = C;
            best_PI= PI;
        }
        //* DODAC ZAPAMIETANIE NAJLEPSZEGO CMAX I NAJLEPSZEJ PERMUTACJI !
        find_critical_path();  // Wyznaczenie ścieżki krytycznej
        // Wyznaczenie wszystkich możliwych permutacji na podstawie ścieżki krytycznej
        mix_critical_oper();
        // Modyfikacja permatuacji oryginalej do najlepszej znalezionej
        // *Zmienić aby znajdowało możliwie najlepszy ruch, niekoniecznie lepszy Cmax niż mamy,
        // młot trzeba uniesc wyzej zeby niżej potem wpadł. I dodać w środku mechanizm sprawdzający
        // listę zabronien(poprawność permutacji z listą zabronien)
        // *Uzupełnienie listy zabronien
        find_best_move();
        ++i;
    } while (i<iter);


    // Zapisywać najlepsze Cmaxy
    for(int& num : Cmax_tab) cout << num << " ";
    cout << endl;
    cout<< *min_element(Cmax_tab.begin(),Cmax_tab.end())<< endl;
    return 0;
}
