//
//  HillClimbingAlgorithm.cpp
//  aoc2022
//
//  Created by Hee Suk Shin on 2023/01/25.
//

#include <algorithm>
#include <future>
#include <optional>
#include <queue>
#include <span>
#include <sstream>
#include <string>
#include <utility>
#include <vector>

#include "HillClimbingAlgorithm.hpp"
#include "utility.h"

namespace hca {

Point::Point(size_t index, char c): index(index), flag(Flag::None) {
    switch (c) {
        case 'a': case 'b': case 'c': case 'd': case 'e': case 'f':
        case 'g': case 'h': case 'i': case 'j': case 'k': case 'l':
        case 'm': case 'n': case 'o': case 'p': case 'q': case 'r':
        case 's': case 't': case 'u': case 'v': case 'w': case 'x':
        case 'y': case 'z':
            this->height = c;
            break;

        case 'S':
            this->height = 'a';
            this->flag = Flag::Start;
            break;

        case 'E':
            this->height = 'z';
            this->flag = Flag::End;
            break;

        default:
            throw CppErrorCodeParse;
    }
}

decltype(Point::index) Point::getIndex() const {
    return index;
}

bool Point::reachable(const Point &other) const {
    return other.height <= 1 + this->height;
}

std::span<std::optional<Ref<Point const>> const> Point::listNeighbours() const {
    return this->neighbours;
}

auto Point::listSources() const {
    auto neighbours = this->listNeighbours();
    std::vector<Ref<Point const>> sources;
    sources.reserve(neighbours.size());
    for (auto neighbour : neighbours) {
        if (neighbour && neighbour->get().reachable(*this)) {
            sources.push_back(*neighbour);
        }
    }
    return sources;
}

auto Map::getRows() const {
    return std::span(this->rowsOfPoints);
}

Point const& Map::getStartingPoint() const {
    for (auto& row : this->rowsOfPoints) {
        for (auto& point : row) {
            if (point.isStart()) {
                return point;
            }
        }
    }
    throw CppErrorCodeLogic;
}

Point const& Map::getEndingPoint() const {
    for (auto& row : this->rowsOfPoints) {
        for (auto& point : row) {
            if (point.isEnd()) {
                return point;
            }
        }
    }
    throw CppErrorCodeLogic;
}

size_t Map::size() const {
    return this->rowsOfPoints.size() * this->rowsOfPoints.front().size();
}

Map Map::parse(std::string&& input) {
    Map map;
    std::istringstream sin { input };
    std::string line;
    size_t index = 0;
    while (std::getline(sin, line)) {
        std::vector<Point> row;
        row.reserve(line.size());
        for (auto c : line) {
            row.emplace_back(index, c);
            ++index;
        }
        map.rowsOfPoints.emplace_back(std::move(row));
    }

    auto maxRows = std::size(map.rowsOfPoints);
    for (auto i = 0; i < maxRows; i++) {
        auto& row = map[i];
        for (auto j = 0; j < std::size(row); j++){
            auto& point = row[j];
            if (i > 0) {
                point.neighbours[Direction::NORTH] = map[i - 1][j];
            }
            if (i < maxRows - 1) {
                point.neighbours[Direction::SOUTH] = map[i + 1][j];
            }
            if (j > 0) {
                point.neighbours[Direction::WEST] = map[i][j - 1];
            }
            if (j < std::size(row) - 1) {
                point.neighbours[Direction::EAST] = map[i][j + 1];
            }
        }
    }
    return map;
}

bool Table::Entry::miss() const {
    return !this->cell.has_value();
}

Count& Table::Entry::operator*() {
    return this->cell.value();
}

Count const& Table::Entry::operator*() const {
    return this->cell.value();
}

Count* Table::Entry::operator->() {
    return &this->cell.value();
}

auto& Table::Entry::operator = (Count count) {
    this->cell = count;
    return *this;
}

auto Table::operator [] (Point const& point) -> Entry {
    return { BaseTable::operator[](point.getIndex()) };
}

auto Table::operator [] (Point const& point) const -> Entry const {
    return { BaseTable::operator[](point.getIndex()) };
}

void Driver::optimise() {
    using Work = std::tuple<Point const&, std::optional<Ref<Point const>>, Count>;
    std::queue<Work> queue;
    queue.emplace(this->map.getEndingPoint(), std::nullopt, 0);
    while (!queue.empty()) {
        auto [point, origin, count] = queue.front();
        queue.pop();
        if (this->table[point]) {
            continue;
        } else {
            this->table[point] = count;

            auto sources = point.listSources();
            if (origin) {
                std::erase(sources, *origin);
            }
            for (auto source : sources) {
                queue.emplace(source, point, 1 + count);
            }
        }
    }
}

auto Driver::driveFrom(Point const& point) const {
    return this->table[point];
}

std::string runPart1(std::string&& input) {
    auto const map = Map::parse(std::move(input));        // Read-only map from input.
    auto driver = Driver { map };
    driver.optimise();
    auto answer = driver.driveFrom(map.getStartingPoint());

    std::ostringstream sout;
    if (answer) {
        sout << *answer;
    } else {
        sout << "unreachable";
    }
    return std::move(sout).str();
}

std::string runPart2(std::string&& input) {
    auto const map = Map::parse(std::move(input));        // Read-only map from input.
    auto driver = Driver { map };
    driver.optimise();

    std::vector<std::future<std::optional<Count>>> futures;
    futures.reserve(map.getRows().size());
    for (auto const& row : map.getRows()) {
        futures.push_back(std::async([&driver, row] () -> std::optional<Count> {
            std::vector<std::optional<Count>> shortestStepsFromPoints;
            for (auto const& point : row) {
                if (point.height == 'a') {
                    auto const shortestSteps = driver.driveFrom(point);
                    shortestStepsFromPoints.push_back(shortestSteps);
                }
            }

            auto min = std::min_element(std::begin(shortestStepsFromPoints),
                                        std::end(shortestStepsFromPoints),
                                        OptionalCountLessThan {});
            if (min != std::end(shortestStepsFromPoints)) {
                return *min;
            } else {
                return std::nullopt;
            }
        }));
    }

    std::vector<std::optional<Count>> counts;
    counts.reserve(futures.size());
    std::transform(std::begin(futures),
                   std::end(futures),
                   std::back_inserter(counts),
                   [] (auto& future) {
        return future.get();
    });

    auto min = std::min_element(std::begin(counts),
                                std::end(counts),
                                OptionalCountLessThan {});
    if (min == std::end(counts)) {
        throw CppErrorCodeInput;
    } else {
        auto answer = *min;
        
        std::ostringstream sout;
        if (answer) {
            sout << *answer;
        } else {
            sout << "unreachable";
        }
        return std::move(sout).str();
    }
}

};
