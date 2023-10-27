//
//  HillClimbingAlgorithm.hpp
//  aoc2022
//
//  Created by Hee Suk Shin on 2023/01/25.
//

#ifndef HillClimbingAlgorithm_hpp
#define HillClimbingAlgorithm_hpp

#include <optional>
#include <span>
#include <string>
#include <vector>

#include "CppErrorCode.h"
#include "utility.h"

namespace hca {

using Count = uint32_t;
using Height = char;

struct OptionalCountLessThan {
   bool operator () (std::optional<Count>& lhs, std::optional<Count>& rhs) {
       if (!lhs.has_value()) {
           return false;
       } else if (!rhs.has_value()) {
           return true;
       } else {
           return lhs.value() < rhs.value();
       }
   }
};

enum class Direction {
    NORTH,
    SOUTH,
    WEST,
    EAST,
};

struct Point;
struct Neighbours {
    std::optional<Ref<Point const>> inner[4];

    Neighbours(): inner { std::nullopt, std::nullopt, std::nullopt, std::nullopt } {}

    operator std::span<std::optional<Ref<Point const>> const> () const {
        return { inner };
    }
    auto operator [] (Direction direction) -> std::optional<Ref<Point const>>& {
        return inner[static_cast<size_t>(direction)];
    }
};

struct Point {
    enum struct Flag: char {
        None,
        Start,
        End,
    };

    Height height;
    Flag flag;
    size_t index;
    Neighbours neighbours;

    Point(size_t index, char c);

    bool isStart() const { return flag == Flag::Start; }
    bool isEnd() const { return flag == Flag::End; }

    decltype(index) getIndex() const;
    std::span<std::optional<Ref<Point const>> const> listNeighbours() const;
    auto listSources() const;
    bool reachable(Point const& other) const;
};

inline bool operator < (Point const& lhs, Point const& rhs) {
    return lhs.getIndex() < rhs.getIndex();
}

inline bool operator == (Point const& lhs, Point const& rhs) {
    return lhs.getIndex() == rhs.getIndex();
}

class Map {
    std::vector<std::vector<Point>> rowsOfPoints;

public:
    static Map parse(std::string&& input);

    auto getRows() const;
    std::vector<Point>& operator [] (size_t index) {
        return rowsOfPoints[index];
    }

    Point const& getStartingPoint() const;
    Point const& getEndingPoint() const;
    size_t size() const;
};

using BaseTable = std::vector<std::optional<Count>>;
class Table: public BaseTable {
    struct Entry {
        BaseTable::reference cell;
        Entry(BaseTable::reference cell): cell(cell) {}
        Entry(BaseTable::const_reference cell): cell(const_cast<BaseTable::reference>(cell)) {}
        bool miss() const;
        operator bool() const { return !this->miss(); };
        operator BaseTable::value_type () { return this->cell; }
        operator BaseTable::value_type () const { return this->cell; }
        Count& operator *();
        Count const& operator *() const;
        Count* operator ->();
        auto& operator = (Count count);
    };

public:
    Table(size_t width): BaseTable(width) {}
    Entry operator [] (Point const& Point);
    Entry const operator [] (Point const& Point) const;
    void printMapOfStates() const;
};

struct Driver {
    Map const& map;
    Table table;

    Driver(Map const& map): map(map), table(map.size()) {}
    void optimise();
    auto driveFrom(Point const& point) const;
};

std::string runPart1(std::string&& input);
std::string runPart2(std::string&& input);

};

#endif /* HillClimbingAlgorithm_hpp */
