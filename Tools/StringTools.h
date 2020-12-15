//
// Created by black on 14.12.2020.
//

#ifndef ATLFORMULACHECKER_STRINGTOOLS_H
#define ATLFORMULACHECKER_STRINGTOOLS_H

#include <vector>
#include <string>
#include <sstream>
#include <algorithm>
#include <iterator>

class StringTools {
public:
    static std::vector<std::string> split(std::string str, char delim);
    static std::vector<std::string> split(std::string str, std::string delim);
    static bool isBlankLine(std::string line);
    static std::string strip(std::string str, char c);
    static std::string join(std::vector<std::string> list, int from, int to);
    static std::vector<std::string> splitLines(std::string str);
    static std::string replace(std::string str, std::string from, std::string to);
};


#endif //ATLFORMULACHECKER_STRINGTOOLS_H
