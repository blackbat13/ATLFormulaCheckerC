//
// Created by black on 14.12.2020.
//

#include "LocalModelParser.h"

LocalModel LocalModelParser::parse(int agentId, std::string modelStr, int agentNo) {
    auto lines = StringTools::splitLines(modelStr);
    std::string agentName = StringTools::split(StringTools::split(lines[0], ' ')[1], '[')[0] + to_string(agentNo);
    std::string initState = StringTools::split(lines[1], ' ')[1];
    std::map<std::string, int> states;
    states[initState] = 0;
    std::vector<std::vector<std::string> >  protocol;
    std::set<std::string> actions;
    std::vector<std::vector<LocalTransition*> > transitions;
    int stateNum = 1;
    int transitionId = 0;
    for (int i = 2; i < lines.size(); i++) {
        if(lines[i].length() <= 1) {
            break;
        }

        std::string line = StringTools::strip(lines[i], ' ');

        line = StringTools::replace(line, "aID", agentName);
        if (isProtocolLine(line)) {
            protocol = parseProtocol(line);
            continue;
        }

        auto localTransition = LocalTransitionParser::parse(line);
        localTransition->setId(transitionId);
        localTransition->setAgentId(agentId);
        transitionId++;
        if (!localTransition->isShared()) {
            localTransition->setAction(localTransition->getAction() + "_" + agentName);
        }

        actions.insert(localTransition->getAction());
        auto stateFrom = localTransition->getStateFrom();
        auto stateTo = localTransition->getStateTo();
        if (states.find(stateFrom) == states.end()) {
            states[stateFrom] = stateNum;
            stateNum++;
        }

        if (states.find(stateTo) == states.end()) {
            states[stateTo] = stateNum;
            stateNum++;
        }

        while (transitions.size() <= states[stateFrom]) {
            transitions.emplace_back();
        }

        transitions[states[stateFrom]].push_back(localTransition);
    }

    while (transitions.size() < states.size()) {
        transitions.emplace_back();
    }

    return LocalModel(agentId, agentName, states, transitions, protocol, actions);
}

bool LocalModelParser::isProtocolLine(std::string line) {
    return line.substr(0, 8) == "PROTOCOL";
}

std::vector<std::vector<std::string> >  LocalModelParser::parseProtocol(std::string line) {
    auto line2 = StringTools::split(line, ':');
    auto protocol = parseProtocolList(line2[1]);
    return protocol;
}

std::vector<std::vector<std::string> > LocalModelParser::parseProtocolList(std::string line) {
    std::vector<std::vector<std::string> > protocol;
    std::string line2 = StringTools::strip(line, ' ');
    line2 = StringTools::strip(line2, '[');
    line2 = StringTools::strip(line2, ']');
    auto split1 = StringTools::split(line2, "],");
    for (auto arr : split1) {
        auto arr2 = StringTools::strip(arr, ' ');
        arr2 = StringTools::strip(arr2, '[');
        arr2 = StringTools::strip(arr2, ']');
        std::vector<std::string> lst;
        auto split2 = StringTools::split(arr2, ',');
        for (auto element : split2) {
            lst.push_back(StringTools::strip(element, ' '));
        }

        protocol.push_back(lst);
    }

    return protocol;
}
