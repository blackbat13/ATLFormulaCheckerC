//
// Created by Damian Kurpiewski on 18.03.2017.
//

#include "DisjointUnion.h"

DisjointUnion::DisjointUnion(int numberOfNodes) {
    this->subsets = std::vector<Element>((unsigned long) numberOfNodes);
    for (int i = 0; i < numberOfNodes; ++i) {
        this->subsets[i] = Element(i, 0);
    }
}

DisjointUnion::DisjointUnion(std::vector<DisjointUnion::Element> subsets): subsets(subsets) {}

int DisjointUnion::find(int nodeNumber) {
    if (this->subsets[nodeNumber].parent != nodeNumber) {
        this->subsets[nodeNumber].parent = this->find(this->subsets[nodeNumber].parent);
    }

    return this->subsets[nodeNumber].parent;
}

void DisjointUnion::unionn(int x, int y) {
    int xRoot = this->find(x);
    int yRoot = this->find(y);

    if (xRoot == yRoot) {
        return;
    }

    if (this->subsets[xRoot].rank < this->subsets[yRoot].rank) {
        this->subsets[xRoot].parent = yRoot;
    } else if (this->subsets[xRoot].rank > this->subsets[yRoot].rank) {
        this->subsets[yRoot].parent = xRoot;
    } else {
        this->subsets[yRoot].parent = xRoot;
        this->subsets[xRoot].rank += 1;
    }
}

bool DisjointUnion::isSame(int x, int y) {
    return this->find(x) == this->find(y);
}

DisjointUnion::~DisjointUnion() {
    this->subsets.clear();
}

const std::vector<DisjointUnion::Element> &DisjointUnion::getSubsets() const {
    return subsets;
}

DisjointUnion::Element::Element(int parent, int rank) : parent(parent), rank(rank) {}

DisjointUnion::Element::Element() {}
