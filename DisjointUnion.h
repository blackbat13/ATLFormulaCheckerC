//
// Created by Damian Kurpiewski on 18.03.2017.
//

#ifndef ATLFORMULACHECKER_DISJOINTUNION_H
#define ATLFORMULACHECKER_DISJOINTUNION_H

#include <vector>

class DisjointUnion {
    struct Element {
        int parent;
        int rank;

        Element(int parent, int rank);

        Element();
    };

    std::vector<Element> subsets;
public:

    DisjointUnion(int numberOfNodes);

    DisjointUnion(std::vector<Element> subsets);

    int find(int nodeNumber);

    void unionn(int x, int y);

    bool isSame(int x, int y);

    const std::vector<Element> &getSubsets() const;

    ~DisjointUnion();
};


#endif //ATLFORMULACHECKER_DISJOINTUNION_H
