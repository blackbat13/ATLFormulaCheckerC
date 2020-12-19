//
// Created by black on 14.12.2020.
//

#ifndef ATLFORMULACHECKER_SHAREDTRANSITION_H
#define ATLFORMULACHECKER_SHAREDTRANSITION_H

#include "LocalTransition.h"
#include <vector>

class SharedTransition: public LocalTransition {
public:
    std::vector<LocalTransition*> transitionList;

    bool isShared() const;

    explicit SharedTransition(LocalTransition* localTransition);

    void addTransition(LocalTransition* localTransition);

    bool operator==(const SharedTransition &rhs) const;

    bool operator!=(const SharedTransition &rhs) const;

    bool operator<(const SharedTransition &rhs) const;

    bool operator>(const SharedTransition &rhs) const;

    bool operator<=(const SharedTransition &rhs) const;

    bool operator>=(const SharedTransition &rhs) const;
};


#endif //ATLFORMULACHECKER_SHAREDTRANSITION_H
