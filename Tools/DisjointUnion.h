//
// Created by Damian Kurpiewski on 18.03.2017.
//

#ifndef ATLFORMULACHECKER_DISJOINTUNION_H
#define ATLFORMULACHECKER_DISJOINTUNION_H

#include <vector>

class DisjointUnion {
    struct Element {
        unsigned int parent;
        unsigned int rank;

        Element(unsigned int parent, unsigned int rank);

        Element();
    };

    std::vector<Element> subsets;
public:

    DisjointUnion(unsigned int numberOfNodes);

    DisjointUnion(std::vector<Element> subsets);

    unsigned int find(unsigned int nodeNumber);

    void unionn(unsigned int x, unsigned int y);

    bool isSame(unsigned int x, unsigned int y);

    const std::vector<Element> &getSubsets() const;

    ~DisjointUnion();
};


#endif //ATLFORMULACHECKER_DISJOINTUNION_H
