//
// Created by black on 14.12.2020.
//

#include "LocalModelParser.h"

LocalModel LocalModelParser::parse(int agentId, std::string modelStr, int agentNo) {
    return LocalModel(0, __cxx11::basic_string(), std::map(), std::vector(), std::map(), std::set());
}

bool LocalModelParser::isProtocolLine(std::string line) {
    return line.substr(0, 8) == "PROTOCOL";
}

std::pair<std::string, std::vector<std::vector<std::string> > > LocalModelParser::parseProtocol(std::string line) {
    auto line2 = StringTools::split(line, ':');
    auto state = StringTools::split(line2[0], ' ')[1];
    auto protocol = parseProtocolList(line2[1]);
    return std::make_pair(state, protocol);
}

std::vector<std::vector<std::string> > LocalModelParser::parseProtocolList(std::string line) {
    std::vector<std::vector<std::string> > protocol;
    std::string line2 = StringTools::strip(line, ' ');
    line2 = StringTools::strip(line2, '[');
    line2 = StringTools::strip(line2, ']');
    auto split1 = StringTools::split(']');
    for(auto arr : split1) {
        auto arr2 = StringTools::strip(arr, ' ');
        arr2 = StringTools::strip(arr2, ',');
        arr2 = StringTools::strip(arr2, '[');
        arr2 = StringTools::strip(arr2, ']');
        std::vector<std::string> lst;
        auto split2 = StringTools::split(arr2, ',');
        for(auto element : split2) {
            lst.push_back(StringTools::strip(element, ' '));
        }

        protocol.push_back(lst);
    }

    return protocol;
}
