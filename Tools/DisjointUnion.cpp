//
// Created by Damian Kurpiewski on 18.03.2017.
//

#include "DisjointUnion.h"

DisjointUnion::DisjointUnion(unsigned int numberOfNodes) {
    this->subsets = std::vector<Element>((unsigned long) numberOfNodes);
    for (unsigned int i = 0; i < numberOfNodes; ++i) {
        this->subsets[i] = Element(i, 0);
    }
}

DisjointUnion::DisjointUnion(std::vector<DisjointUnion::Element> subsets): subsets(subsets) {}

unsigned int DisjointUnion::find(unsigned int nodeNumber) {
    if (this->subsets[nodeNumber].parent != nodeNumber) {
        this->subsets[nodeNumber].parent = this->find(this->subsets[nodeNumber].parent);
    }

    return this->subsets[nodeNumber].parent;
}

void DisjointUnion::unionn(unsigned int x, unsigned int y) {
    auto xRoot = this->find(x);
    auto yRoot = this->find(y);

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

bool DisjointUnion::isSame(unsigned int x, unsigned int y) {
    return this->find(x) == this->find(y);
}

DisjointUnion::~DisjointUnion() {
    this->subsets.clear();
}

const std::vector<DisjointUnion::Element> &DisjointUnion::getSubsets() const {
    return subsets;
}

DisjointUnion::Element::Element(unsigned int parent, unsigned int rank) : parent(parent), rank(rank) {}

DisjointUnion::Element::Element() {}
