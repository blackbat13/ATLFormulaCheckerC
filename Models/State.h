//
// Created by black on 02.10.2019.
//

#ifndef ATLFORMULACHECKER_STATE_H
#define ATLFORMULACHECKER_STATE_H


#include <vector>
#include <string>

class State {
protected:
    std::vector<std::string> props;
public:
    void setProps(std::vector<std::string> props);

    bool operator<(const State &rhs) const;

    bool operator>(const State &rhs) const;

    bool operator<=(const State &rhs) const;

    bool operator>=(const State &rhs) const;
};


#endif //ATLFORMULACHECKER_STATE_H
