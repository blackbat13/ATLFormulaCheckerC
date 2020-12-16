//
// Created by black on 14.12.2020.
//

#include "GlobalModelParser.h"

GlobalModel GlobalModelParser::parse(std::string fileName) {
    auto inputFile = ifstream (fileName);
    std::string line;
    std::vector<std::string> lines;
    std::vector<LocalModel> localModels;
    std::vector<std::string> reduction;
    std::vector<std::string> persistent;
    std::vector<std::string> coalition;
    std::vector<std::string> goal;

    while(std::getline(inputFile, line)) {
        lines.push_back(line + "\n");
    }

    inputFile.close();

    int i = 0;
    while(i < lines.size()) {
        if(StringTools::isBlankLine(lines[i])) {
            i++;
            continue;
        }

        if(isAgentHeader(lines[i])) {
            int lineFrom = i;
            i = findAgentEnd(lines, i+1);
            int lineTo = i;
            int agentMax = parseAgentMax(lines[lineFrom]);
            for(int agentId = 1; agentId <= agentMax; agentId++) {
                auto localModel = LocalModelParser::parse(localModels.size(), StringTools::join(lines, lineFrom, lineTo), agentId);
                localModels.push_back(localModel);
            }
        } else if(isReductionHeader(lines[i])) {
            reduction = parseList(lines[i]);
            i++;
        } else if(isPersistentHeader(lines[i])) {
            persistent = parseList(lines[i]);
            i++;
        } else if(isCoalitionHeader(lines[i])) {
            coalition = parseList(lines[i]);
            i++;
        } else if(isGoalHeader(lines[i])) {
            goal = parseList(lines[i]);
            i++;
        }
    }

    return GlobalModel(localModels, reduction, persistent, coalition, goal);
}

bool GlobalModelParser::isAgentHeader(std::string line) {
    return line.substr(0, 5) == "Agent";
}

bool GlobalModelParser::isReductionHeader(std::string line) {
    return line.substr(0, 9) == "REDUCTION";
}

bool GlobalModelParser::isPersistentHeader(std::string line) {
    return line.substr(0, 10) == "PERSISTENT";
}

bool GlobalModelParser::isCoalitionHeader(std::string line) {
    return line.substr(0, 9) == "COALITION";
}

bool GlobalModelParser::isGoalHeader(std::string line) {
    return line.substr(0, 4) == "GOAL";
}

std::vector<std::string> GlobalModelParser::parseList(std::string line) {
    std::string line2 = StringTools::split(line, ':')[1];
    line2 = StringTools::strip(line2, ' ');
    line2 = StringTools::strip(line2, '[');
    StringTools::strip(line2, ']');
    std::vector<std::string> red;
    auto split1 = StringTools::split(line2, ',');
    red.reserve(split1.size());
    for (const auto& element: split1) {
        red.push_back(StringTools::strip(element, ' '));
    }

    return red;
}

int GlobalModelParser::parseAgentMax(std::string line) {
    auto split1 = StringTools::split(line, '[');
    if (split1.size() > 1) {
        auto split2 = StringTools::split(split1[1], ']');
        return stoi(split2[0]);
    }

    return 1;
}

int GlobalModelParser::findAgentEnd(std::vector<std::string> lines, int lineIndex) {
    while (lineIndex < lines.size() && !StringTools::isBlankLine(lines[lineIndex]) &&
           !isAgentHeader(lines[lineIndex])) {
        lineIndex++;
    }

    return lineIndex;
}
