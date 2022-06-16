//
// Created by adda on 14.6.22.
//

#ifndef TOUCHCURSOR_LINUX_HYPERBINDING_H
#define TOUCHCURSOR_LINUX_HYPERBINDING_H

#include <unordered_map>
#include <string>

struct Binding {
    std::string originalKey;
    std::string mappedKey;
};

class HyperBinding {
    std::unordered_map<std::string,std::unordered_map<std::string, Binding>> hyperBindings;


};



#endif //TOUCHCURSOR_LINUX_HYPERBINDING_H
