#include "ParallelModel.hpp"
#include <unistd.h>
#include <stack>
#include <sys/wait.h>
#include <signal.h>

using namespace std;


inline void trace(string head, string msg) {
    string s;
    s = string(head + "\t(");
    s += msg;
    s += ")\n";
    cout << s;
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * Metody dla klasy opisującej stan
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/* -------------------------------------------------------------------------
 */
ParallelState::ParallelState(int _id) {
    id = _id;
    abstractClass = -1;
    actions.resize(2);
    edges.resize(2);
    counter = 0;
    accept = false;

    classCounter = 0;

    clean();
}

/* -------------------------------------------------------------------------
 */
int ParallelState::addTransition(int action, int dest) {

    if (counter == actions.size()) {
        int s = 2 * actions.size();
        actions.resize(s);
        edges.resize(s);
    }
    actions[counter] = action;
    edges[counter] = dest;
    counter++;
    return counter;
}

string ParallelState::to_string() {
    string str;
    str += std::to_string(id) + ":" + std::to_string(accept) + ":" + std::to_string(abstractClass) + "(";
    for (int i = 0; i < counter; i++) {
        str += "(" + std::to_string(actions[i]) + "," + std::to_string(edges[i]) + ")";
    }
    str += "):" + std::to_string(currentAction) + "," + std::to_string(opState) + "," + std::to_string(threadId);

    return str;
}

int ParallelState::findAction(int action) {
    // zacznij przeglądanie od początku tabeli akcji
    int i;
    for (i = 0; i < counter; i++) {
        // jeśli bieżąca akcja odpowiada poszukiwanej - przerwij wyszukiwanie
        if (actions[i] == action) {
            return i;
        }
    }
    // jeśli nie udało się znaleźć - zwróć -1
    return -1;
}

/* -------------------------------------------------------------------------
 */
ostream &operator<<(ostream &str, ParallelState &s) {
    str << s.to_string();
    return str;
}


void dumpActionPath(ActionPath p, ostream &str) {
    while (p.size() > 0) {
        str << "->" << p.front();
        p.pop_front();
    }
    str << endl;
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * Metody dla klasy opisującej model
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

/* -------------------------------------------------------------------------
 */

ParallelModel::ParallelModel(int size) {
    counter = size;
    if (size != 0) states.resize(counter);
    else states.resize(2);
    for (int i = 0; i < counter; i++) {
        states[i] = new ParallelState(i);
    }
    // jeden wątek zablokowany na główny
    threadsStarted = 1;
    threadsRunning = 0;
    // ustaw domyślny początkowy rozmiar tablicy wątków
    threadsVector.resize(10);
    threadsVector[0] = NULL;
}

/* -------------------------------------------------------------------------
 */
int ParallelModel::addState(ParallelState *s) {
    // jeśli za mało miejsca to zwiększ
    if (counter == states.size()) {
        int ns = 2 * states.size();
        states.resize(ns);
    }
    // id musi się zgadzać z miejscem
    s->id = counter;
    // dodaj do tablicy
    states[counter] = s;
    counter++;
    return counter;
}

/* -------------------------------------------------------------------------
 */
int ParallelModel::addNewState() {
    ParallelState *s = new ParallelState(counter);
    addState(s);
    return counter;
}

/* -------------------------------------------------------------------------
 */
void ParallelModel::cleanAll() {
    for (int n = 0; n < counter; n++) {
        states[n]->clean();
    }
}

/* -------------------------------------------------------------------------
 * Połącz dwa węzły w klasę abstrakcji - podłącz klasę drugiego do pierwszego
 */
int ParallelModel::unify(int _id1, int _id2) {
    // ustaw - najpierw mniejszy, potem większy
    int id1 = min(_id1, _id2);
    int id2 = max(_id1, _id2);
    // W każdym przypadku przejdź do głowy
    int h1 = findClass(id1);
    int h2 = findClass(id2);

    // jeśli to samo to już nie trzeba łączyć
    if (h1 == h2 && h1 != -1) return h1;

    // jeśli pierwszy jeszcze nie był podpięty nigdzie, zainicjuj
    if (h1 == -1) {
        // przypisz siebie
        states[id1]->abstractClass = id1;
        h1 = id1;
    }
    // to samo z drugim, zainicjuj
    if (h2 == -1) {
        h2 = id2;
        // nie trzeba przypisywać bo zaraz to nastąpi
    }
    // teraz podepnij drugą klasę pod pierwszy
    states[h2]->abstractClass = h1;

    // jako wynik id reprezentanta klasy
    return h1;
}

/* -------------------------------------------------------------------------
 * znajdź węzeł reprezentujący klasę do której należy węzeł
 */
int ParallelModel::findClass(int id) {
    // jeśli węzeł nie należy do żadnej klasy, zwróć -1
    if (states[id]->abstractClass == -1) return -1;
    // w p.p. cofnij się "do głowy"
    while (states[id]->abstractClass != id) id = states[id]->abstractClass;
    return id;
}

/* -------------------------------------------------------------------------
 * Metoda zwracająca akcję związaną z danym stanem.
 * Należy uwzględnić klasę abstrakcji.
 */
int ParallelModel::getAction(ParallelState *s) {

    int currentClass = findClass(s->id);
    if (currentClass == -1) {
        return s->actions[s->currentAction];
    } else {
        return states[currentClass]->actions[states[currentClass]->currentAction];
    }
}

void ParallelModel::setAction(ParallelState *s, int action) {

    // co do akcji to sprawdzamy, czy nie jest to węzeł z jakiejś klasy abstrakcji
    if (s->abstractClass == -1) {
        // jeśli nie, to decydujemy o akcji lokalnie
        s->currentAction = action;
    } else {
        // w p.p. zależymy od "głowy"    // węzeł reprezentant klasy abstrakcji, jeśli jest
        ParallelState *headState = states[findClass(s->id)];

        // teraz, czy w gowie jest ustalona akcja
        if (headState->currentAction == -1) {
            // akcja nie jest określona - zrób to
            headState->currentAction = headState->findAction(action);
            // zabezpieczenie - dla poprawnego modelu zbędne
            if (headState->currentAction == -1) {
                cerr << "model::setAction: requested action not found!" << endl;
                exit(1);
            }
        } else if (s != headState) {
            if (headState->actions[headState->currentAction] != action) {
                // chemy ustawić akcję inną od tej w głowie - zgłośc błąd
                cout << "model::setAction: trying to set an action other than in the head" << endl;
                exit(-1);
            } else {
                // znajdź pierwszą akcję zgodną z tą w głowie (jeśli bieżący stan jest głową to nie ma potrzeby)
                s->currentAction = s->findAction(headState->actions[headState->currentAction]);
            }
        }

        // zwiększamy licznik zajętości dla głowy
        headState->classCounter++;
    }
}

void ParallelModel::clearAction(ParallelState *s) {

    // co do akcji to sprawdzamy, czy nie jest to węzeł z jakiejś klasy abstrakcji
    if (s->abstractClass == -1) {
        // jeśli nie, to decydujemy o akcji lokalnie
        s->currentAction = -1;
    } else {
        // w p.p. zależymy od "głowy"    // węzeł reprezentant klasy abstrakcji, jeśli jest
        ParallelState *headState = states[findClass(s->id)];

        // sprawdź licznik węzłów w głowie
        if (headState->classCounter <= 0) {
            // błąd
            cerr << "ParallelModel::clearAction: head already clean!" << endl;
            exit(1);
        } else {
            // zmniejsz licznik przydziałów w klasie równoważności
            headState->classCounter--;
        }

        // jeśli osiągnięto 0, to cała klasa jest pusta
        if (headState->classCounter == 0) {
            // wyzeruj akcję "na górze"
            headState->currentAction = -1;
        }
        // jeśli bieżący stan nie jest głową, wyzeruj w nim
        if (s != headState) {
            s->currentAction = -1;
        }
    }
}

/* -------------------------------------------------------------------------
 * Przejdź po ścieżce, jeśli jest możliwe i zwróc stan na którym się ona kończy.
 * Używana przy obliczaniu możliwego stopnia zrównoleglenia.
 * Rekurencyjnie przechodzi zadaną ścieżkę i zaznacza zajętość węzłów.
 * Ścieżka musi dawać jednoznaczne przejście, jeśli napotkany zostanie
 * niedeterminizm, zwracany jest błąd.
 * Jeśli trafi na węzeł, który jest już zajęty - zwraca błąd. W takim przypadku
 * węzły na ścieżce nie są oznaczane - model pozostaje niezmieniony.
 */
int ParallelModel::followPath(int s, ActionPath path) {

#ifdef __DEBUG__
    cout << "ParallelModel::followPath(" << s << ",): 1" << endl;
#endif

    // bieżący stan
    ParallelState *currentState = states[s];
    // jeśli bieżący węzeł jest już oznaczony, zwróć błąd
    if (currentState->opState != ParallelState::clear) return -1;

#ifdef __DEBUG__
    cout << "ParallelModel::followPath(" << s << ",): 2: " << path.size() << endl;
#endif
    // jeśli długość ścieżki jest zerowa, zwróć bieżący węzeł
    if (path.size() == 0) {
//         currentState->opState = ParallelState::searching;
        return s;
    }
    // w p.p. znajdź akcję w węźle

    // wymagana akcja
    int currentAction = path.front();
    path.pop_front();

#ifdef __DEBUG__
    cout << "ParallelModel::followPath(" << s << ",): 3: " << currentAction << endl;
#endif

    // indeks w tablicy akcji dla poszukiwanej
    int action = currentState->findAction(currentAction);
    // jeśli coś jest nie tak - zwróć błąd
    if (action == -1) return -1;

#ifdef __DEBUG__
    cout << "ParallelModel::followPath(" << s << ",): 4: " << currentAction << endl;
#endif

    // czy to ostatnia lub niepowtarzająca się akcja
    if (action + 1 == currentState->counter || currentState->actions[action + 1] != currentAction) {

        // spróbuj ustawić akcję w stanie - jak nie wyjdzie to się zatrzyma
        setAction(currentState, currentAction);
        // jest dobrze, oznacz bieżący węzeł
        currentState->opState = ParallelState::searching;

        // przejdź rekurencyjnie dalej
        int result = followPath(currentState->edges[action], path);
        // jeśli jest OK, zwróć ostatni stan
        if (result != -1) {
            return result;
        } else {
            // w p.p. wyzeruj i zwróć błąd
            currentState->opState = ParallelState::clear;
            return -1;
        }
    }

    // zwróć błąd - nie ma wymaganej akcji w bieżącym stanie
    return -1;
}

/* -------------------------------------------------------------------------
 * Funkcja do czyszczenia przebytej przez wątek ścieżki.
 * Używana przy obliczaniu możliwego stopnia zrównoleglenia.
 * Rekurencyjnie przechodzi zadaną ścieżkę i odznacza zajętość węzłów.
 * Jeśli trafi na węzeł, który nie jest zajęty - zwraca błąd. W takim przypadku
 * węzły na ścieżce nie są odznaczane - model pozostaje niezmieniony.
 */
int ParallelModel::resetPath(int s, ActionPath path) {
    // bieżący stan
    ParallelState *currentState = states[s];
    // jeśli bieżący węzeł nie jest oznaczony, zwróć błąd
    if (currentState->opState != ParallelState::searching && path.size() != 0) return -1;

    // jeśli długość ścieżki jest zerowa, zwróć bieżący węzeł
    if (path.size() == 0) {
        currentState->opState = ParallelState::clear;

        return s;
    }

    // w p.p. znajdź akcję w węźle

    // wymagana akcja
    int currentAction = path.front();
    path.pop_front();
    int action = currentState->findAction(currentAction);
    // jeśli coś jest nie tak - zwróć błąd
    if (action == -1) return -1;

    // czy to ostatnia lub niepowtarzająca się akcja
    if (action + 1 == currentState->counter || currentState->actions[action + 1] != currentAction) {
        // jest dobrze, przejdź rekurencyjnie
        int result = resetPath(currentState->edges[action], path);
        // jeśli jest OK, wyzeruj.
        if (result != -1) {
            currentState->opState = ParallelState::clear;
            clearAction(currentState);
            return result;
        } else {
            // w p.p. zwróć błąd
            return -1;
        }
    }

    // zwróć błąd - nie ma wymaganej akcji w bieżącym stanie
    return -1;
}

/* -------------------------------------------------------------------------
 * Funkcja poszukująca węzłów, z których można rozpocząć równolegle poszukiwanie
 * strategii. Za pomocą BFS-a przeszukuje ona graf tak długo, jak długo
 * albo nie znajdzie zadanej liczby węzłów, albo się nie zatka.
 * Algorytm poszukuje prefiksów strategii, dlatego chociaż implementuje BFS,
 * to za każdym razem zapamiętuje prefiks strategii dochodzącej do danego stanu
 * - takich prefiksów moze być wiele.
 * 
 * start - prefix (węzeł) początkowy
 * n - maksymalna liczba prefiksów po osiągnięciu której należy zakończyć
 * prefixes - zbiór znalezionych prefiksów strategii.
 */
int ParallelModel::computePaths(int s, ActionPath start, int n, queue <ActionPath> &prefixes) {
    // kolejka aktywnych prefiksów
    queue <ActionPath> activePrefixes;
    // korzystamy z prefixes jako wyniku

    // pierwszy prefiks do aktywnych
    activePrefixes.push(start);

    // w pętli tak długo jak potrzeba lub się da
    while (prefixes.size() + activePrefixes.size() < n && activePrefixes.size() > 0) {

#ifdef __DEBUG__
        cout << "ParallelModel::computePaths: 1" << endl;
#endif
        // jest na czym pracować - wyjmij z kolejki
        ActionPath currentPrefix = activePrefixes.front();
        activePrefixes.pop();

        // przejdź po ścieżce określonej przez prefix i znajdź docelowy stan
        int cs = followPath(s, currentPrefix);
        // wyczyść
        resetPath(s, currentPrefix);

        // jeśli błąd w chodzeniu po ścieżce - zakręć pętlą
        if (cs == -1) {
            cerr << "ParallelModel::computePaths: 2.1: prefix should not be incorrect" << endl;
            exit(1);
#ifdef __DEBUG__
            } else {
                cout << "ParallelModel::computePaths: 2.2: " << cs << endl;
#endif
        }

        // "alias" dla ułatwienia
        ParallelState *currentState = states[cs];

        // jeśli to jest stan bez następników - zrób trochę inaczej
        if (currentState->counter == 0) {
            // przepisz ten prefiks do wyników
            prefixes.push(currentPrefix);
            // i zakręć ponownie
            continue;
        }


        // jeśli są następniki, przejdź po akcjach
        for (int action = 0; action < currentState->counter; action++) {
#ifdef __DEBUG__
            cout << "ParallelModel::computePaths: 3: "<< action << " : " << currentState->actions[action] << "->" << currentState->edges[action] <<endl;
#endif

            // skonstruuj nowy prefix
            ActionPath np = currentPrefix;
            np.push_back(currentState->actions[action]);

            // teraz zależnie od kolejnej akcji
            if (action == currentState->counter - 1 ||
                currentState->actions[action] != currentState->actions[action + 1]) {
#ifdef __DEBUG__
                cout << "ParallelModel::computePaths: 3.1" << endl;
#endif
                // jest to albo ostatnia akcja albo jest deterministycznie - ta akcja może być dalej rozwijana
                activePrefixes.push(np);
            } else {
#ifdef __DEBUG__
                cout << "ParallelModel::computePaths: 3.2" << endl;
#endif
                // akcja nie jest deterministyczna, dalej nie można jej rozwijać
                prefixes.push(np);
                // przy okazji przejedź na najbliższą akcję inną lub na ostatnią
                do {
                    action++;
                } while (action < currentState->counter - 1 &&
                         currentState->actions[action - 1] == currentState->actions[action]);
            }
        }

        // jeśli za mało - zakręć
    }

    // przepisz do wyniku wszystkie bieżące aktywne
    while (activePrefixes.size() > 0) {
        prefixes.push(activePrefixes.front());
        activePrefixes.pop();
    }

    // zakończono - zwróć rozmiar wyniku
    return prefixes.size();
}


// Funkcja analogiczna, uzupełniająca, zakładajaca tylko jeden konkretny węzeł startowy
int ParallelModel::computePaths(int s, int n, queue <ActionPath> &prefixes) {
    ActionPath ap;
    return computePaths(s, ap, n, prefixes);
}

/* -------------------------------------------------------------------------
 * Funkcja do czyszczenia przebytej przez wątek ścieżki.
 * Używana przy poszukiwaniu strategii.
 */
#if 0 // na razie zbędna
void ParallelModel::cleanPath(int s, int p, int threadId) {
    // jeśli nie moje, nie po ścieżce albo już wyczyszczone - wróć
    if(states[s]->threadId != threadId || states[s]->previousStateIdx != p || states[s]->opState == ParallelState::clear) return;

    // zapamiętaj miejsce
    int currentAction = states[s]->currentAction;
    int currentActionId = states[s]->actions[currentAction];
    // wyczyść
    states[s]->clean();

    // przejdź dalej
    do {
        // wyczyść rekurencyjnie ścieżkę
        cleanPath(states[s]->edges[currentAction], s, threadId);
        // idź do poprzedniej akcji
        currentAction--;
    } while (currentAction >= 0 && states[s]->actions[currentAction] == currentActionId);
    // osiągnięto początek tablicy lub akcja wskazywana jest różna od akcji pierwotnej - zakończ
}
#endif

/* -------------------------------------------------------------------------
 * Metoda zrównoleglająca wyszukiwanie strategii przez utworzenie nowych
 * procesów wyłącznie z zadanego węzła.
 * s - węzeł startowy
 */
bool ParallelModel::forkRecursiveDFS(int s) {

#ifdef __DEBUG__
    cout << "ParallelModel::forkRecursiveDFS(" << s << ")" << endl;
#endif

    // zienna pomocnicza, żeby nie trzeba było ciągle dłubać
    ParallelState *currentState = states[s];

    // ilu potomków udało się odpalić
    int forkCounter = 0;

    // pętla po wszystkich akcjach w danym stanie
    for (int i = 0; i < currentState->counter; i++) {
        // zapamiętaj bieżący stan
        int currentAction = currentState->actions[i];
        // utwórz proces potomny
        switch (fork()) {
            case -1:    // błąd
                cerr << "ParallelModel::forkRecursiveDFS: błąd fork" << endl;
                exit(1);

            case 0:     // proces potomny
                // odpal wyszukiwanie zgodnie z nieżącą akcją
                if (recursiveDFS(s, -1, standard, currentAction)) {
                    cout << "OK " << endl;
//                     cout << *this << endl << endl;
//                    printStrategy(0, cout);
                    exit(0);
                } else {
                    cout << "FAILED " << endl;
                    exit(1);
                }

            default:    // proces macierzysty
                forkCounter++;
#ifdef __DEBUG__
                cout << "ParallelModel::forkRecursiveDFS: " << s << "(" << currentAction << ")" << endl;
#endif
                // znajdź kolejną akcję
                while (i < currentState->counter && currentState->actions[i] == currentAction) {
                    i++;
                }
        }
//         break;
    }
    // wyzbieraj potomków
    bool success = false;
    for (int i = 0; i < forkCounter; i++) {
        int result;
        // poczekaj na potomka
        if (wait(&result) == -1) {
            cerr << "ParallelModel::forkRecursiveDFS: wait() error" << endl;
        } else {
            // jeśli znalazł strategię to wygrywamy
            success = success || (result == 0);
        }
    }
    cout << "spawned " << forkCounter << " threads" << endl;
    return success;
}

/* -------------------------------------------------------------------------
 * Metoda zrównoleglająca wyszukiwanie strategii przez utworzenie nowych
 * procesów. Zaczynamy od bieżącego węzła ale następuje próba wstępnego
 * wyliczenia możliwego stopnia zrównoleglenia przez sprawdzenie możliwych
 * prefiksów.
 * s - węzeł startowy
 * n - liczba wątków poniżej której jeszcze się szuka poprawy zrównoleglenia
 */
bool ParallelModel::forkRecursiveDFS(int s, int n) {
    if (n == 0) {
        ParallelState *currentState = states[s];
        bool result = false;
        for (int i = 0; i < currentState->counter;) {
            dfsOk = true;
//            cout << i << endl;
            // zapamiętaj bieżący stan
            int currentAction = currentState->actions[i];
            // utwórz proces potomny
//            cout << currentAction << endl;


            if (nprecursiveDFS(s, -1, standard, currentAction)) {
                result = true;
//                return true;
            }

//            cout << "Hello" << endl;

            while (i < currentState->counter && currentState->actions[i] == currentAction) {
                i++;
//                cout << i << endl;
            }

            for (auto state: states) {
                state->clean();
            }
        }

        return result;
    }
    // znajdź wszystkie prefiksy dla zrównoleglenia
    queue <ActionPath> prefixes;
    set<int> spawnedChildren;

#ifdef __DEBUG__
    cout << "ParallelModel::forkRecursiveDFS(" << s << "," << n << ")" << endl;
#endif

    computePaths(s, n, prefixes);

#ifdef __DEBUG__
    cout << "ParallelModel::forkRecursiveDFS(,): found " << prefixes.size() << " prefixes" << endl;
#endif

    if (prefixes.size() == 1) {
        // za mało na tę procedurę, odpal "standard"
        return forkRecursiveDFS(s);
    }

    // ilu potomków udało się odpalić
    int forkCounter = 0;

    // teraz pętla po prefiksach
    while (prefixes.size() > 0) {
        // jest na czym pracować - wyjmij z kolejki
        ActionPath currentPrefix = prefixes.front();
        prefixes.pop();

#ifdef __DEBUG__
        dumpActionPath(currentPrefix, cout << s);
#endif

        // ostatnia akcja - potrzebna do uruchomienia weryfikacji
        int lastAction = currentPrefix.back();
        currentPrefix.pop_back();

        // przejdź po ścieżce określonej przez prefix i znajdź docelowy stan
        int cs = followPath(s, currentPrefix);

#ifdef __DEBUG__
        cout << "&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&" << endl;
        cout << *this;
        cout << "&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&" << endl;
#endif
        int id;
        // w tym stanie uruchom potomka
        switch (id = fork()) {
            case -1:    // błąd
                cerr << "ParallelModel::forkRecursiveDFS: błąd fork" << endl;
                exit(1);

            case 0:     // proces potomny
                // odpal wyszukiwanie zgodnie z nieżącą akcją

//                     cout << "FAILED " << cs << endl;
//                     exit(1);
                if (recursiveDFS(cs, -1, standard, lastAction)) {
//                    cout << "OK " << cs << endl;
//                     cout << *this << endl << endl;
//                    printStrategy(0, cout);
                    exit(0);
                } else {
//                    cout << "FAILED " << cs << endl;
                    exit(1);
                }

            default:    // proces macierzysty
                // zapamiętaj potomka
                spawnedChildren.insert(id);
                // zwiększ licznik
                forkCounter++;
        }

        // wyczyść model cofając się po prefiksie
        resetPath(s, currentPrefix);
        // zakręć biorąc kolejny
    }

//    cout << "spawned " << forkCounter << " children" << endl;
    // wyzbieraj potomków
    bool success = false;
    int id;
    while (spawnedChildren.size() > 0 && !success) {
        int result;
        // poczekaj na potomka
        id = wait(&result);
        if (id == -1) {
            cerr << "ParallelModel::forkRecursiveDFS: wait() error" << endl;
        } else {
            // usuń ze zbioru potomków
            spawnedChildren.erase(id);
            // jeśli znalazł strategię to wygrywamy
            success = success || (result == 0);
            if (success) {
//                cout << "STRATEGY FOUND at child " << id << endl;
            }
        }
    }
    // wybij resztę
    for (set<int>::iterator it = spawnedChildren.begin(); it != spawnedChildren.end(); it++) {
        // wyślij kill do potomka
        if (kill(*it, SIGTERM) == -1) {
            cerr << "Cannot kill " << *it << endl;
        }
    }
    // i odczekaj na ich koniec
    while (spawnedChildren.size() > 0) {
        // poczekaj na potomka
        id = wait(NULL);
        if (id == -1) {
            cerr << "ParallelModel::forkRecursiveDFS: wait() error" << endl;
        } else {
            // usuń ze zbioru potomków
//            cout << "Child " << id << " succesfully terminated" << endl;
            spawnedChildren.erase(id);
        }
    }
    return success;
}

bool ParallelModel::nprecursiveDFS(int s, int p, operationMode mode, int action) {
//    cout << s << " " << p << " " << mode << " " << action << endl;
    // zienna pomocnicza, żeby nie trzeba było ciągle dłubać
    ParallelState *currentState = states[s];

    currentState->visitedCount++;
    if(currentState->visitedCount > 1000 || !dfsOk) {
        dfsOk = false;
        return false;
    }

#ifdef __DEBUG__
    cout << "ParallelModel::recursiveDFS: " << *currentState << " ; " << p << " ; " << mode << " ; " << action << endl;
#endif

#ifdef __DEBUG__
    cout << " $1(" << currentState->id << ")" << endl;
#endif

    // jeśli stan jest wygrywajacy
    if (currentState->accept) {
        // zwróć odwrotność resume
#ifdef __DEBUG__
        cout << " $1.1(" << currentState->id << ") " << mode << endl;
#endif
        return (action != -1 || mode != resume);
    }


    if (currentState->opState == ParallelState::negative || currentState->counter == 0) {
        // zwróć fałsz
        return false;
    }

    // węzeł reprezentant klasy abstrakcji, jeśli jest
    ParallelState *headState = NULL;
    // co do akcji to sprawdzamy, czy nie jest to węzeł z jakiejś klasy abstrakcji
    if (currentState->abstractClass == -1) {
        // jeśli nie, sam jest dla siebie "głową"
        headState = currentState;
    } else {
        // jak nie to pobieramy "głowę"
        headState = states[findClass(currentState->id)];
    }

#ifdef __DEBUG__
    cout << " $2(" << currentState->id << ")"  << endl;
#endif

    // jeśli normalna akcja
    if (mode == standard) {
        // czy stan był już odwiedzony pozytywnie
        if (currentState->opState == ParallelState::positive) {
            // jest OK, zwróć prawdę
            return true;
        }
#ifdef __DEBUG__
        cout << " $2.1(" << currentState->id << ")"  << *currentState << endl;
#endif

        // w p.p. czy był odwiedzony w ogóle
        if (currentState->opState == ParallelState::searching) {
            // stan był odwiedzony ale nie jest zakończony - zwróć fałsz
//            cout << " BUM" << endl;
            return false;
        }

#ifdef __DEBUG__
        cout << " $2.2(" << currentState->id << ")"  << endl;
#endif

        // prawdopodobnie zbędny sanity check
        if (currentState->opState != ParallelState::clear) {
            // dziwne, to się nie powinno wydarzyć
            cerr << "model::recursiveDFS: I should not be here!" << endl;
            exit(1);
        }
        // stan jest czysty, trzeba zainicjować
        currentState->opState = ParallelState::searching;
        currentState->threadId = 0; // niepotrzebne ale niech będzie
        currentState->previousStateIdx = p;

#ifdef __DEBUG__
        cout << " $2.3(" << currentState->id << ")"  << endl;
#endif

        // co do akcji to sprawdzamy, czy nie jest to węzeł z jakiejś klasy abstrakcji
        if (currentState->abstractClass == -1) {
            // jeśli nie, to decydujemy o akcji lokalnie
            if (action == -1) {
                currentState->currentAction = 0;
            } else {
                currentState->currentAction = currentState->findAction(action);
                if (currentState->currentAction == -1) {
                    cerr << "model::recursiveDFS: requested action not found!" << endl;
                    exit(1);
                }
            }
        } else {
            // w p.p. zależymy od "głowy"
            // teraz, czy w gowie jest ustalona akcja
            if (headState->currentAction == -1) {
                if (action == -1) {
                    headState->currentAction = 0;
                } else {
                    headState->currentAction = headState->findAction(action);
                    if (headState->currentAction == -1) {
                        cerr << "model::recursiveDFS: requested action not found!" << endl;
                        exit(1);
                    }
                }

            } // w p.p. musimy uwzględnić akcję ustawioną w głowie

            // znajdź pierwszą akcję zgodną z tą w głowie
            if (currentState != headState) {
                currentState->currentAction = currentState->findAction(headState->actions[headState->currentAction]);
            }
            // zwiększamy licznik zajętości dla głowy
            headState->classCounter++;
        }


    } else {
        // flaga mode jest ustawiona, po prostu wchodzimy w pętlę
//        cout << "Hello " << currentState->opState << endl;
//        if(mode == resume) {
//        if (currentState->opState == ParallelState::positive) {
//            // jest OK, zwróć prawdę
//            return false;
//        }
//            if (currentState->opState == ParallelState::searching) {
//                // stan był odwiedzony ale nie jest zakończony - zwróć fałsz
////            cout << " BUM" << endl;
//                return false;
//            }
//        }
    }

#ifdef __DEBUG__
    cout << " $3(" << currentState->id << ")"  << endl;
#endif

    // pętla tak długo jak długo są jakieś akcje
//    cout << currentState->currentAction << " " << currentState->counter << endl;
    while (currentState->currentAction < currentState->counter) {
        int currentActionId = currentState->actions[currentState->currentAction];

#ifdef __DEBUG__
        cout << " $3.1(" << currentState->id << ")"  << endl;
#endif

        // rekurencyjnie po grafie
        bool result = nprecursiveDFS(currentState->edges[currentState->currentAction], s, mode);
        // skasuj resume
        mode = standard;

#ifdef __DEBUG__
        cout << " $3.2(" << currentState->id << ") " << result << endl;
#endif

        if (result) {
#ifdef __DEBUG__
            cout << " $3.2.1(" << currentState->id << ")"  << endl;
#endif
            // chwilowo udało się znaleźć dobrą strategię
            if (currentState->currentAction + 1 == currentState->counter ||
                currentActionId != currentState->actions[currentState->currentAction + 1]) {
                // kolejna akcja jest inna niż bieżąca albo jej nie ma
                // zwróć sukces
                currentState->opState = ParallelState::positive;
                return true;
            }
            // następna akcja jest taka sama więc trzeba ją też sprawdzić
            currentState->currentAction++;
            // resztę załątwi pętla
            continue;
        } else {
            // nie da się wygrać bieżącą akcją

#ifdef __DEBUG__
            cout << " $3.2.2(" << *currentState << ")"  << endl;
#endif
            // jeśli poprzedniej akcji nie ma lub jest ona różna od bieżącej
            if (currentState->currentAction == 0 ||
                currentActionId != currentState->actions[currentState->currentAction - 1]) {

                /* Inaczej jak jest klasa abstrakcji */
                if (currentState->abstractClass != -1) {
#ifdef __DEBUG__
                    cout << " $3.2.2.1(" << currentState->id << ")"  << endl;
#endif
                    // jeśli aktywnych węzłów w klasie jest więcej niż 1, oznacza to, że ktoś w klasie jeszcze potrzebuje tej wartości
                    if (headState->classCounter > 1) {
                        // a to oznacza porażkę - powróć
                        // zmniejsz licznik w headState
                        headState->classCounter--;
                        // czyścimy
                        currentState->clean();
#ifdef __DEBUG__
                        cout << " $3.2.2.1.1(" << currentState->id << ")"  << endl;
#endif
                        // zwróć porażkę
                        return false;
                    } else {
#ifdef __DEBUG__
                        cout << " $3.2.2.1.2(" << currentState->id << ")"  << endl;
#endif
                        // w p.p. to był jedyny węzeł z tej klasy i można kontynuować
                    }
                }
#ifdef __DEBUG__
                cout << " $3.2.2.2(" << currentState->id << ")"  << endl;
#endif
                // zaliczono wpadkę, idziemy do kolejnej akcji (lub na koniec)
                do {
                    currentState->currentAction++;
                } while (currentState->currentAction < currentState->counter
                         && currentState->actions[currentState->currentAction] == currentActionId);

                // jeśli była zadana akcja, przeskocz od razu na koniec
                if (action != -1) {
                    currentState->currentAction = currentState->counter;
                }

                // ewentualnie trzeba być może poprawić w węźle nadrzędnym klasy abstrakcji
                if (currentState->abstractClass != -1 && currentState->id != currentState->abstractClass) {
                    // jeśli już koniec
                    if (currentState->counter == currentState->currentAction) {
                        // ustaw w nadrzędnej na koniec
                        headState->currentAction = headState->counter;
                    } else {
                        // w p.p. znajdź pierwszą akcję z takim samym identyfikatorem
                        headState->currentAction = headState->findAction(
                                currentState->actions[currentState->currentAction]);
                    }
                }
                // resztę załatwia pętla
#ifdef __DEBUG__
                cout << " $3.2.2.3(" << currentState->id << ")"  << endl;
#endif
                continue;
            }
            // poprzednia akcja jest taka sama - trzeba się w niej cofnąć
            mode = resume;
            currentState->currentAction--;
            // resztę załatwia pętla
#ifdef __DEBUG__
            cout << " $3.2.3(" << currentState->id << ")"  << endl;
#endif
            continue;
        }
#ifdef __DEBUG__
        cout << " $3.3(" << currentState->id << ")"  << endl;
#endif
    }

#ifdef __DEBUG__
    cout << " $4(" << currentState->id << ")"  << endl;
#endif
    // nie udało się znaleźć akcji prowadzącej do wygranej
    // zmniejsz licznik w headState
    headState->classCounter--;
    // przyznaj się do porażki
    currentState->clean();
    return false;
}

/* -------------------------------------------------------------------------
 * Szeregowa metoda do poszukiwania strategii
 * s - węzeł do odwiedzenia
 * p - poprzedni węzeł (?)
 * mode - czy ma być przeszukiwanie ma być wznowione od ostatniej decyzji
 *          tylko do użytku wewnętrznego
 * depth - głębokość, potrzebne... czasami
 */

bool ParallelModel::recursiveDFS(int s, int p, operationMode mode, int action) {
//    cout << s << " " << p << " " << mode << " " << action << endl;
    // zienna pomocnicza, żeby nie trzeba było ciągle dłubać
    ParallelState *currentState = states[s];

#ifdef __DEBUG__
    cout << "ParallelModel::recursiveDFS: " << *currentState << " ; " << p << " ; " << mode << " ; " << action << endl;
#endif

#ifdef __DEBUG__
    cout << " $1(" << currentState->id << ")" << endl;
#endif

    // jeśli stan jest wygrywajacy
    if (currentState->accept) {
        // zwróć odwrotność resume
#ifdef __DEBUG__
        cout << " $1.1(" << currentState->id << ") " << mode << endl;
#endif
        return (action != -1 || mode != resume);
    }


    if (currentState->opState == ParallelState::negative || currentState->counter == 0) {
        // zwróć fałsz
        return false;
    }

    // węzeł reprezentant klasy abstrakcji, jeśli jest
    ParallelState *headState = NULL;
    // co do akcji to sprawdzamy, czy nie jest to węzeł z jakiejś klasy abstrakcji
    if (currentState->abstractClass == -1) {
        // jeśli nie, sam jest dla siebie "głową"
        headState = currentState;
    } else {
        // jak nie to pobieramy "głowę"
        headState = states[findClass(currentState->id)];
    }

#ifdef __DEBUG__
    cout << " $2(" << currentState->id << ")"  << endl;
#endif

    // jeśli normalna akcja
    if (mode == standard) {
        // czy stan był już odwiedzony pozytywnie
        if (currentState->opState == ParallelState::positive) {
            // jest OK, zwróć prawdę
            return true;
        }
#ifdef __DEBUG__
        cout << " $2.1(" << currentState->id << ")"  << *currentState << endl;
#endif

        // w p.p. czy był odwiedzony w ogóle
        if (currentState->opState == ParallelState::searching) {
            // stan był odwiedzony ale nie jest zakończony - zwróć fałsz
//            cout << " BUM" << endl;
            return false;
        }

#ifdef __DEBUG__
        cout << " $2.2(" << currentState->id << ")"  << endl;
#endif

        // prawdopodobnie zbędny sanity check
        if (currentState->opState != ParallelState::clear) {
            // dziwne, to się nie powinno wydarzyć
            cerr << "model::recursiveDFS: I should not be here!" << endl;
            exit(1);
        }
        // stan jest czysty, trzeba zainicjować
        currentState->opState = ParallelState::searching;
        currentState->threadId = 0; // niepotrzebne ale niech będzie
        currentState->previousStateIdx = p;

#ifdef __DEBUG__
        cout << " $2.3(" << currentState->id << ")"  << endl;
#endif

        // co do akcji to sprawdzamy, czy nie jest to węzeł z jakiejś klasy abstrakcji
        if (currentState->abstractClass == -1) {
            // jeśli nie, to decydujemy o akcji lokalnie
            if (action == -1) {
                currentState->currentAction = 0;
            } else {
                currentState->currentAction = currentState->findAction(action);
                if (currentState->currentAction == -1) {
                    cerr << "model::recursiveDFS: requested action not found!" << endl;
                    exit(1);
                }
            }
        } else {
            // w p.p. zależymy od "głowy"
            // teraz, czy w gowie jest ustalona akcja
            if (headState->currentAction == -1) {
                if (action == -1) {
                    headState->currentAction = 0;
                } else {
                    headState->currentAction = headState->findAction(action);
                    if (headState->currentAction == -1) {
                        cerr << "model::recursiveDFS: requested action not found!" << endl;
                        exit(1);
                    }
                }

            } // w p.p. musimy uwzględnić akcję ustawioną w głowie

            // znajdź pierwszą akcję zgodną z tą w głowie
            if (currentState != headState) {
                currentState->currentAction = currentState->findAction(headState->actions[headState->currentAction]);
            }
            // zwiększamy licznik zajętości dla głowy
            headState->classCounter++;
        }


    } else {
        // flaga mode jest ustawiona, po prostu wchodzimy w pętlę
//        cout << "Hello " << currentState->opState << endl;
//        if(mode == resume) {
//        if (currentState->opState == ParallelState::positive) {
//            // jest OK, zwróć prawdę
//            return false;
//        }
//            if (currentState->opState == ParallelState::searching) {
//                // stan był odwiedzony ale nie jest zakończony - zwróć fałsz
////            cout << " BUM" << endl;
//                return false;
//            }
//        }
    }

#ifdef __DEBUG__
    cout << " $3(" << currentState->id << ")"  << endl;
#endif

    // pętla tak długo jak długo są jakieś akcje
//    cout << currentState->currentAction << " " << currentState->counter << endl;
    while (currentState->currentAction < currentState->counter) {
        int currentActionId = currentState->actions[currentState->currentAction];

#ifdef __DEBUG__
        cout << " $3.1(" << currentState->id << ")"  << endl;
#endif

        // rekurencyjnie po grafie
        bool result = recursiveDFS(currentState->edges[currentState->currentAction], s, mode);
        // skasuj resume
        mode = standard;

#ifdef __DEBUG__
        cout << " $3.2(" << currentState->id << ") " << result << endl;
#endif

        if (result) {
#ifdef __DEBUG__
            cout << " $3.2.1(" << currentState->id << ")"  << endl;
#endif
            // chwilowo udało się znaleźć dobrą strategię
            if (currentState->currentAction + 1 == currentState->counter ||
                currentActionId != currentState->actions[currentState->currentAction + 1]) {
                // kolejna akcja jest inna niż bieżąca albo jej nie ma
                // zwróć sukces
                currentState->opState = ParallelState::positive;
                return true;
            }
            // następna akcja jest taka sama więc trzeba ją też sprawdzić
            currentState->currentAction++;
            // resztę załątwi pętla
            continue;
        } else {
            // nie da się wygrać bieżącą akcją

#ifdef __DEBUG__
            cout << " $3.2.2(" << *currentState << ")"  << endl;
#endif
            // jeśli poprzedniej akcji nie ma lub jest ona różna od bieżącej
            if (currentState->currentAction == 0 ||
                currentActionId != currentState->actions[currentState->currentAction - 1]) {

                /* Inaczej jak jest klasa abstrakcji */
                if (currentState->abstractClass != -1) {
#ifdef __DEBUG__
                    cout << " $3.2.2.1(" << currentState->id << ")"  << endl;
#endif
                    // jeśli aktywnych węzłów w klasie jest więcej niż 1, oznacza to, że ktoś w klasie jeszcze potrzebuje tej wartości
                    if (headState->classCounter > 1) {
                        // a to oznacza porażkę - powróć
                        // zmniejsz licznik w headState
                        headState->classCounter--;
                        // czyścimy
                        currentState->clean();
#ifdef __DEBUG__
                        cout << " $3.2.2.1.1(" << currentState->id << ")"  << endl;
#endif
                        // zwróć porażkę
                        return false;
                    } else {
#ifdef __DEBUG__
                        cout << " $3.2.2.1.2(" << currentState->id << ")"  << endl;
#endif
                        // w p.p. to był jedyny węzeł z tej klasy i można kontynuować
                    }
                }
#ifdef __DEBUG__
                cout << " $3.2.2.2(" << currentState->id << ")"  << endl;
#endif
                // zaliczono wpadkę, idziemy do kolejnej akcji (lub na koniec)
                do {
                    currentState->currentAction++;
                } while (currentState->currentAction < currentState->counter
                         && currentState->actions[currentState->currentAction] == currentActionId);

                // jeśli była zadana akcja, przeskocz od razu na koniec
                if (action != -1) {
                    currentState->currentAction = currentState->counter;
                }

                // ewentualnie trzeba być może poprawić w węźle nadrzędnym klasy abstrakcji
                if (currentState->abstractClass != -1 && currentState->id != currentState->abstractClass) {
                    // jeśli już koniec
                    if (currentState->counter == currentState->currentAction) {
                        // ustaw w nadrzędnej na koniec
                        headState->currentAction = headState->counter;
                    } else {
                        // w p.p. znajdź pierwszą akcję z takim samym identyfikatorem
                        headState->currentAction = headState->findAction(
                                currentState->actions[currentState->currentAction]);
                    }
                }
                // resztę załatwia pętla
#ifdef __DEBUG__
                cout << " $3.2.2.3(" << currentState->id << ")"  << endl;
#endif
                continue;
            }
            // poprzednia akcja jest taka sama - trzeba się w niej cofnąć
            mode = resume;
            currentState->currentAction--;
            // resztę załatwia pętla
#ifdef __DEBUG__
            cout << " $3.2.3(" << currentState->id << ")"  << endl;
#endif
            continue;
        }
#ifdef __DEBUG__
        cout << " $3.3(" << currentState->id << ")"  << endl;
#endif
    }

#ifdef __DEBUG__
    cout << " $4(" << currentState->id << ")"  << endl;
#endif
    // nie udało się znaleźć akcji prowadzącej do wygranej
    // zmniejsz licznik w headState
    headState->classCounter--;
    // przyznaj się do porażki
    currentState->clean();
    return false;
}

/* -------------------------------------------------------------------------
 * Współbieżna metoda do poszukiwania strategii - metoda główna
 * Oparta na metodzie szeregowej ale odpala dodatkowe wątki w przypadku
 * gdy jest rozgałęzienie
 * s - węzeł do odwiedzenia
 * p - poprzedni węzeł (?)
 * resume - czy ma być przeszukiwanie ma być wznowione od ostatniej decyzji
 *          tylko do użytku wewnętrznego
 * threadId - id bieżącego wątku
 */
bool ParallelModel::parallelRecursiveDFS(int s, int p, operationMode mode, int threadId) {
    // zienna pomocnicza, żeby nie trzeba było ciągle dłubać
    ParallelState *currentState = states[s];

#ifdef __DEBUG__
    //     cout << *currentState << " " << p << " " << resume << " " << threadsVector.size() << endl;
#endif

#ifdef __DEBUG__
    trace(" #1",currentState->to_string());
#endif

    // jeśli stan jest wygrywajacy
    if (currentState->accept) {
        // zwróć odwrotność resume
        return (mode != resume);
    }

    // jeśli stan jest przegrywający
    mutexes[currentState->id % MUTEX_COUNT].lock();
    ParallelState::operation opState = currentState->opState;
    mutexes[currentState->id % MUTEX_COUNT].unlock();
    if (opState == ParallelState::negative || currentState->counter == 0) {
        // zwróć fałsz
        return false;
    }

    // węzeł reprezentant klasy abstrakcji, jeśli jest
    ParallelState *headState = NULL;
    // co do akcji to sprawdzamy, czy nie jest to węzeł z jakiejś klasy abstrakcji
    if (currentState->abstractClass == -1) {
        // jeśli nie, sam jest dla siebie "głową"
        headState = currentState;
    } else {
        // jak nie to pobieramy "głowę"
        headState = states[findClass(currentState->id)];
    }

#ifdef __DEBUG__
    trace(" #2",currentState->to_string());
#endif

    // jeśli normalna akcja
    if (mode == standard) {

        // -------------------------------------------------------
        // teraz zablokuj węzeł
        mutexes[currentState->id % MUTEX_COUNT].lock();
        // -------------------------------------------------------

        // czy stan był już odwiedzony pozytywnie
        if (currentState->opState == ParallelState::positive) {
            // jest OK, zwróć prawdę
            mutexes[currentState->id % MUTEX_COUNT].unlock();
            return true;
        }
#ifdef __DEBUG__
        trace(" #2.1",currentState->to_string());
#endif

        // w p.p. czy był odwiedzony w ogóle
        if (currentState->opState == ParallelState::searching) {
            // stan był odwiedzony ale nie jest zakończony
            // czy przez ten wątek?
            if (currentState->threadId == threadId) {
                // owszem - jest pętla, zwróć porażkę
                mutexes[currentState->id % MUTEX_COUNT].unlock();
                return false;
            } else {
                // w p.p. inny wątek tu grzebał - trzeba przejąć

                // sprawdź, czy ten wątek jeszcze działa
                if (threadsVector[currentState->threadId] != NULL) {
                    // zwolnij węzeł
                    mutexes[currentState->id % MUTEX_COUNT].unlock();

#ifdef __DEBUG__
                    trace(" #2.1.1",currentState->to_string());
#endif

                    // wątek pracuje - zaczekaj aż się zakończy
                    threadsVector[currentState->threadId]->join();
                    threadsRunning--;
                    // skasuj stan
                    threadsVector[currentState->threadId] = NULL;

#ifdef __DEBUG__
                    trace(" #2.1.2",currentState->to_string());
#endif
                    // zablokuj węzeł
                    mutexes[currentState->id % MUTEX_COUNT].lock();
                }

                // tryb pracy na follow
                mode = follow;
            }
        }

#if 0
        // prawdopodobnie zbędny sanity check
        if(currentState->opState != state::clear) {
            // dziwne, to się nie powinno wydarzyć
            cerr << "model::parallelRecursiveDFS: I should not be here!" << endl;
            exit(1);
        }
#endif

        // stan jest czysty, trzeba zainicjować
        currentState->opState = ParallelState::searching;
        // ustaw identyfikator na swój
        currentState->threadId = threadId;  // najważniejsze
        currentState->previousStateIdx = p;

#ifdef __DEBUG__
        trace(" #2.2",currentState->to_string());
#endif

        // w trybie pracy follow nic tu nie robimy
        if (mode != follow) {
            // co do akcji to sprawdzamy, czy nie jest to węzeł z jakiejś klasy abstrakcji
            if (currentState->abstractClass == -1) {
                // jeśli nie, to decydujemy o akcji lokalnie
                currentState->currentAction = 0;
            } else {
                // w p.p. zależymy od "głowy"
                // teraz, czy w gowie jest ustalona akcja
                if (headState->currentAction == -1) {
                    // nie ma - to ją ustaw
                    headState->currentAction = 0;
                } // w p.p. musimy uwzględnić akcję ustawioną w głowie

                // znajdź pierwszą akcję zgodną z tą w głowie
                if (currentState != headState) {
                    currentState->currentAction = 0;
                    // skanuj tablicę akcji aż znajdziesz akcję o takim samym identyfikatorze
                    while (currentState->actions[currentState->currentAction] !=
                           headState->actions[headState->currentAction]) {
                        currentState->currentAction++;
                    }
                }
                // zwiększamy licznik zajętości dla głowy
                headState->classCounter++;
            }
        }

        // -------------------------------------------------------
        // teraz odblokuj węzeł, już jesteś jego "właścicielem"
        mutexes[currentState->id % MUTEX_COUNT].unlock();
        // -------------------------------------------------------

    } else {
        // jeśli było resume, wchodzimy w pętlę
    }

#ifdef __DEBUG__
    trace(" #3",currentState->to_string());
#endif

    // pętla tak długo jak długo są jakieś akcje
    while (currentState->currentAction < currentState->counter) {
        int currentActionId = currentState->actions[currentState->currentAction];

#ifdef __DEBUG__
        trace(" #3.1",currentState->to_string()+" "+std::to_string(currentState->currentAction));
#endif


#ifndef __NOFORK__

        // obejrzyj kolejną akcję, jeśli jest
        // jeśli jest taka sama jak bieżąca, wyślij tam nowy wątek
        if (currentState->currentAction + 1 < currentState->counter &&
            currentActionId == currentState->actions[currentState->currentAction + 1]) {
            // kolejna akcja jest taka sama jak bieżąca
#ifdef __DEBUG__
            trace(" #3.1.1",currentState->to_string());
#endif
            // sprawdź, czy nie ma czego posprzątać
            joinQueueMutex.lock();
            int threadsJoined = 0;
            while (joinQueue.size() > 0) {
                // posprzątaj
                int t = joinQueue.front();
                joinQueue.pop();

                threadsVector[t]->join();
                threadsJoined++;
                threadsVector[t] = NULL;
            }
            joinQueueMutex.unlock();

            threadsMutex.lock();
            threadsRunning -= threadsJoined;

            // czy nie za dużo...
            if (threadsRunning < MAX_THREADS &&
                currentState->currentAction + 1 < currentState->counter &&
                currentActionId == currentState->actions[currentState->currentAction + 1]) {
#if 0
                // walniemy wątkami z grubej rury po całości
                for(int j=1; currentState->currentAction + j < currentState->counter &&
                    currentActionId == currentState->actions[currentState->currentAction + j]; j++) {
                    // utwórz nowy wątek
                    thread *t = new thread(recursiveHelperThreadStart, currentState->edges[currentState->currentAction + j], s,
                                        threadsStarted, this);
#else
                {
                    // utwórz nowy wątek
                    thread *t = new thread(recursiveHelperThreadStart,
                                           currentState->edges[currentState->currentAction + 1], s,
                                           threadsStarted, this);
#endif
                    if (t == NULL) {
                        cerr << "WARNING: Cannot spawn a thread " << threadsStarted << endl;
                    } else {
#ifdef __DEBUG__
                        trace(" #3.1.2",currentState->to_string());
#endif
                        threadsRunning++;
                        threadsVector[threadsStarted] = t;
                        // przesuń indeks
                        if (++threadsStarted == threadsVector.size()) {
//                             cout << "# " << threadsStarted << endl;
                            // jeśli już koniec tablicy, zwiększ ją 2x
                            threadsVector.resize(threadsStarted * 2);
//                             cout << "# " << threadsStarted << " OK" << endl;
                        }
#ifdef __DEBUG__
                        trace(" #3.1.3",currentState->to_string());
#endif
                    }
                }
            }
            threadsMutex.unlock();
        } else {
            cout << "& " << currentState->currentAction << " " << currentState->counter << " " << currentActionId << " "
                 << currentState->actions[currentState->currentAction + 1] << endl;
        }

#endif






        // rekurencyjnie po grafie
        bool result = parallelRecursiveDFS(currentState->edges[currentState->currentAction], s, mode, threadId);
        // skasuj resume
        mode = standard;

#ifdef __DEBUG__
        trace(" #3.2",currentState->to_string());
#endif

        if (result) {
#ifdef __DEBUG__
            trace(" #3.2.1",currentState->to_string());
#endif
            // chwilowo udało się znaleźć dobrą strategię
            if (currentState->currentAction + 1 == currentState->counter ||
                currentActionId != currentState->actions[currentState->currentAction + 1]) {
                // kolejna akcja jest inna niż bieżąca albo jej nie ma
                // zwróć sukces
                currentState->opState = ParallelState::positive;
                return true;
            }
            // następna akcja jest taka sama więc trzeba ją też sprawdzić
            currentState->currentAction++;
            // resztę załątwi pętla
            continue;
        } else {
            // nie da się wygrać bieżącą akcją

#ifdef __DEBUG__
            trace(" #3.2.2",currentState->to_string());
#endif
            // jeśli poprzedniej akcji nie ma lub jest ona różna od bieżącej
            if (currentState->currentAction == 0 ||
                currentActionId != currentState->actions[currentState->currentAction - 1]) {

                /* Inaczej jak jest klasa abstrakcji */
                if (currentState->abstractClass != -1) {
#ifdef __DEBUG__
                    trace(" #3.2.2.1",currentState->to_string());
#endif
                    // jeśli aktywnych węzłów w klasie jest więcej niż 1, oznacza to, że ktoś w klasie jeszcze potrzebuje tej wartości
                    if (headState->classCounter > 1) {
                        // a to oznacza porażkę - powróć
                        // zmniejsz licznik w headState
                        headState->classCounter--;
                        // czyścimy
                        currentState->clean();
#ifdef __DEBUG__
                        trace(" #3.2.2.1.1",currentState->to_string());
#endif
                        // zwróć porażkę
                        return false;
                    } else {
#ifdef __DEBUG__
                        trace(" #3.2.2.1.2",currentState->to_string());
#endif
                        // w p.p. to był jedyny węzeł z tej klasy i można kontynuować
                    }
                }
#ifdef __DEBUG__
                trace(" #3.2.2.2",currentState->to_string());
#endif
                // zaliczono wpadkę, idziemy do kolejnej akcji (lub na koniec)
                do {
                    currentState->currentAction++;
                } while (currentState->currentAction < currentState->counter
                         && currentState->actions[currentState->currentAction] == currentActionId);

                // ewentualnie trzeba być może poprawić w węźle nadrzędnym klasy abstrakcji
                if (currentState->abstractClass != -1 && currentState->id != currentState->abstractClass) {
                    // jeśli już koniec
                    if (currentState->counter == currentState->currentAction) {
                        // ustaw w nadrzędnej na koniec
                        headState->currentAction = headState->counter;
                    } else {
                        // w p.p. znajdź pierwszą akcję z takim samym identyfikatorem
                        headState->currentAction = 0;
                        while (currentState->actions[currentState->currentAction] !=
                               headState->actions[headState->currentAction]) {
                            headState->currentAction++;
                        }
                    }
                }
                // resztę załatwia pętla
#ifdef __DEBUG__
                trace(" #3.2.2.3",currentState->to_string());
#endif
                continue;
            }
            // poprzednia akcja jest taka sama - trzeba się w niej cofnąć
            mode = resume;
            currentState->currentAction--;
            // resztę załatwia pętla
#ifdef __DEBUG__
            trace(" #3.2.3",currentState->to_string());
#endif
            continue;
        }
#ifdef __DEBUG__
        trace(" #3.3",currentState->to_string());
#endif
    }

#ifdef __DEBUG__
    trace(" #4",currentState->to_string());
#endif
    // nie udało się znaleźć akcji prowadzącej do wygranej
    // zmniejsz licznik w headState
    headState->classCounter--;
    // przyznaj się do porażki
    mutexes[currentState->id % MUTEX_COUNT].lock();
    currentState->clean();
    mutexes[currentState->id % MUTEX_COUNT].unlock();
    return false;
}

/* -------------------------------------------------------------------------
 * Metoda startująca dla wątku pomocniczego, 
 * mająca za zadanie rzeczywiste uruchomienie wątku oraz jego zakończenie
 * s - węzeł do odwiedzenia
 * p - poprzedni węzeł (?)
 * threadId - id bieżącego wątku
 */
bool ParallelModel::recursiveHelperThreadStart(int s, int p, int threadId, ParallelModel *m) {
    bool res = recursiveHelperThread(s, p, standard, threadId, m);

    // zgłoś zakończenie
    // zablokuj mutex
    m->joinQueueMutex.lock();
    m->joinQueue.push(threadId);
    m->joinQueueMutex.unlock();

//     cout << "% kończę " << threadId << endl;

    return res;
}

/* -------------------------------------------------------------------------
 * Metoda dla wątku pomocniczego
 * Oparta na metodzie szeregowej
 * s - węzeł do odwiedzenia
 * p - poprzedni węzeł (?)
 * resume - czy ma być przeszukiwanie ma być wznowione od ostatniej decyzji
 *          tylko do użytku wewnętrznego
 * threadId - id bieżącego wątku
 */
bool ParallelModel::recursiveHelperThread(int s, int p, operationMode mode, int threadId, ParallelModel *m) {
    // zienna pomocnicza, żeby nie trzeba było ciągle dłubać
    ParallelState *currentState = m->states[s];

#ifdef __DEBUG__
    trace("  %1\tw"+to_string(threadId),currentState->to_string());
#endif

    // jeśli stan jest wygrywajacy
    if (currentState->accept) {
        // zwróć odwrotność resume
        return (mode != resume);
    }

    // jeśli stan jest przegrywający
    m->mutexes[currentState->id % MUTEX_COUNT].lock();
    ParallelState::operation opState = currentState->opState;
    m->mutexes[currentState->id % MUTEX_COUNT].unlock();
    if (opState == ParallelState::negative || currentState->counter == 0) {
        // zwróć fałsz
        return false;
    }

    // jeśli to jest węzeł z jakiejś klasy epistemicznej - kończymy
    if (currentState->abstractClass != -1) {
        /* ------------------------------------------ *
         * to prawdopodobnie wymaga rewizji i poprawy *
         * ------------------------------------------ */
#ifdef __DEBUG__
        trace("  %1.1\tw"+to_string(threadId),currentState->to_string());
#endif
        m->joinQueueMutex.lock();
        m->joinQueue.push(threadId);
        m->joinQueueMutex.unlock();
//         cout << "%# kończę " << threadId << endl;

        pthread_exit(NULL);
    }

#ifdef __DEBUG__
    trace("  %2\tw"+to_string(threadId),currentState->to_string());
#endif

    // jeśli normalna akcja
    if (mode == standard) {

        // -------------------------------------------------------
        // teraz zablokuj węzeł
        m->mutexes[currentState->id % MUTEX_COUNT].lock();
        // -------------------------------------------------------

        // czy stan był już odwiedzony pozytywnie
        if (currentState->opState == ParallelState::positive) {
            // jest OK, zwróć prawdę
            m->mutexes[currentState->id % MUTEX_COUNT].unlock();
            return true;
        }

        // jeśli sam tu byłem
        if (currentState->threadId == threadId) {
            // w p.p. czy był odwiedzony w ogóle
            if (currentState->opState == ParallelState::searching) {
                // stan był odwiedzony ale nie jest zakończony - jest pętla, zwróć porażkę
                m->mutexes[currentState->id % MUTEX_COUNT].unlock();
                return false;
            }
            // tutaj coś jest nie w porządku bo jest za późno i nie wiem co wpisać
            cerr << "model::recursiveHelperThread: co mam zrobić? " << threadId << endl;
            exit(1);
        }

        // czy ktoś inny tu nie rządzi
        if (currentState->threadId > -1) {
            // węzeł jest oznaczony przez inny wątek - po prostu zakończ się (wątek)
            m->mutexes[currentState->id % MUTEX_COUNT].unlock();

            m->joinQueueMutex.lock();
            m->joinQueue.push(threadId);
            m->joinQueueMutex.unlock();
//             cout << "%% kończę " << threadId << endl;
            /* ------------------------------------------ *
             * to prawdopodobnie wymaga rewizji i poprawy *
             * ------------------------------------------ */
            pthread_exit(NULL);
        }

        // stan jest czysty, trzeba zainicjować
        currentState->opState = ParallelState::searching;
        currentState->threadId = threadId;  // najważniejsze
        currentState->previousStateIdx = p;

        // o akcji decydujemy lokalnie bo to nie klasa abstrakcji
        currentState->currentAction = 0;

        // zwolnij mutex (?)
        m->mutexes[currentState->id % MUTEX_COUNT].unlock();

    } else {
        // flaga resume jest ustawiona, po prostu wchodzimy w pętlę
    }

#ifdef __DEBUG__
    trace("  %3\tw"+to_string(threadId),currentState->to_string());
#endif

    // pętla tak długo jak długo są jakieś akcje
    while (currentState->currentAction < currentState->counter) {
        int currentActionId = currentState->actions[currentState->currentAction];

#if 0
        // #ifndef __NOFORK__

                // obejrzyj kolejną akcję, jeśli jest
                // jeśli jest taka sama jak bieżąca, wyślij tam nowy wątek
                if (currentState->currentAction + 1 < currentState->counter &&
                    currentActionId == currentState->actions[currentState->currentAction + 1]) {
                    // kolejna akcja jest taka sama jak bieżąca
#ifdef __DEBUG__
                    trace(" %3.1.1",currentState->to_string());
#endif

                    if(m->threadsRunning < MAX_THREADS &&
                        currentState->currentAction + 1 < currentState->counter &&
                        currentActionId == currentState->actions[currentState->currentAction + 1]) {

                        // ile wątków aktualnie pracuje
                        m->threadsMutex.lock();

                        // czy nie za dużo...
                        if(m->threadsRunning < MAX_THREADS) {
                            // walniemy wątkami z grubej rury po całości
                            int spawned=0;
#if 0
                            for(int j=1; currentState->currentAction + j < currentState->counter &&
                                currentActionId == currentState->actions[currentState->currentAction + j]; j++) {
                                // utwórz nowy wątek
                                thread *t = new thread(recursiveHelperThreadStart, currentState->edges[currentState->currentAction + j], s,
                                                    m->threadsStarted, m);
#else
                            {
                                // utwórz nowy wątek
                                thread *t = new thread(recursiveHelperThreadStart, currentState->edges[currentState->currentAction + 1], s,
                                                    m->threadsStarted, m);
#endif
                                if (t == NULL) {
                                    cerr << "WARNING: Cannot spawn a thread " << m->threadsStarted << endl;
                                } else {
#ifdef __DEBUG__
                                    trace(" %3.1.2",currentState->to_string());
#endif
                                    m->threadsRunning++;
                                    m->threadsVector[m->threadsStarted] = t;
                                    // przesuń indeks
                                    if (++(m->threadsStarted) == m->threadsVector.size()) {
            //                             cout << "# " << threadsStarted << endl;
                                        // jeśli już koniec tablicy, zwiększ ją 2x
                                        m->threadsVector.resize(m->threadsStarted * 2);
            //                             cout << "# " << threadsStarted << " OK" << endl;
                                    }
#ifdef __DEBUG__
                                    trace(" %3.1.3",currentState->to_string());
#endif
                                }
                            }
                        }
                        m->threadsMutex.unlock();
                    }
                }
#endif

        // rekurencyjnie po grafie
        bool result = recursiveHelperThread(currentState->edges[currentState->currentAction], s, mode, threadId, m);
        // skasuj resume
        mode = standard;

        if (result) {

            // chwilowo udało się znaleźć dobrą strategię
            if (currentState->currentAction + 1 == currentState->counter ||
                currentActionId != currentState->actions[currentState->currentAction + 1]) {
                // kolejna akcja jest inna niż bieżąca albo jej nie ma
                // zwróć sukces
                currentState->opState = ParallelState::positive;
                return true;
            }
            // następna akcja jest taka sama więc trzeba ją też sprawdzić
            currentState->currentAction++;
            // resztę załątwi pętla
            continue;
        } else {
            // nie da się wygrać bieżącą akcją

            // jeśli poprzedniej akcji nie ma lub jest ona różna od bieżącej
            if (currentState->currentAction == 0 ||
                currentActionId != currentState->actions[currentState->currentAction - 1]) {

                // zaliczono wpadkę, idziemy do kolejnej akcji (lub na koniec)
                do {
                    currentState->currentAction++;
                } while (currentState->currentAction < currentState->counter
                         && currentState->actions[currentState->currentAction] == currentActionId);
                // resztę załatwia pętla
                continue;
            }

            // poprzednia akcja jest taka sama - trzeba się w niej cofnąć
            mode = resume;
            currentState->currentAction--;
            // resztę załatwia pętla
            continue;
        }
    }

#ifdef __DEBUG__
    trace("  %4\tw"+to_string(threadId),currentState->to_string());
#endif
    // nie udało się znaleźć akcji prowadzącej do wygranej
    // przyznaj się do porażki
    m->mutexes[currentState->id % MUTEX_COUNT].lock();
// 	currentState->clean();
    currentState->opState = ParallelState::negative;
    currentState->threadId = -1;
    m->mutexes[currentState->id % MUTEX_COUNT].unlock();
    return false;
}

/* -------------------------------------------------------------------------
 */
ostream &operator<<(ostream &str, ParallelModel &m) {
    for (int i = 0; i < m.counter; i++) {
        str << *(m.states[i]) << endl;
    }
    return str;
}

/* -------------------------------------------------------------------------
 * Funkcja do wyplucia strategii wygrywającej.
 * s : numer stanu do rozpoczęcia wypisywania
 * str : strumień wyjściowy
 */
void ParallelModel::printStrategy(int s, ostream &str) {
    // wektor odwiedzin stanów
    vector<bool> visited;
    visited.resize(states.size(), false);
    queue<int> pending;

    // wstaw stan startowy
    visited[s] = true;
    pending.push(s);

    str << "***********************" << endl;
    // pętla po stanach do odwiedzenia
    while (pending.size() > 0) {
        // weź stan bieżący
        ParallelState *currentState = states[pending.front()];
        pending.pop();

        // "nagłówek"
        str << "state " << currentState->id << ":";
        // jeśli wygrywający
        if (currentState->accept) {
            str << " accept" << endl;
            continue;
        }

        // jeśli nie ma akcji
        if (currentState->counter == 0) {
            str << " fail" << endl;
            continue;
        }
        // wybrana akcja
        int currentAction = getAction(currentState);
        str << " action " << currentAction << endl;

        // nie jest wygrywający - jedziemy po następnikach
        // przejście przez tablicę akcji
        for (int i = 0; i < currentState->counter; i++) {
            // akcja dla krawędzi zgadza się z bieżącą
            if (currentState->actions[i] == currentAction) {
                //wypluj na wyjście
                int destState = currentState->edges[i];
                str << " -> " << destState << endl;
                // czy docelowy stan jest już odwiedzony
                if (!visited[destState]) {
                    // nieodwiedzony - dorzuć do zbioru
                    visited[destState] = true;
                    pending.push(destState);
                }
            }
        }
    }
    str << "***********************" << endl;
}
