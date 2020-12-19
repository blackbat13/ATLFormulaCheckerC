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
        result.push_back(str.substr(p, i - p));
        i += delim.length();
        p = i;
    }

    result.push_back(str.substr(p));

    return result;
}

bool StringTools::isBlankLine(std::string line) {
    for(auto c : line) {
        if(c != ' ' && c != '\n') {
            return false;
        }
    }

    return true;
}

std::string StringTools::strip(std::string str, char c) {
    int b = 0, e = str.length() - 1;
    while(str[b] == c) {
        b++;
    }

    while(str[e] == c) {
        e--;
    }

    return str.substr(b, e - b + 1);
}

std::string StringTools::join(std::vector<std::string> list, int from, int to) {
    std::string result;
    for(int i = from; i < to; i++) {
        result += list[i];
    }

    return result;
}

std::vector<std::string> StringTools::splitLines(std::string str) {
    return StringTools::split(str, '\n');
}

std::string StringTools::replace(std::string str, std::string from, std::string to) {
    int index;
    while((index = str.find(from)) != -1) {
        auto str1 = str.substr(0, index);
        str1 += to;
        str1 += str.substr(index + from.length());
        str = str1;
    }

    return str;
}
