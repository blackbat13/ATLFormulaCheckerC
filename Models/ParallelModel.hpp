#ifndef __MODEL_HPP
#define __MODEL_HPP

#include <iostream>
#include <vector>
#include <thread>
#include <pthread.h>
#include <mutex>

using namespace std;

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

    inline void clean(){
        opState = clear;
        threadId = -1;
        currentAction = 0;
        previousStateIdx = -1;
    };

    friend ostream& operator<<(ostream &str, ParallelState &s);

    inline string to_string();
};

#define MUTEX_COUNT 10

class ParallelModel {

public:
    // tryb pracy (było bool potrzeba więcej)
    typedef enum {standard, resume, follow} operationMode;

protected:

    int counter;

    // wektor muteksów dla dostępu do modelu
    mutex mutexes[MUTEX_COUNT];
    // bieżący licznik uruchomionych
    int threadsStarted;

    // metoda dla wątków pomocniczych
    static bool recursiveHelperThread(int s, int p, operationMode mode, int threadId, ParallelModel *m);

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
    virtual void cleanPath(int s, int p, int threadId);

    bool recursiveDFS(int s, int p, operationMode mode, int threadId);

    virtual bool parallelRecursiveDFS(int s, int p, operationMode mode, int threadId);

    friend ostream& operator<<(ostream &str, ParallelModel &m);
};
#endif // __MODEL_HPP
