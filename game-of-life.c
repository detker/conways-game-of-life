/**
 * "Gra w Życie"
 * https://en.wikipedia.org/wiki/Conway%27s_Game_of_Life
 * 
 * autor: Wojciech Krolikowski <wk450245@students.mimuw.edu.pl>
 * wersja: 1.0
 * data: 2023-01-10
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>

// Stale definiujace wymiar okna.
#define WIERSZE 22
#define KOLUMNY 80

// Funkcja znajdujaca okreslony element w posorotwanej tablicy jedno/dwuwymiarowej
// ze wzgledu na wiersze (kierunek przeszukiwania -> poziomy).
int binSearch(int tab[], int n, int x)
{
    int l = 1;
    int p = n-1;

    while(l < p)
    {
        int s = (l+p)/2;
        if(tab[s] >= x) p = s;
        else l = s+1;
    }
    if(tab[l] == x) return -1;
    else return l;
}

// Funkcja znajdujaca okreslony element w posorotwanej tablicy dwuwymiarowej
// ze wzgledu na kolumny (kierunek przeszukiwania -> pionowy).
int binSearch2(int **tab, int n, int x)
{
    int l = 0;
    int p = n-1;

    while(l < p)
    {
        int s = (l+p)/2;
        if(tab[s][0] >= x) p = s;
        else l = s+1;
    }
    if(tab[l][0] == x) return l;
    else return -1;
}

// Funkcja alokujaca dodatkowa pamiec dla tablicy jednowymiarowej.
void dodajSlot(int **wspolrzedneDlugosc, int *wspolrzedneDlugoscN, int ileDodac)
{
    int *wynik = *wspolrzedneDlugosc;
    int n = *wspolrzedneDlugoscN;
    
    int *tmp = realloc(wynik, sizeof(wynik) * (size_t)(n+ileDodac));
    if(tmp) wynik = tmp;
    
    *wspolrzedneDlugosc = wynik;
    *wspolrzedneDlugoscN = n+ileDodac;
}

// Funkcja alokujaca dodatkowa pamiec dla wierszy tablicy dwuwymiarowej.
void dodajWiersze(int ***wspolrzedne, int *wiersze, int kolumny, int ileDodac)
{
    int **wynik = *wspolrzedne;
    int n = *wiersze;
    
    int **tmp = realloc(wynik, sizeof(*wynik) * (size_t)(n + ileDodac));
    if(tmp)
    {
        wynik = tmp;
        for(int i = 0; i < ileDodac; i++)
        {
            wynik[n + i] = malloc(sizeof(*wynik[n + i]) * (size_t)kolumny);
        }
    }

    *wspolrzedne = wynik;
    *wiersze = n+ileDodac;
}

// Funkcja alokujaca dodatkowa pamiec dla kolumn tablicy dwuwymiarowej.
void dodajKolumny(int ***wspolrzedne, int *kolumny, int wiersze, int ileDodac)
{
    int **wynik = *wspolrzedne;
    int n = *kolumny;

    for(int i = 0; i < wiersze; i++)
    {
        int *tmp = realloc(wynik[i], sizeof(*wynik[i]) * (size_t)(n + ileDodac));
        if(tmp) wynik[i] = tmp;
    }

    *wspolrzedne = wynik;
    *kolumny = n+ileDodac;
}

// Funkcja odpowiadajaca za uwalnianie pamieci struktur dwuwymiarowych.
void uwolnijPamiec(int ***strukturaT, int dlugosc)
{
    int **struktura = *strukturaT;
    for(int i = 0; i < dlugosc; i++)
    {
        int *akt = struktura[i];
        free(akt);
    }
    free(struktura);
}

// Funkcja wczytujaca opis stanu poczatkowego komorek.
void wczytajWspolrzedne(int ***wspolrzedneT, int *wierszeT, int *kolumnyT, 
                            int **wsporzedneDlugosciT, int *wspolrzedneDlugosciNT)
{
    int **wynikT = *wspolrzedneT;
    int wiersze = *wierszeT;
    int kolumny = *kolumnyT;

    int *dlugosciWierszy = *wsporzedneDlugosciT;
    int dlugosciWierszyN = *wspolrzedneDlugosciNT; 
    
    // Inicjalizacja zmiennych odpowiedzialnych za przechowywanie kolejnych znakow z wejscia.
    int znak, znakNast=0;
    int j=0;
    char liczba[10];
    memset(liczba, 0, sizeof(liczba));

    // Petla wykonujaca sie dopoki nie napotka sie na koniunkcje: znak '/' 
    // oraz wystepujacy od razu po nim '\n'. Oznacza ona koniec opisu stanu poczatkowego.
    for(int i=0; ((znak=getchar()) != '.') && (znak != EOF) && 
        (!((znak == '/') && ((znakNast = getchar()) == '\n'))); i++)
    {
        // Oddaje wczytany nastepny znak z powrotem na wejscie.
        ungetc(znakNast, stdin);
        
        // Jezeli brakuje wiersza - dodaje go.
        if(i == wiersze) dodajWiersze(&wynikT, &wiersze, kolumny, 1);
        
        // Petla wykonuajca sie wewnatrz jednego wiersza opisu danych poczatkowych.
        while(((znak = getchar()) != '\n') && (znak != EOF))
        {
            // Jezeli brakuje wiersza - dodaje go.
            if(i == dlugosciWierszyN) dodajSlot(&dlugosciWierszy, &dlugosciWierszyN, 1);

            // Jezeli brakuje kolumny - dodaje ja.
            if(j == kolumny) dodajKolumny(&wynikT, &kolumny, wiersze, 1);

            // Jezeli petla napotka znak ' ', omija go tj. pobiera kolejny znak, zeruje char *liczba i
            // w razie potrzeby dodaje kolumne do tablicy. Znalezienie ' ' jest rownowazne z
            // pojawieniem sie nowej wspolrzednej w opisie stanu poczatkowego.
            if(znak == ' ')
            {
                znak = getchar();
                memset(liczba, 0, sizeof(liczba));
                j++;
                if(j == kolumny) dodajKolumny(&wynikT, &kolumny, wiersze, 1);
            }

            // Aktualizacja dlugosci wiersza i-tego.
            dlugosciWierszy[i] = j+1;
        
            // Zlaczenie znakow, ktorych nie dzieli znak ' '.
            char znakChar = (char)znak;
            strncat(liczba, &znakChar, 1);

            // Konwersja typu char* na int, zapis gotowej wspolrzednej do i-tego wiersza
            // oraz j-tej kolumny tablicy opisujacej stan poczatkowy.
            int wynik = atoi(liczba);
            wynikT[i][j] = wynik;       
        }

        j = 0;
        memset(liczba, 0, sizeof(liczba));
    }

    for(int i = 0; i < wiersze; i++)
    {
        for(int k = dlugosciWierszy[i]; k < kolumny; k++) wynikT[i][k] = -1;
    }

    *wspolrzedneT = wynikT;
    *wierszeT = wiersze;
    *kolumnyT = kolumny;

    *wsporzedneDlugosciT = dlugosciWierszy;
    *wspolrzedneDlugosciNT = dlugosciWierszyN;
}

// Funkcja zmieniajaca wspolrzedne lewego gornego rogu okna.
void zmienPozycjeOkna(int *pozycjaOknaW, int *pozycjaOknaK, int nowaPozycjaW,
                        int nowaPozycjaK)
{
    *pozycjaOknaW = nowaPozycjaW;
    *pozycjaOknaK = nowaPozycjaK;
}

// Funkcja wyswietlajaca okno wraz z aktualnie zywymi komorkami.
void pokazOkno(int **wspolrzedne, int wiersze, int *wspolrzedneDlugosci,
                int *pozycjaOknaW, int *pozycjaOknaK)
{
    // Jezeli istnieje >0 wspolrzednych.
    if(wspolrzedne != NULL)
    {
        for(int i = (*pozycjaOknaW); i < (*pozycjaOknaW)+WIERSZE; i++)
        {
            int k = binSearch2(wspolrzedne, wiersze, i);
            if(k != -1)
            {
                for(int j = (*pozycjaOknaK); j < (*pozycjaOknaK)+KOLUMNY; j++)
                {
                    if(binSearch(wspolrzedne[k], wspolrzedneDlugosci[k], j) == -1) putchar('0');
                    else putchar('.');
                }
            }
            else
            {
                for(int j = (*pozycjaOknaK); j < (*pozycjaOknaK)+KOLUMNY; j++)
                {
                    putchar('.');
                }
            }
            putchar('\n');
        }
    }
    else
    {
        for(int i = (*pozycjaOknaW); i < (*pozycjaOknaW)+WIERSZE; i++)
        {
            for(int j = (*pozycjaOknaK); j < (*pozycjaOknaK)+KOLUMNY; j++) printf(".");
            printf("\n");
        }
    }

    for(int i = 0; i < KOLUMNY; i++) printf("=");
    printf("\n");
}

// Funkcja sprawdziajaca istnienie sasiadow zywej komorki - to wlasnie tutaj znajdujemy 
// komorki ktore obecnie martwe, w nastepnej generacji, beda zywe.
void sprawdzSasiadow(int **wspolrzedne, int wiersze, int *wspolrzedneDlugosci, int element,
                        int ktoryWiersz, int ***wspolrzedneMartweT, int *wierszeMartweT, 
                        int *kolumnyMartweT, int **dlugosciWierszyMartweT, 
                        int *dlugosciWierszyMartweNT)
{
    int **wspolrzedneMartwe = *wspolrzedneMartweT;
    int wierszeMartwe = *wierszeMartweT;
    int kolumnyMartwe = *kolumnyMartweT;
    
    int *dlugosciWierszyMartwe = *dlugosciWierszyMartweT;
    int dlugosciWierszyMartweN = *dlugosciWierszyMartweNT;

    // Licznik wskazuje ile sasiadow posiada komorka.
    int licznik = 0;
    
    int checkVal;

    // Sprawdzenie jaki znormalizowany indeks wiersza w tablicy trzymajacej obecnie 
    // martwe komorki ma komorka ktora rozpatrujemy.
    int indeks = binSearch2(wspolrzedneMartwe, wierszeMartwe, ktoryWiersz);

    // Osiem par polecen odpowiadajacych kierunkom, po ktorych nalezy szukac sasiadow 
    // komorki. Jezeli sasiad zostanie znaleziony, nastepuje inkrementacja licznika.
    checkVal = binSearch2(wspolrzedne, wiersze, ktoryWiersz-1);
    if((checkVal != -1) && (binSearch(wspolrzedne[checkVal], 
                            wspolrzedneDlugosci[checkVal], element-1) == -1)) licznik++;

    checkVal = binSearch2(wspolrzedne, wiersze, ktoryWiersz-1);
    if((checkVal != -1) && (binSearch(wspolrzedne[checkVal], 
                            wspolrzedneDlugosci[checkVal], element) == -1)) licznik++;

    checkVal = binSearch2(wspolrzedne, wiersze, ktoryWiersz-1);
    if((checkVal != -1) && (binSearch(wspolrzedne[checkVal], 
                            wspolrzedneDlugosci[checkVal], element+1) == -1)) licznik++;
    
    checkVal = binSearch2(wspolrzedne, wiersze, ktoryWiersz);
    if((checkVal != -1)&&(binSearch(wspolrzedne[checkVal], 
                            wspolrzedneDlugosci[checkVal], element-1) == -1)) licznik++;
    
    checkVal = binSearch2(wspolrzedne, wiersze, ktoryWiersz);
    if((checkVal != -1)&&(binSearch(wspolrzedne[checkVal], 
                            wspolrzedneDlugosci[checkVal], element+1) == -1)) licznik++;
    
    checkVal = binSearch2(wspolrzedne, wiersze, ktoryWiersz+1);
    if((checkVal != -1)&&(binSearch(wspolrzedne[checkVal], 
                            wspolrzedneDlugosci[checkVal], element-1) == -1)) licznik++;

    checkVal = binSearch2(wspolrzedne, wiersze, ktoryWiersz+1);
    if((checkVal != -1)&&(binSearch(wspolrzedne[checkVal], 
                            wspolrzedneDlugosci[checkVal], element) == -1)) licznik++;

    checkVal = binSearch2(wspolrzedne, wiersze, ktoryWiersz+1);
    if((checkVal != -1)&&(binSearch(wspolrzedne[checkVal], 
                            wspolrzedneDlugosci[checkVal], element+1) == -1)) licznik++;

    // Jezeli komorka ma dokladnie trzech sasiadow - w nastepnej generacji bedzie zywa.
    // Nastepuje uruchomienie bloku kodu odpowiedzialnego za dodanie jej wspolrzednych
    // do tablicy wspolrzedneMartwe.
    if(licznik == 3)
    {
        // Jezeli brakuje kolumny - dodaje ją.
        if(dlugosciWierszyMartwe[indeks] == kolumnyMartwe)
        {
            int bufor = kolumnyMartwe;
            dodajKolumny(&wspolrzedneMartwe, &kolumnyMartwe, wierszeMartwe, 1);
            for(int i = 0; i < wierszeMartwe; i++) wspolrzedneMartwe[i][bufor] = INT_MAX;
        }
        
        // Jezeli element nie znajduje sie jeszcze w tablicy, dodaje go do niej,
        // pamietajac o zachowaniu kolejnosci rosnacej.
        int xd = binSearch(wspolrzedneMartwe[indeks], dlugosciWierszyMartwe[indeks], 
                            element);
        if((dlugosciWierszyMartwe[indeks] > 1) && (xd != -1) && 
            (element < wspolrzedneMartwe[indeks][dlugosciWierszyMartwe[indeks]-1]))
        {
            int q = dlugosciWierszyMartwe[indeks]-1;
            if(wspolrzedneMartwe[indeks][xd] < element) xd++;

            while(q >= xd)
            {
                wspolrzedneMartwe[indeks][q+1] = wspolrzedneMartwe[indeks][q];
                q--;
            }

            wspolrzedneMartwe[indeks][xd] = element;
            dlugosciWierszyMartwe[indeks]++;
        }
        else if(xd != -1)
        {
            wspolrzedneMartwe[indeks][dlugosciWierszyMartwe[indeks]] = element;
            dlugosciWierszyMartwe[indeks]++;
        }        
    }

    *wspolrzedneMartweT = wspolrzedneMartwe;
    *kolumnyMartweT = kolumnyMartwe;
    *wierszeMartweT = wierszeMartwe;

    *dlugosciWierszyMartweT = dlugosciWierszyMartwe;
    *dlugosciWierszyMartweNT = dlugosciWierszyMartweN;
}

// Funkcja zajmujaca sie obliczaniem nastepnej generacji, w tym zarzadzaniem pamiecia
// oraz tworzeniem nowych struktur oraz ich scalaniem.
void nastepnaGeneracja(int ***wspolrzedneT, int *wierszeT, int *kolumnyT,
                         int **wspolrzedneDlugosciT)
{
    // Jezeli tablica komorek nie jest pusta.
    if(*wspolrzedneT != NULL)
    {
        int **wspolrzedne = *wspolrzedneT;
        int wiersze = *wierszeT;
        int kolumny = *kolumnyT;
        int *wspolrzedneDlugosci = *wspolrzedneDlugosciT;

        // Alokacja tablicy odpowiedzialnej za trzymanie aktualnie zywych komorek,
        // ktore w nastepnej generacji rowniez beda zywe.
        int **wspolrzedneZywe;
        int wierszeZywe = wiersze;
        int kolumnyZywe = 1;
        wspolrzedneZywe = malloc(sizeof(*wspolrzedneZywe) * (size_t)wierszeZywe);
        if(wspolrzedneZywe)
        {
            for(int i = 0; i < wierszeZywe; i++)
            wspolrzedneZywe[i] = malloc(sizeof(*wspolrzedneZywe[i]) * (size_t)kolumnyZywe);
        }

        // Alokowanie tablicy, która zawiera dlugosci wierszy tablicy
        // "wspolrzedneZywe".
        int *dlugosciWierszyZywe;
        int dlugosciWierszyZyweN = wiersze;
        dlugosciWierszyZywe = malloc(sizeof(dlugosciWierszyZywe) * (size_t)dlugosciWierszyZyweN);
        for(int i = 0; i < dlugosciWierszyZyweN; i++) dlugosciWierszyZywe[i] = 1;

        // Alokacja tablicy odpowiedzialnej za trzymanie martwych komorek, ktore w
        // nastepnej generacji beda zywe.
        int **wspolrzedneMartwe;
        int wierszeMartwe = 1;
        int kolumnyMartwe = 1;
        wspolrzedneMartwe = malloc(sizeof(*wspolrzedneMartwe) * (size_t)wierszeMartwe);
        // Alokacja wierszy w nastepujacej konwencji:
        // w tablicy wspolrzedneMartwe umieszczamy wiersze dla wartosci k-1, k, k+1;
        // k = wspolrzedne[i][0], gdzie i=0, 1, 2, ..., wiersze-1.
        if(wspolrzedneMartwe)
        {
            for(int i = 0; i < wierszeMartwe; i++)
            {
                wspolrzedneMartwe[i] = malloc(sizeof(*wspolrzedneMartwe[i]) * (size_t)kolumnyMartwe);
            }
            
            wspolrzedneMartwe[0][0] = wspolrzedne[0][0]-1; 
            
            int j = 1; 
            int indeks = 1;

            while(j < wiersze)
            {
                int odleglosc = wspolrzedne[j][0]-wspolrzedne[j-1][0];
                if(odleglosc == 1)
                {
                    dodajWiersze(&wspolrzedneMartwe, &wierszeMartwe, kolumnyMartwe, 1);
                    wspolrzedneMartwe[indeks][0] = wspolrzedne[j-1][0];
                    indeks++;
                }
                else if(odleglosc == 2)
                {
                    dodajWiersze(&wspolrzedneMartwe, &wierszeMartwe, kolumnyMartwe, 2);
                    wspolrzedneMartwe[indeks][0] = wspolrzedne[j-1][0];
                    indeks++;
                    wspolrzedneMartwe[indeks][0] = wspolrzedne[j-1][0]+1;
                    indeks++;
                }
                else
                {
                    dodajWiersze(&wspolrzedneMartwe, &wierszeMartwe, kolumnyMartwe, 3);
                    wspolrzedneMartwe[indeks][0] = wspolrzedne[j-1][0];
                    indeks++;
                    wspolrzedneMartwe[indeks][0] = wspolrzedne[j-1][0]+1;
                    indeks++;
                    wspolrzedneMartwe[indeks][0] = wspolrzedne[j][0]-1;
                    indeks++;
                }

                j++;
            }

            dodajWiersze(&wspolrzedneMartwe, &wierszeMartwe, kolumnyMartwe, 2);
            wspolrzedneMartwe[indeks][0] = wspolrzedne[j-1][0];
            indeks++;
            wspolrzedneMartwe[indeks][0] = wspolrzedne[j-1][0]+1;
        }

        // Alokowanie tablicy, która zawiera dlugosci wierszy tablicy
        // "wspolrzedneMartwe".
        int *dlugosciWierszyMartwe;
        int dlugosciWierszeMartweN = wierszeMartwe;
        dlugosciWierszyMartwe = malloc(sizeof(dlugosciWierszyMartwe) * (size_t)dlugosciWierszeMartweN);
        for(int i = 0; i < dlugosciWierszeMartweN; i++) dlugosciWierszyMartwe[i] = 1;
        
        // Licznik wskazuje ile sasiadow posiada komorka.
        int licznik = 0;

        // Petle przechodzace po kazdej aktualnie zywej komorce.
        for(int j = 1; j <= kolumny; j++)
        {
            for(int i = 0; i < wiersze; i++)
            {
                if(j < wspolrzedneDlugosci[i])
                {
                    int ktoryWiersz = wspolrzedne[i][0];
                    int element = wspolrzedne[i][j];

                    // Jezeli brakuje kolumny - dodaje ją.
                    if(dlugosciWierszyZywe[i] == kolumnyZywe)
                    {
                        dodajKolumny(&wspolrzedneZywe, &kolumnyZywe, wierszeZywe, 1);
                    }

                    // Osiem par polecen odpowiadajacych kierunkom, po ktorych
                    // nalezy szukac sasiadow komorki. Jezeli sasiad zostanie znaleziony,
                    // nastepuje inkrementacja licznika. Jezeli nie znaleziono sasiada w
                    // danym kierunku nastepuje wywolanie funkcji sprawdzSasiadow na
                    // wspolrzednych na ktorych nie znaleziono sasiada.
                    if((j >= 1) && (i > 0) && (wspolrzedne[i-1][0] == wspolrzedne[i][0]-1) &&
                        (binSearch(wspolrzedne[i-1], wspolrzedneDlugosci[i-1], element-1) == -1))
                            licznik++;
                    else sprawdzSasiadow(wspolrzedne, wiersze, wspolrzedneDlugosci, 
                                            element-1, ktoryWiersz-1, &wspolrzedneMartwe,
                                            &wierszeMartwe, &kolumnyMartwe,
                                            &dlugosciWierszyMartwe, &dlugosciWierszeMartweN);

                    if((j > 1) && (wspolrzedne[i][j-1] == element-1)) licznik++;
                    else sprawdzSasiadow(wspolrzedne, wiersze, wspolrzedneDlugosci, 
                                            element-1, ktoryWiersz, &wspolrzedneMartwe, 
                                            &wierszeMartwe, &kolumnyMartwe, 
                                            &dlugosciWierszyMartwe, &dlugosciWierszeMartweN);

                    if((j >= 1) && (i < wiersze-1) && (wspolrzedne[i+1][0] == wspolrzedne[i][0]+1) &&
                        (binSearch(wspolrzedne[i+1], wspolrzedneDlugosci[i+1], element-1) == -1)) 
                            licznik++; 
                    else sprawdzSasiadow(wspolrzedne, wiersze, wspolrzedneDlugosci,
                                            element-1, ktoryWiersz+1, &wspolrzedneMartwe, 
                                            &wierszeMartwe, &kolumnyMartwe, 
                                            &dlugosciWierszyMartwe, &dlugosciWierszeMartweN);

                    if((i > 0) && (wspolrzedne[i-1][0] == wspolrzedne[i][0]-1) && 
                        (binSearch(wspolrzedne[i-1], wspolrzedneDlugosci[i-1], element) == -1)) 
                            licznik++;
                    else sprawdzSasiadow(wspolrzedne, wiersze, wspolrzedneDlugosci, 
                                            element, ktoryWiersz-1, &wspolrzedneMartwe, 
                                            &wierszeMartwe, &kolumnyMartwe, 
                                            &dlugosciWierszyMartwe, &dlugosciWierszeMartweN);

                    if((i < wiersze-1) && (wspolrzedne[i+1][0] == wspolrzedne[i][0]+1) && 
                        (binSearch(wspolrzedne[i+1], wspolrzedneDlugosci[i+1], element) == -1)) 
                            licznik++;
                    else sprawdzSasiadow(wspolrzedne, wiersze, wspolrzedneDlugosci, 
                                            element, ktoryWiersz+1, &wspolrzedneMartwe, 
                                            &wierszeMartwe, &kolumnyMartwe, 
                                            &dlugosciWierszyMartwe, &dlugosciWierszeMartweN);

                    if((j <= wspolrzedneDlugosci[i]-1) && (i > 0) && 
                        (wspolrzedne[i-1][0] == wspolrzedne[i][0]-1) && 
                            (binSearch(wspolrzedne[i-1], wspolrzedneDlugosci[i-1], element+1) == -1))
                                licznik++;
                    else sprawdzSasiadow(wspolrzedne, wiersze, wspolrzedneDlugosci, 
                                            element+1, ktoryWiersz-1, &wspolrzedneMartwe, 
                                            &wierszeMartwe, &kolumnyMartwe, 
                                            &dlugosciWierszyMartwe, &dlugosciWierszeMartweN);

                    if((j < wspolrzedneDlugosci[i]-1) && (wspolrzedne[i][j+1] == element+1)) 
                        licznik++;
                    else sprawdzSasiadow(wspolrzedne, wiersze, wspolrzedneDlugosci, 
                                            element+1, ktoryWiersz, &wspolrzedneMartwe, 
                                            &wierszeMartwe, &kolumnyMartwe, 
                                            &dlugosciWierszyMartwe, &dlugosciWierszeMartweN);

                    if((j <= wspolrzedneDlugosci[i]-1) && (i < wiersze-1) && 
                        (wspolrzedne[i+1][0] == wspolrzedne[i][0]+1) && 
                            (binSearch(wspolrzedne[i+1], wspolrzedneDlugosci[i+1], element+1) == -1))
                                licznik++; 
                    else sprawdzSasiadow(wspolrzedne, wiersze, wspolrzedneDlugosci, 
                                            element+1, ktoryWiersz+1, &wspolrzedneMartwe, 
                                            &wierszeMartwe, &kolumnyMartwe, 
                                            &dlugosciWierszyMartwe, &dlugosciWierszeMartweN);

                    // Jezeli komorka ma dokladnie 2 lub 3 sasiadow to w nastepnej
                    // generacji bedzie rowniez zywa. Nastepuje uruchomienie bloku kodu
                    // odpowiedzialnego za dodanie wspolrzednych komorki do tablicy 
                    // wspolrzedneZywe oraz aktualizacja dlugosci wiersza i-tego.
                    if((licznik == 2) || (licznik == 3)) 
                    {   
                        wspolrzedneZywe[i][dlugosciWierszyZywe[i]] = element;
                        dlugosciWierszyZywe[i]++;
                    }
                    
                    licznik = 0;
                    wspolrzedneZywe[i][0] = ktoryWiersz;
                }
            }
        }

        // Zniszczenie starej tablicy ze wspolrzednymi.
        uwolnijPamiec(&wspolrzedne, wiersze);
        free(wspolrzedneDlugosci);

        // Redukcja tablicy wspolrzedneZywe - usuniecie wierszy w ktorych nie ma ani
        // jednego elementu.
        for(int i = 0; i < wierszeZywe; i++)
        {
            if(dlugosciWierszyZywe[i] == 1)
            {
                int wierszDoUsuniecia = i;
                
                for(int k = wierszDoUsuniecia; k < dlugosciWierszyZyweN-1; k++)
                {
                    dlugosciWierszyZywe[k] = dlugosciWierszyZywe[k+1];
                }
                dlugosciWierszyZywe = realloc(dlugosciWierszyZywe, 
                    sizeof(dlugosciWierszyZywe) * (size_t)(dlugosciWierszyZyweN-1));
                dlugosciWierszyZyweN--;

                free(wspolrzedneZywe[wierszDoUsuniecia]);
                int **tmp1 = malloc(sizeof(*tmp1) * (size_t)(wierszeZywe-1));
                int tmpI1 = 0;
                for(int j = 0; j < wierszeZywe; j++)
                {
                    if(j != wierszDoUsuniecia) tmp1[tmpI1++] = wspolrzedneZywe[j];
                }
                free(wspolrzedneZywe);
                wspolrzedneZywe = tmp1;
                wierszeZywe--;

                i--;
            }
        }

        // Redukcja tablicy wspolrzedneMartwe - usuniecie wierszy w ktorych nie ma ani
        // jednego elementu.
        for(int i = 0; i < wierszeMartwe; i++)
        {
            if(dlugosciWierszyMartwe[i] == 1)
            {
                int wierszDoUsuniecia = i;
                
                for(int k = wierszDoUsuniecia; k < dlugosciWierszeMartweN-1; k++)
                {
                    dlugosciWierszyMartwe[k] = dlugosciWierszyMartwe[k+1];
                }
                dlugosciWierszyMartwe = realloc(dlugosciWierszyMartwe, 
                    sizeof(dlugosciWierszyMartwe) * (size_t)(dlugosciWierszeMartweN-1));
                dlugosciWierszeMartweN--;

                free(wspolrzedneMartwe[wierszDoUsuniecia]);
                int **tmp = malloc(sizeof(*tmp) * (size_t)(wierszeMartwe-1));
                int tmpI = 0;
                for(int j = 0; j < wierszeMartwe; j++)
                {
                    if(j != wierszDoUsuniecia) tmp[tmpI++] = wspolrzedneMartwe[j];
                }
                free(wspolrzedneMartwe);
                wspolrzedneMartwe = tmp;
                wierszeMartwe--;

                i--;
            }
        }

        // Jezeli w wyniku redukcji obie tablice sa puste.
        if((dlugosciWierszeMartweN == 0) && (dlugosciWierszyZyweN == 0))
        {
            uwolnijPamiec(&wspolrzedneMartwe, wierszeMartwe);
            free(dlugosciWierszyMartwe);
            uwolnijPamiec(&wspolrzedneZywe, wierszeZywe);
            free(dlugosciWierszyZywe);

            *wspolrzedneT = NULL;
            *wierszeT = 0;
            *wspolrzedneDlugosciT = 0;
        }
        // Jezeli w wyniku redukcji jedynie tablica wspolrzedneMartwe jest pusta.
        else if(dlugosciWierszeMartweN == 0)
        {
            uwolnijPamiec(&wspolrzedneMartwe, wierszeMartwe);
            free(dlugosciWierszyMartwe);

            *wspolrzedneT = wspolrzedneZywe;
            *wierszeT = wierszeZywe;
            *wspolrzedneDlugosciT = dlugosciWierszyZywe;
        }
        // Jezeli w wyniku redukcji jedynie tablica wspolrzedneZywe jest pusta.
        else if(dlugosciWierszyZyweN == 0)
        {
            uwolnijPamiec(&wspolrzedneZywe, wierszeZywe);
            free(dlugosciWierszyZywe);

            *wspolrzedneT = wspolrzedneMartwe;
            *wierszeT = wierszeMartwe;
            *wspolrzedneDlugosciT = dlugosciWierszyMartwe;
        }
        // Jezeli obie tablice zawieraja co najmniej wspolrzedna jednej komorki.
        else
        {
            // Alokacja tablicy odpowiedzialnej za trzymanie obu scalonych tablic.
            int **wspolrzedneNowe;
            int wierszeNowe = 1;
            int kolumnyNowe = 1;
            wspolrzedneNowe = malloc(sizeof(*wspolrzedneNowe) * (size_t)wierszeNowe);
            if(wspolrzedneNowe)
            {
                for(int i = 0; i < wierszeNowe; i++)
                wspolrzedneNowe[i] = malloc(sizeof(*wspolrzedneNowe[i]) * (size_t)kolumnyNowe);
            }

            // Alokacja tablicy, która przechowuje dane o dlugosci wierszy tablicy "wspolrzedneNowe".
            int *dlugosciWierszyNowe;
            int dlugosciWierszyNoweN = 1;
            dlugosciWierszyNowe = malloc(sizeof(dlugosciWierszyNowe) * (size_t)dlugosciWierszyNoweN);
            
            // Blok kodu scalajacy obie listy w jedna.
            int wn = 0, kn = 1, wz = 0, wm = 0, kz = 1, km = 1;
            while((wz < wierszeZywe) && (wm < wierszeMartwe))
            {
                if(wn == wierszeNowe)
                {
                    dodajWiersze(&wspolrzedneNowe, &wierszeNowe, kolumnyNowe, 1);
                    dodajSlot(&dlugosciWierszyNowe, &dlugosciWierszyNoweN, 1);
                }

                if(wspolrzedneMartwe[wm][0] < wspolrzedneZywe[wz][0])
                {
                    wspolrzedneNowe[wn][0] = wspolrzedneMartwe[wm][0];
                    while(km < dlugosciWierszyMartwe[wm])
                    {
                        if(kn == kolumnyNowe) 
                        {
                            dodajKolumny(&wspolrzedneNowe, &kolumnyNowe, wierszeNowe, 1);
                        }
                        wspolrzedneNowe[wn][kn] = wspolrzedneMartwe[wm][km];
                        kn++;
                        km++;
                        dlugosciWierszyNowe[wn] = kn;
                    }
                    free(wspolrzedneMartwe[wm]);

                    kn = 1;
                    km = 1;
                    
                    wn++;
                    wm++;

                }
                else if(wspolrzedneMartwe[wm][0] > wspolrzedneZywe[wz][0])
                {
                    wspolrzedneNowe[wn][0] = wspolrzedneZywe[wz][0];
                    while(kz < dlugosciWierszyZywe[wz])
                    {
                        if(kn == kolumnyNowe) 
                        {
                            dodajKolumny(&wspolrzedneNowe, &kolumnyNowe, wierszeNowe, 1);
                        }
                        wspolrzedneNowe[wn][kn] = wspolrzedneZywe[wz][kz];
                        kn++;
                        kz++;
                        dlugosciWierszyNowe[wn] = kn;
                    }
                    free(wspolrzedneZywe[wz]);

                    kn = 1;
                    kz = 1;

                    wn++;
                    wz++;

                }
                else if(wspolrzedneMartwe[wm][0] == wspolrzedneZywe[wz][0])
                {
                    while((kz < dlugosciWierszyZywe[wz]) && (km < dlugosciWierszyMartwe[wm]))
                    {
                        wspolrzedneNowe[wn][0] = wspolrzedneMartwe[wm][0];
                        if(kn == kolumnyNowe) 
                        {
                            dodajKolumny(&wspolrzedneNowe, &kolumnyNowe, wierszeNowe, 1);
                        }
                        if(wspolrzedneZywe[wz][kz] < wspolrzedneMartwe[wm][km])
                        {
                            wspolrzedneNowe[wn][kn] = wspolrzedneZywe[wz][kz];
                            kn++;
                            kz++;
                            dlugosciWierszyNowe[wn] = kn;
                        }
                        else if(wspolrzedneZywe[wz][kz] > wspolrzedneMartwe[wm][km])
                        {
                            wspolrzedneNowe[wn][kn] = wspolrzedneMartwe[wm][km];
                            kn++;
                            km++;
                            dlugosciWierszyNowe[wn] = kn;
                        }
                        else
                        {
                            wspolrzedneNowe[wn][kn] = wspolrzedneMartwe[wm][km];
                            kn++;
                            km++;
                            kz++;
                            dlugosciWierszyNowe[wn] = kn;
                        }
                    }
                    
                    // Jezeli pierwszy skonczy sie wiersz tablicy wspolrzedneZywe.
                    if(kz == dlugosciWierszyZywe[wz])
                    {
                        while(km < dlugosciWierszyMartwe[wm])
                        {
                            if(kn == kolumnyNowe)
                            {
                                dodajKolumny(&wspolrzedneNowe, &kolumnyNowe, wierszeNowe, 1);
                            }
                            wspolrzedneNowe[wn][kn] = wspolrzedneMartwe[wm][km];
                            km++;
                            kn++;
                            dlugosciWierszyNowe[wn] = kn;
                        }
                    }

                    // Jezeli pierwszy skonczy sie wiersz tablicy wspolrzedneMartwe.
                    if(km == dlugosciWierszyMartwe[wm])
                    {
                        while(kz < dlugosciWierszyZywe[wz])
                        {
                            if(kn == kolumnyNowe)
                            {
                                dodajKolumny(&wspolrzedneNowe, &kolumnyNowe, wierszeNowe, 1);
                            }
                            wspolrzedneNowe[wn][kn] = wspolrzedneZywe[wz][kz];
                            kz++;
                            kn++;
                            dlugosciWierszyNowe[wn] = kn;
                        }
                    }
                    free(wspolrzedneMartwe[wm]);
                    free(wspolrzedneZywe[wz]);

                    km = 1;
                    kz = 1;
                    kn = 1;

                    wm++;
                    wz++;
                    wn++;
                }
            }

            // Jezeli pierwsza skonczy sie tablica wspolrzedneZywe.
            if(wz == wierszeZywe)
            {
                while(wm < wierszeMartwe)
                {
                    if(wn == wierszeNowe)
                    {
                        dodajWiersze(&wspolrzedneNowe, &wierszeNowe, kolumnyNowe, 1);
                        dodajSlot(&dlugosciWierszyNowe, &dlugosciWierszyNoweN, 1);
                    }

                    wspolrzedneNowe[wn][0] = wspolrzedneMartwe[wm][0];
                    
                    while(km < dlugosciWierszyMartwe[wm])
                    {
                        if(kn == kolumnyNowe)
                        {
                            dodajKolumny(&wspolrzedneNowe, &kolumnyNowe, wierszeNowe, 1);
                        }
                        wspolrzedneNowe[wn][kn] = wspolrzedneMartwe[wm][km];
                        km++;
                        kn++;
                        dlugosciWierszyNowe[wn] = kn;
                    }
                    free(wspolrzedneMartwe[wm]);

                    km = 1;
                    kn = 1;

                    wm++;
                    wn++;
                }
            }

            // Jezeli pierwsza skonczy sie tablica wspolrzedneMartwe.
            if(wm == wierszeMartwe)
            {
                while(wz < wierszeZywe)
                {
                    if(wn == wierszeNowe)
                    {
                        dodajWiersze(&wspolrzedneNowe, &wierszeNowe, kolumnyNowe, 1);
                        dodajSlot(&dlugosciWierszyNowe, &dlugosciWierszyNoweN, 1);
                    }
                    
                    wspolrzedneNowe[wn][0] = wspolrzedneZywe[wz][0];

                    while(kz < dlugosciWierszyZywe[wz])
                    {
                        if(kn == kolumnyNowe)
                        {
                            dodajKolumny(&wspolrzedneNowe, &kolumnyNowe, wierszeNowe, 1);
                        }
                        wspolrzedneNowe[wn][kn] = wspolrzedneZywe[wz][kz];
                        kz++;
                        kn++;
                        dlugosciWierszyNowe[wn] = kn;
                    }
                    free(wspolrzedneZywe[wz]);

                    kz = 1;
                    kn = 1;

                    wz++;
                    wn++;
                }
            }        

            // Uwolnienie pamieci dla tablic wspolrzedneZywe/Martwe.
            // (Uwolnienie wierszy tych tablic nastepowalo powyzej, natychmiast po
            // wyczerpaniu wspolrzednych z tychze wierszy.)
            free(wspolrzedneMartwe);
            free(dlugosciWierszyMartwe);
            free(wspolrzedneZywe);
            free(dlugosciWierszyZywe);

            *wspolrzedneT = wspolrzedneNowe;
            *wierszeT = wierszeNowe;
            *kolumnyT = kolumnyNowe;
            *wspolrzedneDlugosciT = dlugosciWierszyNowe;
        }
    }
}

// Funkcja przekazujaca na wyjscie zrzut aktualnej generacji.
void zrzutAktGeneracji(int **wspolrzedne, int wiersze, int *wspolrzedneDlugosci)
{
    // Jezeli wspolrzednych jest >0.
    if(wspolrzedne != NULL)
    {
        for(int i = 0; i < wiersze; i++)
        {
            if(wspolrzedneDlugosci[i] > 1) putchar('/');
            for(int j = 0; j < wspolrzedneDlugosci[i]; j++)
            {
                if(j != wspolrzedneDlugosci[i]-1) printf("%d ", wspolrzedne[i][j]);
                else printf("%d", wspolrzedne[i][j]);
            }
            if(wspolrzedneDlugosci[i] > 1) putchar('\n');
        }
    }
    putchar('/');
    putchar('\n');
}

int main(void)
{
    // Dynamiczne alokowanie tablicy, odpowiedzialnej za przechowywanie 
    // wspolrzednych komorek.
    int **wspolrzedne;
    int wiersze = 1;
    int kolumny = 1;
    wspolrzedne = malloc(sizeof(*wspolrzedne) * (size_t)wiersze);
    if(wspolrzedne)
    {
        for(int i = 0; i < wiersze; i++)
        wspolrzedne[i] = malloc(sizeof(*wspolrzedne[i]) * (size_t)kolumny);
    }

    // Dynamiczne alokowanie tablicy, która zawiera dlugosci wierszy tablicy
    // "wspolrzedne".
    int *wspolrzedneDlugosci;
    int wspolrzedneDlugosciN = 1;
    wspolrzedneDlugosci = malloc(sizeof(wspolrzedneDlugosci) * (size_t)wspolrzedneDlugosciN);

    // Zmienne przechowujace dane o wspolrzednych okna.
    int pozycjaOknaW = 1;
    int pozycjaOknaK = 1;

    // Wczyt poczatkowego stanu komorek do tablicy dynamicznej.
    wczytajWspolrzedne(&wspolrzedne, &wiersze, &kolumny, 
                        &wspolrzedneDlugosci, &wspolrzedneDlugosciN);

    // Oddanie na wyjscie okna z obecnym stanem komorek.
    pokazOkno(wspolrzedne, wiersze, wspolrzedneDlugosci, &pozycjaOknaW, &pozycjaOknaK);

    // Inicjalizacja zmiennych odpowiedzialnych za wczyt i obsluge polecen z wejscia.
    char polecenieTresc[24];
    int polecenie[2];
    char separator[] = " "; 
    int c;

    // Pelta wykonujaca sie az nie napotka znaku '.' - konczacego sekwencje
    // wydawania polecen.
    while((c = getchar()) != '.')
    {
        // Jezeli znak okaze sie byc '\n' - oblicz nastepna generacje.
        if(c == '\n')
        {
            nastepnaGeneracja(&wspolrzedne, &wiersze, &kolumny, &wspolrzedneDlugosci);
            pokazOkno(wspolrzedne, wiersze, wspolrzedneDlugosci, 
                        &pozycjaOknaW, &pozycjaOknaK);
        }
        else
        {
            // Przyjmujemy konwencje, iz INT_MAX oznacza wartosc niezainicjalizowana.
            polecenie[1] = INT_MAX;
            int j = 0;
            ungetc(c, stdin);
            // Wczyt napisu do zmiennej polecenieTresc.
            fgets(polecenieTresc, 24, stdin);

            // Podzielenie napisu ze wzgledu na separator - " ".
            char *ptr = strtok(polecenieTresc, separator);

            // Zapis podzielonego napisu do nastepnych slotow tablicy polecenie.
            while(ptr != NULL)
            {
                polecenie[j] = atoi(ptr);
                j++;
                ptr = strtok(NULL, separator);
            }
            // Jezeli poleceniem bylo 0 to zostanie zrzucony aktualny stan generacji. 
            if((polecenie[1] == INT_MAX) && (polecenie[0] == 0))
            {
                zrzutAktGeneracji(wspolrzedne, wiersze, wspolrzedneDlugosci);
            }
            // Jezeli poleceniem byla liczba k!=0 - zostanie obliczona k-ta generacja.
            else if(polecenie[1] == INT_MAX)
            {
                for(int i = 0; i < polecenie[0]; i++)
                {
                    nastepnaGeneracja(&wspolrzedne, &wiersze, &kolumny, 
                                        &wspolrzedneDlugosci);
                } 
            }
            // Jezeli poleceniem byla para liczb <x, y> - zmien wspolrzedne lewego gornego
            // rogu okna na x oraz y.
            if(polecenie[1] != INT_MAX)
            {
                zmienPozycjeOkna(&pozycjaOknaW, &pozycjaOknaK, polecenie[0], polecenie[1]);
            }
            
            pokazOkno(wspolrzedne, wiersze, wspolrzedneDlugosci,
                        &pozycjaOknaW, &pozycjaOknaK);
        }
    }

    // Uwalnianie zaalokowanej pamieci przed koncem dzialania programu.
    uwolnijPamiec(&wspolrzedne, wiersze);
    free(wspolrzedneDlugosci);

    return 0;
}