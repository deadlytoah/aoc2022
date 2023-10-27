//
//  DistressSignal.hpp
//  aoc2022
//
//  Created by Hee Suk Shin on 2023/02/07.
//

#ifndef DistressSignal_hpp
#define DistressSignal_hpp

#include <memory>
#include <optional>
#include <string>
#include <utility>
#include <variant>
#include <vector>

namespace ds {

struct Element;
struct NoValue {};
using Value = int;
using List = std::vector<Element>;
struct Element: public std::variant<NoValue, Value, List> {
    using Base = std::variant<NoValue, Value, List>;
    Element(List const& list): Base(list) {}
    Element(std::initializer_list<Element> list): Base(list) {}
    Element(List&& list): Base(list) {}
    Element(Value value): Base(value) {}
    Element(NoValue noValue): Base(noValue) {}
};

bool operator < (Element const& left, Element const& right);

struct Packet {
    Element root;
    static auto parse(std::string&& str);
    static auto parseMany(std::string&&);
};

bool operator == (Packet const& left, Packet const& right);

struct Signal {
    int index;
    Packet left;
    Packet right;

    Signal(int index, Packet&& left, Packet&& right): index(index), left(std::move(left)), right(std::move(right)) {}

    static auto parse(std::string input);

    bool checkOrder() const;
};

struct Zipped {
    std::vector<std::pair<Element, Element>> zipped;
    Zipped(List& left, List& right);

    auto begin() {
        return std::begin(zipped);
    }

    auto end() {
        return std::end(zipped);
    }
};

template<typename T>
struct Reversed {
    T inner;
    Reversed(T&& inner): inner(std::move(inner)) {}

    auto begin() {
        return std::reverse_iterator(std::end(inner));
    }

    auto end() {
        return std::reverse_iterator(std::begin(inner));
    }
};

std::string runPart1(std::string&& input);
std::string runPart2(std::string&& input);

}

#endif /* DistressSignal_hpp */
