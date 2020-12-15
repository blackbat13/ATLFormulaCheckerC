//
// Created by black on 14.12.2020.
//

#ifndef ATLFORMULACHECKER_LOCALTRANSITIONPARSER_H
#define ATLFORMULACHECKER_LOCALTRANSITIONPARSER_H

#include "../LocalTransition.h"
#include <string>

class LocalTransitionParser {
private:
public:
    static LocalTransition parse(std::string transitionStr);
};


#endif //ATLFORMULACHECKER_LOCALTRANSITIONPARSER_H
