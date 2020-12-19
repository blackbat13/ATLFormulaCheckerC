#ifndef __PARALLEL_MODEL_HPP
#define __PARALLEL_MODEL_HPP

#include <iostream>
#include <vector>

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

    // konstruktor
    ParallelState(int _id=0);

    // dodaj tranzycję z bieżącego węzła do innego przy zadanej akcji
    virtual int addTransition(int action, int dest);

    inline void clean(){
        opState = clear;
        threadId = -1;
        currentAction = -1;
        previousStateIdx = -1;
    };

    friend ostream& operator<<(ostream &str, ParallelState &s);
};

class ParallelModel {
public:
    int counter;
    vector<ParallelState*> states;

    // tworzy pusty model
    ParallelModel();
    // tworzy model o zadanej liczbie węzłów
    explicit ParallelModel(int size);

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

    virtual bool recursiveDFS(int s, int p, bool resume, int threadId);

    friend ostream& operator<<(ostream &str, ParallelModel &m);
};

#endif // __PARALLEL_MODEL_HPP
