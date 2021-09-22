#ifndef __MODEL_HPP
#define __MODEL_HPP

#include <iostream>
#include <vector>
#include <queue>
#include <deque>
#include <thread>
#include <set>
#include <pthread.h>
#include <mutex>

using namespace std;

/* -------------------------------------------------------------------------
 * Klasa opisująca pojedynczy stan w modelu, wykorzystywana do równoległych
 * algorytmów poszukiwania strategii.
 */

class ParallelState {
    
public:
    // jednak potrzebny identyfikator
    int id;

    int abstractClass; // -1 jeśli nie ma klasy, id głowy jeśli jest
    int classCounter;  // potrzebne tylko dla głowy w trakcie pracy

    int counter;
    vector<int> actions;
    vector<int> edges;

    int currentAction;
    int previousStateIdx;

    bool accept;
    // ustaw akceptację
    inline void setAccept() { accept = true;};

    typedef enum {clear, searching, positive, negative} operation;
    operation opState;

    // identyfikator wątka obrabiającego stan (-1 == żaden)
    int threadId;
    // przy okazji wskaźnik
    thread *threadPtr;

    // konstruktor
    explicit ParallelState(int _id=0);

    // dodaj tranzycję z bieżącego węzła do innego przy zadanej akcji
    virtual int addTransition(int action, int dest);

    virtual int findAction(int action); // zlokalizuj pierwsze wystąpienie zadanej akcji w stanie

    inline void clean(){
        opState = clear;
        threadId = -1;
        currentAction = -1;
        previousStateIdx = -1;
    };

    friend ostream& operator<<(ostream &str, ParallelState &s);

    inline string to_string();
};

/* -------------------------------------------------------------------------
 * Klasa pomocnicza określająca sekwencję akcji.
 * Jest to w zasadzie kontener na prefiks strategii.
 */
typedef class deque<int> ActionPath;
void dumpActionPath(ActionPath p, ostream &str);

/* -------------------------------------------------------------------------
 * Klasa modelu równoległego.
 */

#define MUTEX_COUNT 10
#define MAX_THREADS 4

class ParallelModel {

public:
    // tryb pracy (było bool potrzeba więcej)
    typedef enum {standard, resume, follow} operationMode;

    // lista wątków oczekujących na wyłapanie przez join
    queue<int> joinQueue;
    // mutex do ochrony
    mutex joinQueueMutex;
    // bieżący licznik uruchomionych
    int threadsStarted;    
    
protected:

    int counter;

    // wektor muteksów dla dostępu do modelu
    mutex mutexes[MUTEX_COUNT];
    // bieżący licznik działających
    int threadsRunning;
    // // mutex dostępowy do tych liczników
    mutex threadsMutex;

    // metoda dla wątków pomocniczych
    static bool recursiveHelperThread(int s, int p, operationMode mode, int threadId, ParallelModel *m);
    static bool recursiveHelperThreadStart(int s, int p, int threadId, ParallelModel *m);

private:
    // przejdź po ścieżce, jeśli jest możliwe i zwróc stan na którym się ona kończy
    virtual int followPath(int s, ActionPath path);
    // wyczyść ścieżkę, jeśli to możliwe
    virtual int resetPath(int s, ActionPath path);
    // wyszukaj prefiksy dla zrównoleglenia
    virtual int computePaths(int s, int n, queue<ActionPath> &prefixes);
    virtual int computePaths(int s, ActionPath start, int n, queue<ActionPath> &prefixes);
    
    // metody umożliwiające łatwiejszy dostęp do akcji w stanie - ze względu na klasy równoważności
    virtual int getAction(ParallelState *s);
    virtual void setAction(ParallelState *s, int action);
    virtual void clearAction(ParallelState *s);
    
public:

    // wektor stanów
    vector<ParallelState*> states;
    vector<thread*> threadsVector;

    // tworzy model o zadanej liczbie węzłów
    explicit ParallelModel(int size = 0);

    // utwórz i dodaj nowy węzeł (automatyczna numeracja)
    virtual int addNewState();
    // dodaj zadany węzeł (automatyczna numeracja)
    virtual int addState(ParallelState* s);

    /* Połącz dwa węzły w klasę abstrakcji - podłącz drugi do pierwszego */
    virtual int unify(int _id1, int _id2);
    // odszukaj węzła reprezentanta danej klasy abstrakcji
    virtual int findClass(int id);

    virtual void cleanAll();
//     virtual void cleanPath(int s, int p, int threadId);

    virtual bool forkRecursiveDFS(int s);
    virtual bool forkRecursiveDFS(int s, int n);

    virtual bool recursiveDFS(int s, int p, operationMode mode, int action = -1);
    
    virtual bool parallelRecursiveDFS(int s, int p, operationMode mode, int threadId);

    friend ostream& operator<<(ostream &str, ParallelModel &m);
    
    // wypluj strategię wygrywającą - jeśli istnieje
    virtual void printStrategy(int s, ostream &str);

};
#endif // __MODEL_HPP
