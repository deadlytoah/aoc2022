//
//  HillClimbingAlgorithmDebug.cpp
//  aoc2022
//
//  Created by Hee Suk Shin on 2023/01/28.
//

#include <iostream>

#include "HillClimbingAlgorithm.hpp"

namespace hca {

void Table::printMapOfStates() const {
    auto i = 0;
    for (auto const& entry : *this) {
        std::cerr << i++;
        try {
            auto count = *entry;
            std::cerr << ": " << count;
        } catch (...) {
            std::cerr << ": .";
        }
        std::cerr << std::endl;
    }
}

}
