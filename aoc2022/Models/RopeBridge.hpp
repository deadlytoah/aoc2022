//
//  RopeBridge.hpp
//  aoc2022
//
//  Created by Hee Suk Shin on 2023/01/05.
//

#ifndef RopeBridge_hpp
#define RopeBridge_hpp

#include <stdio.h>
#include <string>
#include <unordered_set>
#include <variant>
#include <vector>

#import "CppErrorCode.h"
#import "RBDirection.h"

typedef unsigned int RBSteps;

struct RBInstruction {
    RBDirection direction;
    RBSteps steps;
};

std::variant<std::vector<RBInstruction>, CppErrorCode> RBParseInstructions(std::string input);

typedef int32_t RBOffset;

struct RBLocation {
    RBOffset x, y;

    bool isEqual(RBLocation const &rhs) {
        return this->x == rhs.x && this->y == rhs.y;
    }

    uint32_t hash() {
        return x * 7 + y * 11;
    }

    RBLocation translate(RBInstruction const &instruction) {
        switch (instruction.direction) {
            case RBUp:
                return RBLocation { this->x, this->y - (RBOffset)instruction.steps };
            case RBDown:
                return RBLocation { this->x, this->y + (RBOffset)instruction.steps };
            case RBLeft:
                return RBLocation { this->x - (RBOffset)instruction.steps, this->y };
            case RBRight:
                return RBLocation { this->x + (RBOffset)instruction.steps, this->y };
            default:
                abort();
        }
    }

    bool isTouching(RBLocation const &other) {
        auto xDiff = this->x - other.x;
        auto yDiff = this->y - other.y;
        return xDiff >= -1 && xDiff <= 1 && yDiff >= -1 && yDiff <= 1;
    }
};

struct RBLocationHasher {
    std::size_t operator () (RBLocation key) const {
        return std::hash<RBOffset>()(key.x) + std::hash<RBOffset>()(key.y);
    }
};

struct RBLocationEqualTo {
    bool operator () (RBLocation const & lhs, RBLocation const & rhs) const {
        return lhs.x == rhs.x && lhs.y == rhs.y;
    }
};

struct RBBody {
    RBLocation location;

    void follow(RBBody const &leader);
    bool isTouching(RBBody const &other);
};

struct RBHead: public RBBody {
    void follow(RBInstruction const &instruction) {
        auto newLocation = this->location.translate(instruction);
        this->location = newLocation;
    }
};

struct RBTail: public RBBody {
    std::unordered_set<RBLocation, RBLocationHasher, RBLocationEqualTo> visitedLocations;

    std::unordered_set<RBLocation, RBLocationHasher, RBLocationEqualTo> const &getVisitedLocations() const;
    void addVisitedLocation(RBLocation location);
    void rememberVisitedLocation();
};

struct RBBridge {
    RBHead head;
    std::vector<RBBody> sequenceOfBodies;
    RBTail tail;

    RBBridge(): RBBridge(2) {}
    RBBridge(size_t lengthOfBridge);

    void follow(RBInstruction const &instruction);

    void rememberVisitedLocation() {
        this->tail.rememberVisitedLocation();
    }

    std::unordered_set<RBLocation, RBLocationHasher, RBLocationEqualTo> const &getVisitedLocations() const;
};

#endif /* RopeBridge_hpp */
