# conway's-game-of-life
my C implementation of conway's game of life using dynamic memory allocation

tested with **valgrind** --leak-check=full -q --error-exitcode=1

no memory leaks detected :)

## specification (in polish lang):

Stworzona przez Johna Conwaya Gra w Życie to automat komórkowy, czyli symulacja świata komórek.

Symulacja jest prowadzona na planszy, składającej się z nieskończenie wielu wierszy i nieskończenie wielu kolumn.

Wiersze i kolumny planszy są numerowane liczbami całkowitymi ze znakiem.

Na każdym polu planszy jest komórka w jednym z dwóch stanów: żywa lub martwa.

Łączny stan wszystkich komórek nazywamy generacją.

Przyjmujemy, że komórka w wierszu w i kolumnie k sąsiaduje na planszy z ośmioma innymi komórkami, które mają numer wiersza od w - 1 do w + 1 a numer kolumny od k - 1 do k + 1.

Symulacja zaczyna się od pewnej generacji początkowej, na podstawie której liczymy następne.

W następnej generacji komórka będzie żywa wtedy i tylko wtedy, gdy:
<ul>
<li>w bieżącej generacji jest żywa i ma dokładnie dwóch lub trzech żywych sąsiadów, albo</li>
<li>w bieżącej generacji jest martwa i ma dokładnie trzech żywych sąsiadów.</li>
</ul>

--------------------------------------------------------------------

Program czyta z wejścia opis generacji początkowej. Następnie, w pętli, pokazuje fragment planszy, po czym czyta i wykonuje polecenie użytkownika.

Polecenia sterują liczeniem kolejnych generacji. Określają też fragment planszy, dalej nazywany oknem, którego zawartość jest pokazywana użytkownikowi.

Program jest parametryzowany dwiema dodatnimi liczbami całkowitymi:
<ul>
<li>WIERSZE to liczba wierszy okna;</li>
<li>KOLUMNY to liczba kolumn okna.</li>
</ul>

Wartości tych parametrów są określone za pomocą stałych symbolicznych, które można zdefiniować opcją -D kompilatora.

W kodzie programu są podane wartości domyślne:
<ul>
<li>WIERSZE ma wartość 22;</li>
<li>KOLUMNY ma wartość 80.</li>
</ul>

Pozycja okna na planszy jest określona przez pozycję jego lewego górnego rogu. Jeśli lewy górny róg okna jest w wierszu w i kolumnie k, to okno obejmuje komórki, które mają numer wiersza od w do w + WIERSZE - 1 i numer kolumny od k do k + KOLUMNY - 1.

Początkowo lewy górny róg okna jest w wierszu numer 1 i kolumnie numer 1.

-------------------------------------------------------------------

Na wejściu programu jest opis generacji początkowej i ciąg poleceń.

Opis generacji wskazuje komórki, które są żywe. Ma postać ciągu wierszy zaczynających się od znaku '/' (slash).

W ostatnim wierszu opisu generacji jest tylko znak '/'. We wszystkich pozostałych wierszach po znaku '/' jest liczba całkowita, będąca numerem wiersza planszy. Po niej jest uporządkowany rosnąco niepusty ciąg liczb całkowitych, będących numerami kolumn. Każda z tych liczb jest poprzedzona jedną spacją.

Wiersz postaci:

**/w k1 .. kn**

informuje, że w wierszu planszy o numerze w żyją komórki w kolumnach k1, .., kn.

W opisie generacji ciąg numerów wierszy, czyli liczb poprzedzonych znakami '/', jest uporządkowany rosnąco.

Po opisie generacji początkowej, na wejściu programu jest ciąg jednowierszowych poleceń.

Program rozpoznaje polecenia:
<ul>
<li>
zakończenia pracy,
Ma postać wiersza ze znakiem '.' (kropka).
Przerywa pętlę czytającą polecenia i kończy pracę programu.
</li>
<li>
obliczenia N-tej kolejnej generacji,
Ma postać wiersza z dodatnią liczbą całkowitą N.
Liczy N-tą kolejną generację, zaczynając od aktualnej. W szczególności, polecenie 1 powoduje obliczenie **następnej generacji**.
</li>
<li>
obliczenia następnej generacji,
Ma postać wiersza pustego. Jest równoważne poleceniu 1.
</li>
<li>
zrzutu stanu aktualnej generacji,
Ma postać wiersza z liczbą 0 (zero).
Generuje kompletny opis aktualnej generacji w takim samym formacie, w jakim był wczytany przez program opis generacji początkowej.
</li>
<li>
przesunięcia okna.
Ma postać wiersza z dwiema liczbami całkowitymi, w oraz k, rozdzielonymi spacją.
Zmienia współrzędne lewego górnego rogu okna, umieszczając go w wierszu w i kolumnie k.
</li>
</ul>

--------------------------------------------------------------------------------

Przed wczytaniem każdego polecenia program pokazuje zawartość okna, zaczynając od lewego górnego rogu.

Zawartość okna jest przedstawiona za pomocą WIERSZE wierszy, z których każdy ma długość KOLUMNY. Po ostatnim z nich następuje wiersz, w którym jest ciąg znaków '=' (równość) o długości KOLUMNY.

Znak w wierszu zawartości okna określa stan komórki. Komórka żywa jest reprezentowana przez znak '0' (zero) a komórka martwa jest reprezentowana przez znak . (kropka).

Compilation and launching:
-------------------------

To **compile**, type command: **gcc @opcje -DWIERSZE=val1 -DKOLUMNY=val2 main.c -o 01**

**opcje** is a gcc configuration file, included in repo.
<br>
**WIERSZE, KOLUMNY** - positive integers;

To **launch**, type command: **./01**

*specification provided by: MIM UW*
