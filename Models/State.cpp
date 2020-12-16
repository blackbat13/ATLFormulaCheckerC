//
// Created by black on 02.10.2019.
//

#include "State.h"

void State::setProps(std::vector<std::string> props) {
    this->props = props;
}

bool State::operator<(const State &rhs) const {
    return props < rhs.props;
}

bool State::operator>(const State &rhs) const {
    return rhs < *this;
}

bool State::operator<=(const State &rhs) const {
    return !(rhs < *this);
}

bool State::operator>=(const State &rhs) const {
    return !(*this < rhs);
}

void State::print() {
    printf("Not implemented\n");
}

AsyncState::AsyncState(int id, std::string name) {
    this->id = id;
    this->name = name;
}
