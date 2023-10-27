//
//  RegolithReservoir.cpp
//  aoc2022
//
//  Created by Hee Suk Shin on 2023/02/16.
//

#include <algorithm>
#include <chrono>
#include <optional>
#include <sstream>
#include <string>
#include <string_view>
#include <thread>
#include <utility>
#include <variant>
#include <vector>

#include <zmq.hpp>
#include <zmq_addon.hpp>

#include "CppErrorCode.h"
#include "utility.h"
#include "RegolithReservoir.hpp"

namespace rr {

bool Bounds::includes(int x, int y) const {
    return x >= this->x && x < (this->x + this->width) &&
    y >= this->y && y < (this->y + this->height);
}

CaveIterator& CaveIterator::operator++() {
    if (++cell != this->column->second.end()) {
        return *this;
    } else {
        if (++this->column != buckets->get().end()) {
            cell = this->column->second.begin();
            assert(cell != this->column->second.end());
            return *this;
        } else {
            buckets = std::nullopt;
            return *this;
        }
    }
}

Cell const& CaveIterator::operator*() {
    return cell->second;
}

bool operator==(CaveIterator const& lhs, CaveIterator const& rhs) {
    return (!lhs.buckets.has_value() && !rhs.buckets.has_value()) ||
            (&lhs.buckets->get() == &rhs.buckets->get() &&
             lhs.column == rhs.column &&
             lhs.cell == rhs.cell);
}

Cave::Cave(): buckets({}), floor(Oblivion {}) {
    this->insertCell(Cell { CellType::Spawn, SPAWN_POINT});
}

auto Cave::begin() const {
    return CaveIterator {
        this->buckets,
        this->buckets.begin(),
        this->buckets.begin()->second.begin(),
    };
}

auto Cave::end() const {
    return CaveIterator { std::nullopt };
}

template <typename T>
void Cave::insertObject(T&& object) {
    for (auto&& obj : object.intoCells()) {
        this->insertCell(std::move(obj));
    }
}

Cave::IteratorToCell Cave::insertCell(Cell&& cell) {
    auto& bucket = buckets[cell.getCoordinate().x];
    auto [it, _] = bucket.insert_or_assign(cell.getCoordinate().y, std::move(cell));
    return it;
}

void Cave::removeCell(Coordinate coordinate) {
    int bucket_key = coordinate.x;
    auto& bucket = buckets[bucket_key];
    std::erase_if(bucket, [&coordinate] (auto const& item) { return item.first == coordinate.y; });
}

void Cave::removeCell(IteratorToCell iterator) {
    int bucket_key = iterator->second.getCoordinate().x;
    auto bucket = buckets.find(bucket_key);
    bucket->second.erase(iterator);
    if (bucket->second.empty()) {
        buckets.erase(bucket);
    }
}

Cave::IteratorToCell Cave::relocate(IteratorToCell iterator, Coordinate to) {
    auto& cell = iterator->second;
    if (cell.getCoordinate() != to) {
        auto targetCell = findCell(to);
        if (targetCell.has_value()) {
            throw CppErrorCodeState;
        } else {
            Cell inTransit = std::move(cell);
            this->removeCell(iterator);
            inTransit.setCoordinate(to);
            return this->insertCell(std::move(inTransit));
        }
    } else {
        // Nothing needs to be done.
        return iterator;
    }
}

std::optional<Cave::IteratorToCell> Cave::findCell(Coordinate coordinate) {
    int bucket_key = coordinate.x;
    auto it = buckets.find(bucket_key);
    if (it == buckets.end()) {
        return std::nullopt;
    }

    auto& cells = it->second;
    auto point = cells.lower_bound(coordinate.y);
    if (point != cells.end() && point->second.getCoordinate() == coordinate) {
        return point;
    } else {
        return std::nullopt;
    }
}

std::optional<Coordinate> Cave::findObjectBelow(Coordinate coordinate) const {
    int bucket_key = coordinate.x;
    auto it = buckets.find(bucket_key);
    if (it == buckets.end()) {
        return std::nullopt;
    } else {
        auto& cells = it->second;
        auto candidate = cells.upper_bound(coordinate.y);

        if (candidate != cells.end()) {
            return candidate->second.getCoordinate();
        } else {
            // If an object would fall to the endless depth.
            return std::nullopt;
        }
    }
}

auto Cave::getSpawnCell() -> CellRef {
    auto spawnCell = this->findCell(SPAWN_POINT);
    assert(spawnCell);
    return { *spawnCell };
}

/// Spawns a new sand cell at SPAWN\_POINT.
///
/// Returns the iterator to the spawn cell.
Cave::IteratorToCell Cave::spawnSand() {
    auto spawnCell = this->getSpawnCell();
    assert(spawnCell->getType() == CellType::Spawn);
    spawnCell->setType(CellType::SandBlockingSpawn);
    return spawnCell;
}

bool Cave::isWall(Coordinate coordinate) const {
    int bucket_key = coordinate.x;
    auto it = buckets.find(bucket_key);
    if (it == buckets.end()) {
        return false;
    } else {
        for (auto const& [y, obj] : it->second) {
            if (y == coordinate.y) {
                return obj.getType() == CellType::Wall;
            }
        }
        return false;
    }
}

bool Cave::isEmpty(Coordinate coordinate) const {
    int bucket_key = coordinate.x;
    auto it = buckets.find(bucket_key);
    if (it == buckets.end()) {
        return true;
    } else {
        return std::end(it->second) == std::find_if(std::begin(it->second), std::end(it->second), [&coordinate] (auto const& item) { return item.first == coordinate.y; });
    }
}

Bounds Cave::calculateBounds() const {
    int min_x = std::numeric_limits<int>::max();
    int max_x = std::numeric_limits<int>::min();
    int min_y = std::numeric_limits<int>::max();
    int max_y = std::numeric_limits<int>::min();

    for (auto const& [x, bucket] : buckets) {
        min_x = std::min(min_x, x);
        max_x = std::max(max_x, x);
        for (auto const& [y, cell] : bucket) {
            min_y = std::min(min_y, y);
            max_y = std::max(max_y, y);
        }
    }

    if (auto floor = this->floor.getHorizontalFloor()) {
        // Include the cave floor in the bounds.
        max_y = *floor;
    }

    return { min_x, min_y, max_x - min_x + 1, max_y - min_y + 1 };
}

void PrintingPress::load(Cave const& cave) {
    this->bounds = cave.calculateBounds();

    // Resize the grid to the size of the cave
    int width = this->bounds->width;
    int height = this->bounds->height;

    this->grid.resize(height, std::vector<CellState>(width, CellState::Empty));

    for (auto const& cell : cave) {
        int x = cell.getCoordinate().x - this->bounds->x;
        int y = cell.getCoordinate().y - this->bounds->y;

        if (x >= 0 && x < width && y >= 0 && y < height) {
            if (cell.getType() == CellType::Sand) {
                grid[y][x] = CellState::Sand;
            } else if (cell.getType() == CellType::SandBlockingSpawn) {
                grid[y][x] = CellState::Sand;
            } else if (cell.getType() == CellType::Wall) {
                grid[y][x] = CellState::Wall;
            } else if (cell.getType() == CellType::Spawn) {
                grid[y][x] = CellState::Spawn;
            }
        }
    }

    if (cave.hasHorizontalFloor()) {
        // Render the floor as a horizontal wall across the bottom of
        // the cave.
        for (int x = 0; x < width; x++) {
            grid[height - 1][x] = CellState::Wall;
        }
    }
}

std::string PrintingPress::printCave() const {
    std::stringstream ss;

    for (const auto& row : this->grid) {
        for (const auto& cellState : row) {
            char cellRepresentation;
            switch (cellState) {
                case CellState::Empty:
                    cellRepresentation = '.';
                    break;
                case CellState::Sand:
                    cellRepresentation = 'o';
                    break;
                case CellState::Wall:
                    cellRepresentation = '#';
                    break;
                case CellState::Spawn:
                    cellRepresentation = '+';
                    break;
            }
            ss << cellRepresentation;
        }
        ss << std::endl;
    }

    return std::move(ss).str();
}

std::optional<Coordinate> Physics::simulate(Cave::CellRef cellToSimulate) {
    struct LeaveSpawn { Coordinate target; };
    struct Fall { Coordinate target; };
    struct Slide { Coordinate target; };
    struct Rest {};
    struct Destroy {};

    struct Change {
        Cave::CellRef cell;
        std::variant<LeaveSpawn, Fall, Slide, Rest, Destroy> how;

        operator Event const() {
            std::ostringstream out;
            out << "CHANGE:[TYPE:";
            if (auto action = std::get_if<LeaveSpawn>(&how)) {
                out << "LeaveSpawn," << action->target;
            } else if (auto action = std::get_if<Fall>(&how)) {
                out << "Fall," << action->target;
            } else if (auto action = std::get_if<Slide>(&how)) {
                out << "Slide," << action->target;
            } else if (std::holds_alternative<Rest>(how)) {
                out << "Rest";
            } else if (std::holds_alternative<Destroy>(how)) {
                out << "Destroy";
            } else {
                assert(false);
                throw CppErrorCodeState;
            }
            out << "]";
            return out.str();
        }
    };

    bool running = true;
    auto quitSimulation = [&running] () { running = false; };

    std::optional<Cave::CellRef> active = cellToSimulate;

    History history;
    history.add("ACTIVE: " + *cellToSimulate);

    while (running) {
        std::optional<Change> change = std::nullopt;
        assert(active);
        Cell& sand = **active;
        auto coordinate = sand.getCoordinate();

        assert(sand.getType() == CellType::Sand || sand.getType() == CellType::SandBlockingSpawn);

        // ===============
        // PLAN THE CHANGE
        // ===============

        if (sand.getType() == CellType::SandBlockingSpawn) {
            if (auto floor = this->cave.getHorizontalFloor();
                floor && coordinate.y == *floor - 1) {
                // The sand hit the floor, coming to rest.  The floor is
                // right beneath the spawn point.  This would be a very rare
                // case.
                change = { *active, Rest {} };
            } else if (cave.isEmpty(coordinate.below())) {
                change = { *active, LeaveSpawn { coordinate.below() } };
            } else if (cave.isEmpty(coordinate.belowLeft())) {
                change = { *active, LeaveSpawn { coordinate.belowLeft() } };
            } else if (cave.isEmpty(coordinate.belowRight())) {
                change = { *active, LeaveSpawn { coordinate.belowRight() } };
            } else {
                change = { *active, Rest {} };
            }
        } else if (sand.getType() == CellType::Sand) {
            // Have the sand fall until it hits a wall, another
            // sand or the floor.
            if (auto floor = this->cave.getHorizontalFloor();
                floor && coordinate.y == *floor - 1) {
                // The sand hit the floor, coming to rest.  The floor is
                // right beneath the spawn point.  This would be a very rare
                // case.
                change = { *active, Rest {} };
            } else if (cave.isEmpty(coordinate.below())) {
                auto coordinateOfCollision = this->cave.findObjectBelow(coordinate);
                if (coordinateOfCollision) {
                    // Will something stop the sand from falling forever?
                    change = { *active, Fall { coordinateOfCollision->above() } };
                } else if (auto floor = this->cave.getHorizontalFloor()) {
                    // Sand hit the bottom of the cave.
                    change = {
                        *active,
                        Fall { Coordinate(sand.coordinate.x, *floor).above() },
                    };
                } else {
                    // Sand has fallen through the bottom of the cave.
                    change = { *active, Destroy {} };
                }
            } else if (cave.isEmpty(coordinate.belowLeft())) {
                change = { *active, Slide { coordinate.belowLeft() } };
            } else if (cave.isEmpty(coordinate.belowRight())) {
                change = { *active, Slide { coordinate.belowRight() } };
            } else {
                change = { *active, Rest {} };
            }
        } else {
            // Expect the active cell to be a sand cell, or a sand cell
            // right at the spawn point.
            throw CppErrorCodeState;
        }

#ifdef DEBUG
        if (change) {
            history.add(*change);
        } else {
            history.add("NO MORE ITERATIVE CHANGES");
        }
#endif

        // ==================
        // EXECUTE THE CHANGE
        // ==================
        //
        // The actions in this section of code implement the specification
        // in Specs/CaveCell.tla.

        if (change) {
            // We must be careful to update the reference to the active
            // cell when it may become invalid.

            assert(change->cell->getType() == CellType::Sand ||
                   change->cell->getType() == CellType::SandBlockingSpawn);

            std::visit(overloaded {
                [&] (LeaveSpawn& action) {
                    change->cell->setType(CellType::Spawn);
                    active = cave.insertCell({ CellType::Sand, action.target });
                },
                [&] (Fall& action) {
                    assert(this->cave.isEmpty(action.target));
                    Cell moving = std::move(*change->cell);
                    moving.setCoordinate(action.target);
                    this->cave.removeCell(change->cell);
                    active = this->cave.insertCell(std::move(moving));
                },
                [&] (Slide& action) {
                    assert(this->cave.isEmpty(action.target));
                    Cell moving = std::move(*change->cell);
                    moving.setCoordinate(action.target);
                    this->cave.removeCell(change->cell);
                    active = this->cave.insertCell(std::move(moving));
                },
                [&] (Rest&) {
                    quitSimulation();
                },
                [&] (Destroy&) {
                    if (change->cell->getType() == CellType::SandBlockingSpawn) {
                        change->cell->setType(CellType::Spawn);
                    } else {
                        this->cave.removeCell(change->cell);
                    }
                    active = std::nullopt;
                    quitSimulation();
                },
            }, change->how);
        } else {
            // There was no change in the simulation - exit simulation.
            quitSimulation();
        }

        this->assertValidity();
    }

    // Produce the coordinate of the resting sand cell.
    if (active) {
        auto& cell = **active;
        history.add("SIMULATION RESULT:[" + cell + ']');
        history.print();
        return cell.getCoordinate();
    } else {
        history.add("SIMULATION RESULT:[NO ACTIVE CELL]");
        history.print();
        return std::nullopt;
    }
}

void Physics::assertValidity() const {
#ifdef DEBUG
    // Validate there is only one spawn or sand blocking spawn cell.
    int count = 0;
    for (auto& cell : this->cave) {
        if (cell.getType() == CellType::Spawn || cell.getType() == CellType::SandBlockingSpawn) {
            ++count;
        } else {
            // Keep counting.
        }
    }
    assert(count == 1);

    // Validate the columns are in the correct groups.
    for (auto& bucket : this->cave.buckets) {
        for (auto& cell : bucket.second) {
            assert(cell.second.getCoordinate().x == bucket.first);
            assert(cell.first == cell.second.getCoordinate().y);
        }
    }
#endif
}

bool Segment::isHorizontal() {
    return coordinates.first.y == coordinates.second.y;
}

auto Segment::toCells() {
    std::vector<Cell> cells;
    if (isHorizontal()) {
        // Add the horizontal cells
        int minY = std::min(coordinates.first.y, coordinates.second.y);
        int maxX = std::max(coordinates.first.x, coordinates.second.x);
        int startX = std::min(coordinates.first.x, coordinates.second.x);
        for (int x = startX; x <= maxX; ++x) {
            cells.emplace_back(Cell { CellType::Wall, Coordinate { x, minY } });
        }
    } else {
        // Add the vertical cells
        int minX = std::min(coordinates.first.x, coordinates.second.x);
        int maxY = std::max(coordinates.first.y, coordinates.second.y);
        int startY = std::min(coordinates.first.y, coordinates.second.y);
        for (int y = startY; y <= maxY; ++y) {
            cells.emplace_back(Cell { CellType::Wall, Coordinate { minX, y } });
        }
    }
    return cells;
}

int Segment::getMaxY() const {
    return std::max(this->coordinates.first.y, this->coordinates.second.y);
}

std::vector<Segment> Segment::fromCoordinates(std::vector<Coordinate>&& coordinates) {
    if (coordinates.size() == 0) {
        return {};
    } else if (coordinates.size() == 1) {
        return std::vector { Segment { std::make_pair(coordinates[0], coordinates[0]) } };
    } else {
        auto segments { std::vector<Segment> {} };
        segments.reserve(coordinates.size() - 1);
        auto first = std::begin(coordinates);
        auto second = [&first] () { return std::next(first); };

        while (second() != std::end(coordinates)) {
            auto segmentCoords = std::make_pair(*first, *second());
            segments.emplace_back(Segment { std::move(segmentCoords) });
            ++first;
        }

        return segments;
    }
}

auto Wall::intoCells() {
    auto cells { std::vector<Cell> {} };
    for (auto& segment : this->segments) {
        for (auto& cell : segment.toCells()) {
            cells.emplace_back(cell);
        }
    }
    this->segments = {};
    return cells;
}

int Wall::getMaxY() const {
    auto const& segment = *std::max_element(std::begin(this->segments),
                                            std::end(this->segments),
                                            [] (auto const& a, auto const& b) {
        return a.getMaxY() < b.getMaxY();
    });
    return segment.getMaxY();
}

Wall Wall::parse(std::string&& line) {
    std::vector<Coordinate> coordinates;
    std::istringstream sin { std::move(line) };
    std::string maybeCoordinate;
    while (sin >> maybeCoordinate) {
        if (maybeCoordinate == "->") {
            continue;
        } else {
            std::istringstream coordinateIn(maybeCoordinate);
            std::string xAsString, yAsString;
            std::getline(coordinateIn, xAsString, ',');
            std::getline(coordinateIn, yAsString, ',');
            try {
                int x = std::stoi(xAsString);
                int y = std::stoi(yAsString);
                coordinates.emplace_back(x, y);
            } catch (std::invalid_argument) {
                throw CppErrorCodeParse;
            }
        }
    }
    return { Segment::fromCoordinates(std::move(coordinates)) };
}

auto Wall::parseFromLines(std::string&& input) {
    std::vector<Wall> walls;
    std::istringstream sin { std::move(input) };
    std::string line;
    while (std::getline(sin, line)) {
        walls.push_back(Wall::parse(std::move(line)));
    }
    return walls;
}

Delta::Delta(int checkpoint, Coordinate coordinate) {
    this->checkpoint = checkpoint;
    this->x = coordinate.x;
    this->y = coordinate.y;
}

void Visualization::addSand(int x, int y) {
    int i = x - this->bounds.x;
    int j = y - this->bounds.y;
    int index = j * (1 + this->bounds.width) + i;
    this->data[index] = 'o';
}

bool Visualization::resizeFor(int x, int y) {
    // Resize the visualization in the X axis.
    if (x < this->bounds.x) {
        // Insert an empty column.
        std::ostringstream visualization {};
    }
    return false;
}

Checkpoint::Checkpoint(Cave const& cave) {
    PrintingPress printingPress;
    printingPress.load(cave);
    auto bounds = printingPress.getBounds();
    auto visualization = printingPress.printCave();
    this->visualization = { bounds, std::move(visualization) };
}

void Checkpoint::apply(Delta const& delta) {
    if (this->visualization.includes(delta.x, delta.y)) {
        this->visualization.addSand(delta.x, delta.y);
    } else {
        // As the new sand cell is outside the bounds of the visualization,
        // we are unable to represent this Delta.
        return;
    }
}

std::string loadSnapshot(std::vector<Snapshot> const& snapshots, int index) {
    return std::visit(overloaded {
        [] (Checkpoint const& checkpoint) {
            return checkpoint.visualization.toString();
        },
        [snapshots, index] (Delta const& delta) {
            auto snapshot = std::get<Checkpoint>(snapshots[delta.checkpoint]);
            for (int i = delta.checkpoint + 1; i <= index; i++) {
                snapshot.apply(std::get<Delta>(snapshots[i]));
            }
            return snapshot.visualization.intoString();
        },
    }, snapshots[index]);
}

void handleZmqError(zmq::socket_t& socket, zmq::context_t& context)
{
    // Close the current socket
    socket.close();

    // Wait for a short period of time before trying to reopen the socket
    std::this_thread::sleep_for(std::chrono::milliseconds(100));

    // Reopen the socket
    zmq::socket_t new_socket { context, zmq::socket_type::rep };
    socket = std::move(new_socket);
}

void runServiceOfVisualisation(zmq::context_t&& context, std::string&& address, std::vector<Snapshot>&& snapshots)
{
    zmq::socket_t socket { context, zmq::socket_type::rep };
    zmq::recv_result_t result {};
    bool quit = false;

    try {
        socket.bind(address);
    } catch (zmq::error_t) {
        perror("ServiceOfVisualisation::run");
        throw CppErrorCodeMsgq;
    }

    while (!quit) {
        try {
            std::vector<zmq::message_t> request {};

            // Wait for request
            result = zmq::recv_multipart(socket, std::back_inserter(request));
            assert(result.has_value());

            std::vector<zmq::message_t> response {};
            auto command = request.front().to_string_view();
            if (command == "STOP") {
                quit = true;
                response.emplace_back(std::string_view{ "OK" });
            } else if (command == "GET") {
                try {
                    // Extract step number from request
                    int requestedStep = std::stoi(request[1].to_string());

                    // Send back visualisation data for requested step
                    if (requestedStep >= 0 && requestedStep < snapshots.size()) {
                        response.emplace_back(std::string_view{ "OK" });
                        response.emplace_back(loadSnapshot(snapshots, requestedStep));
                    } else {
                        // Requested step out of range, ignore and move on to next request
                        response.emplace_back(std::string_view{ "ERROR" });
                    }
                } catch (std::invalid_argument&) {
                    // Invalid request format, ignore and move on to next request
                    response.emplace_back(std::string_view{ "ERROR" });
                } catch (std::out_of_range&) {
                    // Invalid request format, ignore and move on to next request
                    response.emplace_back(std::string_view{ "ERROR" });
                }
            } else {
                // Unrecoginised command
                response.emplace_back(std::string_view{ "ERROR" });
            }
            result = zmq::send_multipart(socket, std::move(response));
            assert(result.has_value());
        } catch (zmq::error_t) {
            handleZmqError(socket, context);
        }
    }
}

void ServiceOfVisualisation::start(zmq::context_t&& context) {
    // Beware ServiceOfVisualisation object's lifetime isn't as long as
    // that of the thread.
    auto thread = std::thread(runServiceOfVisualisation, std::move(context), std::move(this->address), std::move(this->snapshots));
    thread.detach();
}

template<bool FLOOR>
std::string run(std::string&& input, bool enableVisualisation) {
    zmq::context_t context {};
    Cave cave {};
    Physics physics { cave };
    std::vector<Snapshot> snapshots {};
    int turn = 0;
    int snapshotId = 0;
    int lastCheckpoint = 0;
    int maxY = 0;

    {
        auto walls = Wall::parseFromLines(std::move(input));
        for (auto&& wall : walls) {
            maxY = std::max(maxY, wall.getMaxY());
            cave.insertObject(std::move(wall));
        }
    }

    if (FLOOR) {
        cave.setFloor({ 2 + maxY });
    }

    if (enableVisualisation) {
        snapshots.emplace_back(Checkpoint(cave));
        lastCheckpoint = snapshotId;
        ++snapshotId;
    }

    while (true) {
        std::optional<Coordinate> maybeRestingCoordinate;
        if (auto spawnCell = cave.getSpawnCell();
            spawnCell->getType() == CellType::SandBlockingSpawn) {
            // Is spawn blocked? -- this is an exit condition in Part 2.
            break;
        } else {
            maybeRestingCoordinate = physics.simulate(cave.spawnSand());
        }

        if (enableVisualisation) {
            if (snapshotId % 100 == 0) {
                lastCheckpoint = snapshotId;
                snapshots.emplace_back(Checkpoint(cave));
            } else if (auto restingCoordinate = maybeRestingCoordinate) {
                if (std::get<Checkpoint>(snapshots[lastCheckpoint]).includes(*restingCoordinate)) {
                    snapshots.emplace_back(Delta(lastCheckpoint, *restingCoordinate));
                } else {
                    // Visualization requires a resize.  Delta works
                    // only when such resize isn't necessary.  So we
                    // must use Checkpoint instead, which enables
                    // resizes.
                    lastCheckpoint = snapshotId;
                    snapshots.emplace_back(Checkpoint(cave));
                }
            } else {
                // No active cell means the sand cell didn't land anywhere
                // in Part 1.
            }
            ++snapshotId;
        } else {
            // Visualization not requested.
        }

        if (maybeRestingCoordinate) {
            ++turn;
        } else {
            // No changes from the simulation turn is the exit
            // condition for Part 1.
            break;
        }
    }

    if (enableVisualisation) {
        ServiceOfVisualisation service { "tcp://*:22143", std::move(snapshots) };
        service.start(std::move(context));
    }

    return std::to_string(turn);
}

std::string runPart1(std::string&& input, bool enableVisualisation) {
    return run<false>(std::move(input), enableVisualisation);
}

std::string runPart2(std::string&& input, bool enableVisualisation) {
    return run<true>(std::move(input), enableVisualisation);
}

}
