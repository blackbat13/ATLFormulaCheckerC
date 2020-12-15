//
// Created by black on 14.12.2020.
//

#include "StringTools.h"

std::vector<std::string> StringTools::split(std::string str, char delim) {
    std::vector<std::string> cont;

    std::stringstream ss(str);
    std::string token;
    while (std::getline(ss, token, delim)) {
        cont.push_back(token);
    }

    return cont;
}
