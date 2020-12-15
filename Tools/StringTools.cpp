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

std::vector<std::string> StringTools::split(std::string str, std::string delim) {
    std::vector<std::string> result;

    int i = 0, p = 0;
    while(i < str.length() && (i = str.find(delim, i)) != -1) {
        result.push_back(str.substr(p, i - p - 1));
        i += delim.length();
    }

    return result;
}
