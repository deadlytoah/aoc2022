//
//  RopeBridge.cpp
//  aoc2022
//
//  Created by Hee Suk Shin on 2023/01/05.
//

#import "CppErrorCode.h"
#import "RopeBridge.hpp"

#include <optional>
#include <sstream>
#include <tuple>
#include <unordered_set>
#include <variant>
#include <vector>

std::vector<std::tuple<std::optional<std::string>, std::optional<std::string>>> RBTokenise(std::string input) {
    std::vector<std::tuple<std::optional<std::string>, std::optional<std::string>>> tokens;
    std::istringstream sin { input };
    std::string line;
    while (std::getline(sin, line)) {
        std::istringstream sin { line };
        std::string token;
        sin >> token;
        if (!sin || token == "") {
            tokens.push_back(std::make_tuple(std::nullopt, std::nullopt));
        } else {
            std::string token2;
            sin >> token2;
            if (!sin || token2 == "") {
                tokens.push_back(std::make_tuple(token, std::nullopt));
            } else {
                tokens.push_back(std::make_tuple(token, token2));
            }
        }
    }
    return tokens;
}

std::variant<RBDirection, CppErrorCode> RBParseDirection(std::string direction) {
    if (direction == "U") {
        return RBUp;
    } else if (direction == "D") {
        return RBDown;
    } else if (direction == "L") {
        return RBLeft;
    } else if (direction == "R") {
        return RBRight;
    } else {
        return CppErrorCodeParse;
    }
}

RBSteps RBParseSteps(std::string steps) {
    std::istringstream sin { steps };
    RBSteps num;
    sin >> num;
    return num;
}

std::variant<std::vector<RBInstruction>, CppErrorCode> RBParseInstructions(std::string input) {
    std::vector<RBInstruction> instructions;

    for (auto [maybeDirection, maybeSteps] : RBTokenise(input)) {
        if (!maybeDirection.has_value() || !maybeSteps.has_value()) {
            return CppErrorCodeParse;
        } else {
            auto direction = RBParseDirection(*maybeDirection);
            if (const auto errorPtr = std::get_if<CppErrorCode>(&direction)) {
                return *errorPtr;
            } else {
                auto steps = RBParseSteps(*maybeSteps);
                instructions.push_back({std::get<RBDirection>(direction), steps});
            }
        }
    }

    return instructions;
}

void RBBody::follow(const RBBody &leader) {
    auto headX = leader.location.x;
    auto tailX = this->location.x;
    auto headY = leader.location.y;
    auto tailY = this->location.y;
    RBOffset offsetX, offsetY;
    if (headX < tailX) {
        offsetX = -1;
    } else if (headX == tailX) {
        offsetX = 0;
    } else {
        offsetX = 1;
    }
    if (headY < tailY) {
        offsetY = -1;
    } else if (headY == tailY) {
        offsetY = 0;
    } else {
        offsetY = 1;
    }
    this->location = { tailX + offsetX, tailY + offsetY };
}

bool RBBody::isTouching(RBBody const &other) {
    return this->location.isTouching(other.location);
}

std::unordered_set<RBLocation, RBLocationHasher, RBLocationEqualTo> const &RBTail::getVisitedLocations() const {
    return this->visitedLocations;
}

void RBTail::addVisitedLocation(RBLocation location) {
    this->visitedLocations.insert(location);
}

void RBTail::rememberVisitedLocation() {
    this->visitedLocations.insert(this->location);
}

RBBridge::RBBridge(size_t lengthOfBridge) {
    if (lengthOfBridge > 2) {
        this->sequenceOfBodies = std::vector(lengthOfBridge - 2, RBBody {});
    }
    this->rememberVisitedLocation();
}

std::unordered_set<RBLocation, RBLocationHasher, RBLocationEqualTo> const &RBBridge::getVisitedLocations() const {
    return this->tail.getVisitedLocations();
}

void RBBridge::follow(RBInstruction const &instruction) {
    this->head.follow(instruction);
    RBBody* before = &this->head;
    for (auto& body : this->sequenceOfBodies) {
        if (!body.isTouching(*before)) {
            body.follow(*before);
        }
        before = &body;
    }
    if (!tail.isTouching(*before)) {
        this->tail.follow(*before);
        this->tail.rememberVisitedLocation();
    }
}
