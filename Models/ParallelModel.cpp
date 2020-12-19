#include "ParallelModel.hpp"

using namespace std;

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

/* -------------------------------------------------------------------------
 */
ostream& operator<<(ostream &str, ParallelState &s) {
    str << s.id << ":" << s.accept << ":" << s.abstractClass << "(";
    for(int i=0; i<s.counter; i++) {
        str << "(" << s.actions[i] << "," << s.edges[i] << ")";
    }
    str << "):" << s.currentAction;
    return str;
}


/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * Metody dla klasy opisującej model
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

/* -------------------------------------------------------------------------
 */
ParallelModel::ParallelModel() {
    counter=0;
    states.resize(2);
}

/* -------------------------------------------------------------------------
 */
ParallelModel::ParallelModel(int size) {
    counter = size;
    states.resize(counter);
    for(int i = 0; i<counter; i++) {
        states[i] = new ParallelState(i);
    }
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
    auto *s = new ParallelState(counter);
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
 * Metoda do poszukiwania strategii
 * s - węzeł do odwiedzenia
 * p - poprzedni węzeł (?)
 * resume - czy ma być przeszukiwanie ma być wznowione od ostatniej decyzji
 * threadId - id bieżącego wątku
 */
bool ParallelModel::recursiveDFS(int s, int p, bool resume, int threadId) {
    // zienna pomocnicza, żeby nie trzeba było ciągle dłubać
    auto *currentState=states[s];

#ifdef __DEBUG__
    cout << *currentState << " " << p << " " << resume << endl;
#endif

    // jeśli stan jest wygrywajacy
    if(currentState->accept)
        // zwróć odwrotność resume
        return !resume;

#ifdef __DEBUG__
    cout << " #1(" << currentState->id << ")" << endl;
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
    cout << " #2(" << currentState->id << ")"  << endl;
#endif

    // jeśli normalna akcja
    if(! resume) {
        // czy stan był już odwiedzony pozytywnie
        if(currentState->opState == ParallelState::positive) {
            // jest OK, zwróć prawdę
            return true;
        }
#ifdef __DEBUG__
        cout << " #2.1(" << currentState->id << ")"  << endl;
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
        cout << " #2.2(" << currentState->id << ")"  << endl;
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
        if(currentState->currentAction == -1){
            //return false;
            currentState->currentAction = 0;
        }
        // flaga resume jest ustawiona, po prostu wchodzimy w pętlę
    }

#ifdef __DEBUG__
    cout << " #3(" << currentState->id << ")"  << endl;
#endif

    // pętla tak długo jak długo są jakieś akcje
    while(currentState->currentAction < currentState->counter) {
        int currentActionId = currentState->actions[currentState->currentAction];

#ifdef __DEBUG__
        cout << " #3.1(" << currentState->id << ")"  << endl;
#endif

        // rekurencyjnie po grafie
        bool result = recursiveDFS(currentState->edges[currentState->currentAction], s, resume, threadId);
        // skasuj resume
        resume = false;

#ifdef __DEBUG__
        cout << " #3.2(" << currentState->id << ")"  << endl;
#endif

        if(result) {
#ifdef __DEBUG__
            cout << " #3.2.1(" << currentState->id << ")"  << endl;
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
            cout << " #3.2.2(" << *currentState << ")"  << endl;
#endif
            // jeśli poprzedniej akcji nie ma lub jest ona różna od bieżącej
            if(currentState->currentAction == 0 ||
               currentActionId != currentState->actions[currentState->currentAction-1]) {

                /* Inaczej jak jest klasa abstrakcji */
                if(currentState->abstractClass != -1) {
#ifdef __DEBUG__
                    cout << " #3.2.2.1(" << currentState->id << ")"  << endl;
#endif
                    // jeśli aktywnych węzłów w klasie jest więcej niż 1, oznacza to, że ktoś w klasie jeszcze potrzebuje tej wartości
                    if(headState->classCounter > 1) {
                        // a to oznacza porażkę - powróć
                        // zmniejsz licznik w headState
                        headState->classCounter--;
                        // czyścimy
                        currentState->clean();
#ifdef __DEBUG__
                        cout << " #3.2.2.1.1(" << currentState->id << ")"  << endl;
#endif
                        // zwróć porażkę
                        return false;
                    } else {
#ifdef __DEBUG__
                        cout << " #3.2.2.1.2(" << currentState->id << ")"  << endl;
#endif
                        // w p.p. to był jedyny węzeł z tej klasy i można kontynuować
                    }
                }
#ifdef __DEBUG__
                cout << " #3.2.2.2(" << currentState->id << ")"  << endl;
#endif
                // zaliczono wpadkę, idziemy do kolejnej akcji (lub na koniec)
                do {
                    currentState->currentAction++;
                } while(currentState->currentAction < currentState->counter
                        && currentState->actions[currentState->currentAction] == currentActionId);
                // resztę załatwia pętla
#ifdef __DEBUG__
                cout << " #3.2.2.3(" << currentState->id << ")"  << endl;
#endif
                continue;
            }
            // poprzednia akcja jest taka sama - trzeba się w niej cofnąć
            resume = true;
            currentState->currentAction--;
            // resztę załatwia pętla
#ifdef __DEBUG__
            cout << " #3.2.3(" << currentState->id << ")"  << endl;
#endif
            continue;
        }
#ifdef __DEBUG__
        cout << " #3.3(" << currentState->id << ")"  << endl;
#endif
    }

#ifdef __DEBUG__
    cout << " #4(" << currentState->id << ")"  << endl;
#endif
    // nie udało się znaleźć akcji prowadzącej do wygranej
    // zmniejsz licznik w headState
    headState->classCounter--;
    // przyznaj się do porażki
    currentState->clean();
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

