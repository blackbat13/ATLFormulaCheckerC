//
// Created by black on 14.12.2020.
//

#ifndef ATLFORMULACHECKER_LOCALMODELPARSER_H
#define ATLFORMULACHECKER_LOCALMODELPARSER_H

#include "../LocalTransition.h"
#include "../LocalModel.h"
#include "LocalTransitionParser.h"
#include "../../../Tools/StringTools.h"

#include <vector>
#include <map>
#include <set>
#include <string>

class LocalModelParser {
private:
public:
    static LocalModel parse(int agentId, std::string modelStr, int agentNo);
    static bool isProtocolLine(std::string line);
    static std::pair<std::string, std::vector<std::vector<std::string> > > parseProtocol(std::string line);
    static std::vector<std::vector<std::string> > parseProtocolList(std::string line);
};


#endif //ATLFORMULACHECKER_LOCALMODELPARSER_H
