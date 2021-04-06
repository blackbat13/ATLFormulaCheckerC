#include "ParallelModel.hpp"
#include <unistd.h>
#include <stack>

using namespace std;


#ifdef __DEBUG__
inline void trace(string head, string msg) {
    string s;
    s = string(head+"\t(");
    s += msg;
    s += ")\n";
    cout << s;
}
#endif

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

    if(counter == actions.size()) {
        int s = 2*actions.size();
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
    for(int i=0; i<counter; i++) {
        str += "(" + std::to_string(actions[i]) + "," + std::to_string(edges[i]) + ")";
    }
    str += "):" + std::to_string(currentAction) + "," + std::to_string(opState) + "," + std::to_string(threadId);

    return str;
}
/* -------------------------------------------------------------------------
 */
ostream& operator<<(ostream &str, ParallelState &s) {
    str << s.to_string();
    return str;
}


/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * Metody dla klasy opisującej model
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

/* -------------------------------------------------------------------------
 */

ParallelModel::ParallelModel(int size) {
    counter = size;
    if(size != 0) states.resize(counter);
    else states.resize(2);
    for(int i = 0; i<counter; i++) {
        states[i] = new ParallelState(i);
    }
    // jeden wątek zablokowany na główny
    threadsStarted=1;
    threadsRunning=0;
    // ustaw domyślny początkowy rozmiar tablicy wątków
    threadsVector.resize(10);
    threadsVector[0]=NULL;

}

/* -------------------------------------------------------------------------
 */
int ParallelModel::addState(ParallelState *s) {
    // jeśli za mało miejsca to zwiększ
    if(counter == states.size()) {
        int ns = 2*states.size();
        states.resize(ns);
    }
    // id musi się zgadzać z miejscem
    s->id=counter;
    // dodaj do tablicy
    states[counter]=s;
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
    for(int n=0; n<counter; n++) {
        states[n]->clean();
    }
}

/* -------------------------------------------------------------------------
 * Połącz dwa węzły w klasę abstrakcji - podłącz klasę drugiego do pierwszego
 */
int ParallelModel::unify(int _id1, int _id2) {
    // ustaw - najpierw mniejszy, potem większy
    int id1=min(_id1, _id2);
    int id2=max(_id1, _id2);
    // W każdym przypadku przejdź do głowy
    int h1=findClass(id1);
    int h2=findClass(id2);

    // jeśli to samo to już nie trzeba łączyć
    if(h1 == h2 && h1 != -1) return h1;

    // jeśli pierwszy jeszcze nie był podpięty nigdzie, zainicjuj
    if (h1 == -1) {
        // przypisz siebie
        states[id1]->abstractClass = id1;
        h1=id1;
    }
    // to samo z drugim, zainicjuj
    if (h2 == -1) {
        h2=id2;
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
int ParallelModel::findClass(int id){
    // jeśli węzeł nie należy do żadnej klasy, zwróć -1
    if(states[id]->abstractClass == -1) return -1;
    // w p.p. cofnij się "do głowy"
    while(states[id]->abstractClass != id) id=states[id]->abstractClass;
    return id;
}

/* -------------------------------------------------------------------------
 */
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

/* -------------------------------------------------------------------------
 * Szeregowa metoda do poszukiwania strategii
 * s - węzeł do odwiedzenia
 * p - poprzedni węzeł (?)
 * mode - czy ma być przeszukiwanie ma być wznowione od ostatniej decyzji
 *          tylko do użytku wewnętrznego
 * threadId - id bieżącego wątku
 */
bool ParallelModel::recursiveDFS(int s, int p, operationMode mode, int threadId) {

    // zienna pomocnicza, żeby nie trzeba było ciągle dłubać
    ParallelState *currentState=states[s];

#ifdef __DEBUG__
    cout << *currentState << " " << p << " " << mode << endl;
#endif

    // jeśli stan jest wygrywajacy
    if(currentState->accept) {
        // zwróć odwrotność resume
        if(mode == resume) return false;
        else return true;
    }

#ifdef __DEBUG__
    cout << " $1(" << currentState->id << ")" << endl;
#endif

    // węzeł reprezentant klasy abstrakcji, jeśli jest
    ParallelState *headState=NULL;
    // co do akcji to sprawdzamy, czy nie jest to węzeł z jakiejś klasy abstrakcji
    if(currentState->abstractClass == -1) {
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
    if(mode == standard) {
        // czy stan był już odwiedzony pozytywnie
        if(currentState->opState == ParallelState::positive) {
            // jest OK, zwróć prawdę
            return true;
        }
#ifdef __DEBUG__
        cout << " $2.1(" << currentState->id << ")"  << endl;
#endif

        // w p.p. czy był odwiedzony w ogóle
        if(currentState->opState == ParallelState::searching) {
            // stan był odwiedzony ale nie jest zakończony
            // czy przez ten wątek?
            if(currentState->threadId == threadId) {
                // owszem - jest pętla, zwróć porażkę
                return false;
            }
        }
        // prawdopodobnie zbędny sanity check
        if(currentState->opState != ParallelState::clear) {
            // dziwne, to się nie powinno wydarzyć
            cerr << "model::recursiveDFS: I should not be here!" << endl;
            exit(1);
        }
        // stan jest czysty, trzeba zainicjować
        currentState->opState = ParallelState::searching;
        currentState->threadId = threadId;
        currentState->previousStateIdx = p;

#ifdef __DEBUG__
        cout << " $2.2(" << currentState->id << ")"  << endl;
#endif

        // co do akcji to sprawdzamy, czy nie jest to węzeł z jakiejś klasy abstrakcji
        if(currentState->abstractClass == -1) {
            // jeśli nie, to decydujemy o akcji lokalnie
            currentState->currentAction = 0;
        } else {
            // w p.p. zależymy od "głowy"
            // teraz, czy w gowie jest ustalona akcja
            if(headState->currentAction == -1) {
                // nie ma - to ją ustaw
                headState->currentAction = 0;
            } // w p.p. musimy uwzględnić akcję ustawioną w głowie

            // znajdź pierwszą akcję zgodną z tą w głowie
            if(currentState != headState) {
                currentState->currentAction = 0;
                // skanuj tablicę akcji aż znajdziesz akcję o takim samym identyfikatorze
                while(currentState->actions[currentState->currentAction] != headState->actions[headState->currentAction]) {
                    currentState->currentAction++;
                }
            }
            // zwiększamy licznik zajętości dla głowy
            headState->classCounter++;
        }


    } else {
        // flaga mode jest ustawiona, po prostu wchodzimy w pętlę
    }

#ifdef __DEBUG__
    cout << " $3(" << currentState->id << ")"  << endl;
#endif

    // pętla tak długo jak długo są jakieś akcje
    while(currentState->currentAction < currentState->counter) {
        int currentActionId = currentState->actions[currentState->currentAction];

#ifdef __DEBUG__
        cout << " $3.1(" << currentState->id << ")"  << endl;
#endif

        // rekurencyjnie po grafie
        bool result = recursiveDFS(currentState->edges[currentState->currentAction], s, resume, threadId);
        // skasuj resume
        mode = standard;

#ifdef __DEBUG__
        cout << " $3.2(" << currentState->id << ")"  << endl;
#endif

        if(result) {
#ifdef __DEBUG__
            cout << " $3.2.1(" << currentState->id << ")"  << endl;
#endif
            // chwilowo udało się znaleźć dobrą strategię
            if(currentState->currentAction+1 == currentState->counter ||
               currentActionId != currentState->actions[currentState->currentAction+1]) {
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
            if(currentState->currentAction == 0 ||
               currentActionId != currentState->actions[currentState->currentAction-1]) {

                /* Inaczej jak jest klasa abstrakcji */
                if(currentState->abstractClass != -1) {
#ifdef __DEBUG__
                    cout << " $3.2.2.1(" << currentState->id << ")"  << endl;
#endif
                    // jeśli aktywnych węzłów w klasie jest więcej niż 1, oznacza to, że ktoś w klasie jeszcze potrzebuje tej wartości
                    if(headState->classCounter > 1) {
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
                } while(currentState->currentAction < currentState->counter
                        && currentState->actions[currentState->currentAction] == currentActionId);
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
    ParallelState *currentState=states[s];

#ifdef __DEBUG__
    //     cout << *currentState << " " << p << " " << resume << " " << threadsVector.size() << endl;
#endif

#ifdef __DEBUG__
    trace(" #1",currentState->to_string());
#endif

    // jeśli stan jest wygrywajacy
    if(currentState->accept) {
        // zwróć odwrotność resume
        return (mode != resume);
    }

    // jeśli stan jest przegrywający
    mutexes[currentState->id % MUTEX_COUNT].lock();
    ParallelState::operation opState = currentState->opState;
    mutexes[currentState->id % MUTEX_COUNT].unlock();
    if(opState == ParallelState::negative || currentState->counter == 0) {
        // zwróć fałsz
        return false;
    }

    // węzeł reprezentant klasy abstrakcji, jeśli jest
    ParallelState *headState=NULL;
    // co do akcji to sprawdzamy, czy nie jest to węzeł z jakiejś klasy abstrakcji
    if(currentState->abstractClass == -1) {
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
    if(mode == standard) {

        // -------------------------------------------------------
        // teraz zablokuj węzeł
        mutexes[currentState->id % MUTEX_COUNT].lock();
        // -------------------------------------------------------

        // czy stan był już odwiedzony pozytywnie
        if(currentState->opState == ParallelState::positive) {
            // jest OK, zwróć prawdę
            mutexes[currentState->id % MUTEX_COUNT].unlock();
            return true;
        }
#ifdef __DEBUG__
        trace(" #2.1",currentState->to_string());
#endif

        // w p.p. czy był odwiedzony w ogóle
        if(currentState->opState == ParallelState::searching) {
            // stan był odwiedzony ale nie jest zakończony
            // czy przez ten wątek?
            if(currentState->threadId == threadId) {
                // owszem - jest pętla, zwróć porażkę
                mutexes[currentState->id % MUTEX_COUNT].unlock();
                return false;
            } else {
                // w p.p. inny wątek tu grzebał - trzeba przejąć

                // sprawdź, czy ten wątek jeszcze działa
                if(threadsVector[currentState->threadId] != NULL) {
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
        if(mode != follow) {
            // co do akcji to sprawdzamy, czy nie jest to węzeł z jakiejś klasy abstrakcji
            if(currentState->abstractClass == -1) {
                // jeśli nie, to decydujemy o akcji lokalnie
                currentState->currentAction = 0;
            } else {
                // w p.p. zależymy od "głowy"
                // teraz, czy w gowie jest ustalona akcja
                if(headState->currentAction == -1) {
                    // nie ma - to ją ustaw
                    headState->currentAction = 0;
                } // w p.p. musimy uwzględnić akcję ustawioną w głowie

                // znajdź pierwszą akcję zgodną z tą w głowie
                if(currentState != headState) {
                    currentState->currentAction = 0;
                    // skanuj tablicę akcji aż znajdziesz akcję o takim samym identyfikatorze
                    while(currentState->actions[currentState->currentAction] != headState->actions[headState->currentAction]) {
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
    while(currentState->currentAction < currentState->counter) {
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
            while(joinQueue.size() > 0) {
                // posprzątaj
                int t=joinQueue.front();
                joinQueue.pop();
                
                threadsVector[t]->join();
                threadsRunning--;
                threadsVector[t] = NULL;
//                 cout << "% sprzątnąłem " << t << endl;
            }
            joinQueueMutex.unlock();
            
            // czy nie za dużo...
            if(threadsRunning < MAX_THREADS) {
                // utwórz nowy wątek
                thread *t = new thread(recursiveHelperThreadStart, currentState->edges[currentState->currentAction + 1], s,
                                    threadsStarted, this);
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
//                         cout << "# " << threadsStarted << endl;
                        // jeśli już koniec tablicy, zwiększ ją 2x
                        threadsVector.resize(threadsStarted * 2);
//                         cout << "# " << threadsStarted << " OK" << endl;
                    }
#ifdef __DEBUG__
                    trace(" #3.1.3",currentState->to_string());
#endif
                }
            }
        }

#endif






        // rekurencyjnie po grafie
        bool result = parallelRecursiveDFS(currentState->edges[currentState->currentAction], s, mode, threadId);
        // skasuj resume
        mode = standard;

#ifdef __DEBUG__
        trace(" #3.2",currentState->to_string());
#endif

        if(result) {
#ifdef __DEBUG__
            trace(" #3.2.1",currentState->to_string());
#endif
            // chwilowo udało się znaleźć dobrą strategię
            if(currentState->currentAction+1 == currentState->counter ||
               currentActionId != currentState->actions[currentState->currentAction+1]) {
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
            if(currentState->currentAction == 0 ||
               currentActionId != currentState->actions[currentState->currentAction-1]) {

                /* Inaczej jak jest klasa abstrakcji */
                if(currentState->abstractClass != -1) {
#ifdef __DEBUG__
                    trace(" #3.2.2.1",currentState->to_string());
#endif
                    // jeśli aktywnych węzłów w klasie jest więcej niż 1, oznacza to, że ktoś w klasie jeszcze potrzebuje tej wartości
                    if(headState->classCounter > 1) {
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
                } while(currentState->currentAction < currentState->counter
                        && currentState->actions[currentState->currentAction] == currentActionId);
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
    bool res=recursiveHelperThread(s, p, standard, threadId, m);
    
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
    ParallelState *currentState=m->states[s];

#ifdef __DEBUG__
    trace("  %1\tw"+to_string(threadId),currentState->to_string());
#endif

    // jeśli stan jest wygrywajacy
    if(currentState->accept) {
        // zwróć odwrotność resume
        return (mode != resume);
    }

    // jeśli stan jest przegrywający
    m->mutexes[currentState->id % MUTEX_COUNT].lock();
    ParallelState::operation opState = currentState->opState;
    m->mutexes[currentState->id % MUTEX_COUNT].unlock();
    if(opState == ParallelState::negative || currentState->counter == 0) {
        // zwróć fałsz
        return false;
    }

    // jeśli to jest węzeł z jakiejś klasy epistemicznej - kończymy
    if(currentState->abstractClass != -1) {
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
    if(mode == standard) {

        // -------------------------------------------------------
        // teraz zablokuj węzeł
        m->mutexes[currentState->id % MUTEX_COUNT].lock();
        // -------------------------------------------------------

        // jeśli sam tu byłem
        if(currentState->threadId == threadId) {

            // czy stan był już odwiedzony pozytywnie
            if(currentState->opState == ParallelState::positive) {
                // jest OK, zwróć prawdę
                m->mutexes[currentState->id % MUTEX_COUNT].unlock();
                return true;
            }

            // w p.p. czy był odwiedzony w ogóle
            if(currentState->opState == ParallelState::searching) {
                // stan był odwiedzony ale nie jest zakończony - jest pętla, zwróć porażkę
                m->mutexes[currentState->id % MUTEX_COUNT].unlock();
                return false;
            }
            // tutaj coś jest nie w porządku bo jest za późno i nie wiem co wpisać
            cerr << "model::recursiveHelperThread: co mam zrobić? " << threadId << endl;
            exit(1);
        }
        // czy ktoś inny tu nie rządzi
        if(currentState->threadId > -1) {
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
    while(currentState->currentAction < currentState->counter) {
        int currentActionId = currentState->actions[currentState->currentAction];

        // rekurencyjnie po grafie
        bool result = recursiveHelperThread(currentState->edges[currentState->currentAction], s, mode, threadId, m);
        // skasuj resume
        mode = standard;

        if(result) {

            // chwilowo udało się znaleźć dobrą strategię
            if(currentState->currentAction+1 == currentState->counter ||
               currentActionId != currentState->actions[currentState->currentAction+1]) {
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
            if(currentState->currentAction == 0 ||
               currentActionId != currentState->actions[currentState->currentAction-1]) {

                // zaliczono wpadkę, idziemy do kolejnej akcji (lub na koniec)
                do {
                    currentState->currentAction++;
                } while(currentState->currentAction < currentState->counter
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
    m->mutexes[currentState->id % MUTEX_COUNT].unlock();
    return false;
}

/* -------------------------------------------------------------------------
 */
ostream& operator<<(ostream &str, ParallelModel &m) {
    for(int i=0; i<m.counter; i++) {
        str << *(m.states[i]) << endl;
    }
    return str;
}

/* -------------------------------------------------------------------------
 * Funkcja do wyplucia strategii wygrywającej.
 * s : numer stanu do rozpoczęcia wypisywania
 * str : strumień wyjściowy
 */
void ParallelModel::printStrategy(int s, ostream &str){
    // wektor odwiedzin stanów
    vector<bool> visited;
    visited.resize(states.size(), false);
    queue<int> pending;
    
    // wstaw stan startowy
    visited[s]=true;
    pending.push(s);
    
    str << "***********************" << endl;
    // pętla po stanach do odwiedzenia
    while(pending.size() > 0) {
        // weź stan bieżący
        ParallelState *currentState = states[pending.front()];
        pending.pop();
        
        // "nagłówek"
        str << "state " << currentState->id << ":";
        // jeśli wygrywający
        if(currentState->accept) {
            str << " accept" << endl;
            continue;
        }
    
        // jeśli nie ma akcji
        if(currentState->counter == 0) {
            str << " fail" << endl;
            continue;
        }
        // wybrana akcja
        int currentAction;
        int currentClass=findClass(currentState->id);
        if(currentClass == -1) {
            currentAction=currentState->actions[currentState->currentAction];
        } else {
            currentAction=states[currentClass]->actions[states[currentClass]->currentAction];
        }
        str << " action " << currentAction << endl;
        
        // nie jest wygrywający - jedziemy po następnikach
        // przejście przez tablicę akcji
        for(int i=0; i < currentState->counter; i++) {
            // akcja dla krawędzi zgadza się z bieżącą
            if(currentState->actions[i] == currentAction) {
                //wypluj na wyjście
                int destState=currentState->edges[i];
                str << " -> " << destState << endl;
                // czy docelowy stan jest już odwiedzony
                if(! visited[destState]) {
                    // nieodwiedzony - dorzuć do zbioru
                    visited[destState] = true;
                    pending.push(destState);
                }
            }
        }
    }
    str << "***********************" << endl;
}
