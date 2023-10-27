//
//  RegolithReservoir.hpp
//  aoc2022
//
//  Created by Hee Suk Shin on 2023/02/16.
//

#ifndef RegolithReservoir_hpp
#define RegolithReservoir_hpp

#include <map>
#include <optional>
#include <string>
#include <string_view>
#include <unordered_map>
#include <utility>
#include <variant>
#include <vector>

#ifdef DEBUG
#include <iostream>
#endif

#include <zmq.hpp>

#include "utility.h"

namespace rr {

struct Coordinate {
    int x, y;

    Coordinate(int x, int y): x(x), y(y) {}

    Coordinate above() const {
        return {x, y - 1};
    }

    Coordinate below() const {
        return {x, y + 1};
    }

    Coordinate belowLeft() const {
        return {x - 1, y + 1};
    }

    Coordinate belowRight() const {
        return {x + 1, y + 1};
    }

    bool operator==(Coordinate const& other) const {
        return x == other.x && y == other.y;
    }
};

inline std::basic_ostream<char>& operator << (std::basic_ostream<char>& out, Coordinate coordinate) {
    out << "[COORD:" << coordinate.x << ',' << coordinate.y << ']';
    return out;
}

static inline const Coordinate SPAWN_POINT { 500, 0 };

struct Bounds {
    int x;
    int y;
    int width;
    int height;

    bool includes(int x, int y) const;
};

enum class CellType {
    Wall,
    Sand,
    Spawn,
    SandBlockingSpawn,
};

inline std::basic_ostream<char>& operator << (std::basic_ostream<char>& out, CellType cellType) {
    switch (cellType) {
        case CellType::Wall:
            out << "Wall";
            break;

        case CellType::Sand:
            out << "Sand";
            break;

        case CellType::Spawn:
            out << "Spawn";
            break;

        case CellType::SandBlockingSpawn:
            out << "SandBlockingSpawn";
            break;
    }
    return out;
}

struct Cell {
    CellType type;
    Coordinate coordinate;

    Cell(CellType type, Coordinate coordinate): type(type), coordinate(coordinate) {}

    Coordinate getCoordinate() const { return this->coordinate; }
    void setCoordinate(Coordinate coordinate) { this->coordinate = coordinate; }
    CellType getType() const { return this->type; }
    void setType(CellType type) { this->type = type; }

    bool operator==(Cell const& other) const {
        return this->coordinate == other.coordinate && this->type == other.type;
    }
};

inline std::string operator + (std::string lhs, Cell const& rhs) {
    std::ostringstream out;
    out << lhs;
    out << "CELL[TYPE:" << rhs.getType() << "," << rhs.getCoordinate() << ']';
    return out.str();
}

enum struct ActiveCellState {
    Falling,
    Sliding,
};

struct Oblivion {};

// Represents the horizontal floor at the specified Y coordinate that
// stretches forever along the X axis.
using HorizontalFloor = int;

struct Floor: public std::variant<Oblivion, HorizontalFloor> {
    using Base = std::variant<Oblivion, HorizontalFloor>;

    Floor(Base value): Base(value) {}

    bool isOblivion() const {
        return std::holds_alternative<Oblivion>(*this);
    }

    bool isHorizontalFloor() const {
        return std::holds_alternative<HorizontalFloor>(*this);
    }

    std::optional<HorizontalFloor> getHorizontalFloor() const {
        if (auto floor = std::get_if<HorizontalFloor>(this)) {
            return *floor;
        } else {
            return std::nullopt;
        }
    }
};

struct CaveIterator {
    CaveIterator& operator++();
    Cell const& operator*();

    using Buckets = std::unordered_map<int, std::map<int, Cell>>;
    std::optional<Ref<const Buckets>> buckets;
    Buckets::const_iterator column;
    Buckets::value_type::second_type::const_iterator cell;
};

bool operator==(CaveIterator const& lhs, CaveIterator const& rhs);

class Cave {
public:
    using Buckets = std::unordered_map<int, std::map<int, Cell>>;
    using IteratorToCell = Buckets::value_type::second_type::iterator;

    /// @brief Represents the reference to a cell in the cave.
    ///
    /// <code>CellRef</code> points to a valid cell in the cave.  The
    /// cave may sometimes undergo a structural change that may
    /// invalidate a <code>CellRef</code>.  In this case, accessing
    /// the <code>CellRef</code> to resolve a <code>Cell</code> will
    /// throw an exception.
    struct CellRef {
        IteratorToCell iteratorToCell;
        CellRef(IteratorToCell iteratorToCell): iteratorToCell(iteratorToCell) {}
        auto operator * () -> Cell& {
            return this->iteratorToCell->second;
        }
        auto operator -> () -> Cell* {
            return &this->iteratorToCell->second;
        }
        operator IteratorToCell () {
            return this->iteratorToCell;
        }
    };

    Cave();

    auto begin() const;
    auto end() const;

    template <typename T>
    void insertObject(T&& object);
    IteratorToCell insertCell(Cell&& cell);
    void removeCell(Coordinate);
    void removeCell(IteratorToCell);
    IteratorToCell relocate(IteratorToCell, Coordinate to);

    void setFloor(Floor floor) { this->floor = floor; }
    auto getHorizontalFloor() const { return this->floor.getHorizontalFloor(); }
    auto hasHorizontalFloor() const { return this->floor.isHorizontalFloor(); }

    std::optional<Coordinate> findObjectBelow(Coordinate) const;
    std::optional<IteratorToCell> findCell(Coordinate);

    /// @brief Returns the spawn cell of the cave.
    ///
    /// Returns the only spawn cell of the cave.  It is an error if
    /// there is no spawn cell found in the cave.  The behaviour of
    /// the function is undefined in this case.  In DEBUG build mode,
    /// this causes an assertion failure.
    ///
    /// @return The reference object to the spawn cell.
    CellRef getSpawnCell();

    IteratorToCell spawnSand();

    bool isWall(Coordinate) const;
    bool isEmpty(Coordinate) const;
    Bounds calculateBounds() const;

private:
    Buckets buckets;

    /// Describes the floor of the cave.
    Floor floor;

    friend class Physics;
};

enum class CellState {
    Empty,
    Sand,
    Wall,
    Spawn,
};

class PrintingPress {
public:
    PrintingPress(): bounds(std::nullopt), grid({}) {}

    Bounds getBounds() const { return this->bounds.value(); }
    void load(Cave const& cave);
    std::string printCave() const;

private:
    std::optional<Bounds> bounds;

    std::vector<std::vector<CellState>> grid;
};

struct Physics {
    Cave& cave;

    /// @brief Simulates the give cell in the cave.
    ///
    /// <code>simulate()</code> simulates the given cell in the cave
    /// according to the physics model in the specification.  The cell
    /// must exist in the cave already.  If it is not, the behaviour
    /// of the function is undefined.
    ///
    /// @param cellToSimulate The reference to the cell in the cave.
    ///
    /// @return The coordinate of the cell if it has come to rest.  If
    ///         it has left the cave (destroyed), returns
    ///         <code>std::nullopt</code>.
    std::optional<Coordinate> simulate(Cave::CellRef cellToSimulate);

private:
    void assertValidity() const;
};

using Event = std::string;

struct History {
    std::vector<Event> events;

    History& add(Event event) {
#ifdef DEBUG
        events.push_back(event);
#endif
        return *this;
    }

    void print() const {
#ifdef DEBUG
        std::ios_base::sync_with_stdio(false);
        for (auto const& event : this->events) {
            std::cerr << event << std::endl;
        }
        std::cerr << std::flush;
        std::ios_base::sync_with_stdio(true);
#endif
    }
};

struct Segment {
    std::pair<Coordinate, Coordinate> coordinates;

    Segment(auto coordinates): coordinates(coordinates) {}

    bool isHorizontal();
    bool isVertical() { return !this->isHorizontal(); }
    auto toCells();
    int getMaxY() const;
    static std::vector<Segment> fromCoordinates(std::vector<Coordinate>&&);
};

struct Wall {
    std::vector<Segment> segments;

    Wall(auto segments): segments(segments) {}

    auto intoCells();
    int getMaxY() const;

    static Wall parse(std::string&& line);
    static auto parseFromLines(std::string&& input);
};

/// Represents a snapshot that is a delta from the previous snapshot.
/// You can produce the snapshot by running the sequence of all deltas
/// from <code>checkpoint</code>.  (x, y) means a new sand object appeared
/// in this delta at the coordinate (x, y).
struct Delta {
    int checkpoint;
    int x;
    int y;

    Delta(int checkpoint, Coordinate);
};

/// @brief Represents the state of the cave in ASCII.
///
/// Represents the state of the cave in ASCII at a certain step of
/// simulation. Only includes the printable, whitespace and new line
/// characters.
class Visualization {
    /// The bounds of the cave.
    Bounds bounds;

    /// The visualization data.
    std::string data;

public:
    /// @brief Constructs a Visualization with default bounds and data.
    Visualization(): bounds {}, data {} {}

    /// @brief Constructs a Visualization with the given bounds and data.
    Visualization(Bounds bounds, std::string&& data): bounds(bounds), data(std::move(data)) {}

    /// @brief Adds a sand cell at the given location in the Visualization.
    void addSand(int x, int y);

    /// @brief Resizes the cave in the Visualization.
    ///
    /// Resizes the Visualization for a cave that includes the given
    /// coordinate.  Returns true if a resize happened, and false if
    /// no changes were necessary.
    ///
    /// <code>resizeFor(int, int)</code> resizes the visualization for
    /// X, but not Y.  So it ignores the second parameter.  The reason
    /// for this is the visible height of the cave doesn't ever
    /// change.
    ///
    /// @param x The X coordinate to resize for.
    /// @return Returns true if a resize happened.
    bool resizeFor(int x, int);

    /// @brief Checks if the Visualization bounds includes the given location.
    bool includes(int x, int y) const { return this->bounds.includes(x, y); }

    /// @brief Converts the Visualization into a string representation.
    ///
    /// <code>intoString()</code> converts the Visualization into a string.
    /// It destroys the Visualization in the process.
    std::string intoString() { return std::move(this->data); }

    /// @brief Converts the Visualization into a string representation.
    std::string toString() const { return this->data; }

    /// @brief Returns the bounds of the Visualization.
    Bounds getBounds() const { return this->bounds; }
};

/// Represents the checkpoint snapshot of the cave.  Several subsequent
/// snapshots will be a sequence of deltas from this checkpoint.
struct Checkpoint {
    /// The visualization data at this checkpoint.
    Visualization visualization;

    /// Takes a Checkpoint Snapshot.
    Checkpoint(Cave const&);

    /// Applies the given <code>delta</code> to the checkpoint snapshot.
    void apply(Delta const&);

    /// @brief Checks if the Checkpoint would show a Coordinate.
    bool includes(Coordinate c) const { return this->visualization.includes(c.x, c.y); }
};

using Snapshot = std::variant<Checkpoint, Delta>;

struct ServiceOfVisualisation {
    std::string address;
    std::vector<Snapshot> snapshots;
    void start(zmq::context_t&&);

private:
    void run(zmq::context_t&&);
};

std::string runPart1(std::string&& input, bool enableVisualisation);
std::string runPart2(std::string&& input, bool enableVisualisation);

}

#endif /* RegolithReservoir_hpp */
