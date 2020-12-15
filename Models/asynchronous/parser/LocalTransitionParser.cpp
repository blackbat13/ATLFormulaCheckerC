//
// Created by black on 14.12.2020.
//

#include "LocalTransitionParser.h"

LocalTransition LocalTransitionParser::parse(std::string transitionStr) {
    bool shared = false;
    std::string action, stateFrom, stateTo;
    std::vector<Condition> cond;
    std::map<std::string, std::string> props;

    if (transitionStr.substr(0, 6) == "shared") {
        shared = true;
        transitionStr = transitionStr.substr(7);
    }

    auto split1 = StringTools::split(transitionStr, ':');

    action = split1[0];
    transitionStr = split1[1];

    if (transitionStr.find("->") == -1) {
        split1 = StringTools::split(transitionStr, "-[");
        stateFrom = split1[0];
        transitionStr = split1[1];
        split1 = StringTools::split(transitionStr, "]>");
        auto conditions = split1[0];
        transitionStr = split1[1];

        if (conditions.find("==") != -1) {
            split1 = StringTools::split(conditions, "==");
            auto condVar = split1[0];
            auto condVal = split1[1];
            cond.push_back({condVar, std::stoi(condVal), "=="});
        } else if (conditions.find("!=") != -1) {
            split1 = StringTools::split(conditions, "!=");
            auto condVar = split1[0];
            auto condVal = split1[1];
            cond.push_back({condVar, std::stoi(condVal), "!="});
        } else {
            printf("ERROR!");
        }
    } else {
        split1 = StringTools::split(transitionStr, "->");
        stateFrom = split1[0];
        transitionStr = split1[1];
    }


    if (transitionStr.find('[') != -1) {
        split1 = StringTools::split(transitionStr, '[');
        stateTo = split1[0];
        transitionStr = split1[1];
        transitionStr = StringTools::split(transitionStr, ']')[0];
        auto variables = StringTools::split(transitionStr, ',');
        for (auto variable : variables) {
            variable = StringTools::strip(variable, ' ');
            if (variable.find('=') == -1) {
                if (variable[variable.size() - 1] == '?') {
                    props[StringTools::strip(variable, '?')] = "?";
                } else {
                    props[variable] = "!";
                }
                continue;
            }

            split1 = StringTools::split(variable, '=');
            auto prop = split1[0];
            auto val = split1[1];
            props[prop] = val;
        }
    } else {
        stateTo = transitionStr;
    }

    stateFrom = StringTools::strip(stateFrom, ' ');
    stateTo = StringTools::strip(stateTo, ' ');
    return LocalTransition(stateFrom, stateTo, action, shared, cond, props);
}
