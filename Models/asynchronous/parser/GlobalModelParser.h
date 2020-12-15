//
// Created by black on 14.12.2020.
//

#ifndef ATLFORMULACHECKER_GLOBALMODELPARSER_H
#define ATLFORMULACHECKER_GLOBALMODELPARSER_H

#include "../GlobalModel.h"
#include "LocalModelParser.h"
#include "../../../Tools/StringTools.h"

#include <vector>
#include <string>
#include <fstream>

class GlobalModelParser {
public:
    static GlobalModel parse(std::string fileName);
    static bool isAgentHeader(std::string line);
    static bool isReductionHeader(std::string line);
    static bool isPersistentHeader(std::string line);
    static bool isCoalitionHeader(std::string line);
    static bool isGoalHeader(std::string line);
    static std::vector<std::string> parseList(std::string line);
    static int parseAgentMax(std::string line);
    static int findAgentEnd(std::vector<std::string> lines, int lineIndex);
};


#endif //ATLFORMULACHECKER_GLOBALMODELPARSER_H
