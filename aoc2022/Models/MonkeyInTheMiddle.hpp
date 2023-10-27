//
//  MonkeyInTheMiddle.hpp
//  aoc2022
//
//  Created by Hee Suk Shin on 2023/01/16.
//

#ifndef MonkeyInTheMiddle_hpp
#define MonkeyInTheMiddle_hpp

#include <deque>
#include <map>
#include <optional>
#include <set>
#include <string>
#include <utility>
#include <variant>
#include <vector>

#include "CppErrorCode.h"

struct CompressedInteger {
    static std::set<uint32_t> seen;

    // Makes sure you can only enter the remainders in the map.
    template<typename MODULUS_T, typename REMAINDER_T>
    struct Remainders: public std::map<MODULUS_T, REMAINDER_T> {
        struct Iterator {
            using key_type = typename std::map<MODULUS_T, REMAINDER_T>::key_type;
            typename std::map<MODULUS_T, REMAINDER_T>::iterator iter;

            std::pair<key_type, Iterator&> operator *() {
                return std::make_pair(this->iter->first, std::ref(*this));
            }

            Iterator& operator *= (uint32_t other) {
                iter->second *= other;
                iter->second %= iter->first;
                return *this;
            }

            Iterator& operator += (uint32_t other) {
                iter->second += other;
                iter->second %= iter->first;
                return *this;
            }

            bool operator != (Iterator other) const {
                return this->iter != other.iter;
            }

            void operator ++ () {
                ++this->iter;
            }
        };

        void insert(typename std::map<MODULUS_T, REMAINDER_T>::value_type pair) {
            if (pair.first < pair.second) {
                auto adjusted = std::make_pair(pair.first, pair.second % pair.first);
                std::map<MODULUS_T, REMAINDER_T>::insert(adjusted);
            } else {
                std::map<MODULUS_T, REMAINDER_T>::insert(pair);
            }
        }

        Iterator begin() {
            return { std::map<MODULUS_T, REMAINDER_T>::begin() };
        }

        Iterator end() {
            return { std::map<MODULUS_T, REMAINDER_T>::end() };
        }
    };

    // This reference integer here will end up overflowing.  But it
    // is useful until we have the map of remainders working.  We can
    // stop calculating its value after a while, but I haven't yet
    // figured out how to do that in a straightforward way.
    uint32_t reference;

    Remainders<uint32_t, uint32_t> remainders;
    CompressedInteger(uint32_t value): reference(value) {};

    CompressedInteger& operator *= (uint32_t other);
    CompressedInteger operator * (uint32_t other) const;
    CompressedInteger& operator *= (CompressedInteger const& other);
    CompressedInteger operator * (CompressedInteger const& other) const;
    CompressedInteger& operator += (uint32_t other);
    CompressedInteger operator + (uint32_t other) const;
    CompressedInteger& operator += (CompressedInteger const& other);
    CompressedInteger operator + (CompressedInteger const& other) const;
    uint32_t operator % (uint32_t modulus);
    CompressedInteger& operator /= (uint32_t other) {
        // Not supported
        throw CppErrorCodeLogic;
    }
};

enum class MITMLexicalType {
    ROOT,
    INDENTATION,
    NEW_LINE,
    NUMBER,
    COLON,
    EQUAL,
    TIMES,
    PLUS,
    MONKEY,
    STARTING_ITEMS,
    OPERATION,
    NEW,
    OLD,
    TEST,
    DIVISIBLE_BY,
    IF_TRUE,
    IF_FALSE,
    THROW_TO_MONKEY,
};

struct MITMLex {
    MITMLexicalType type;
    std::optional<uint32_t> value;

    MITMLex(MITMLexicalType type): type(type), value(std::nullopt) {}
    MITMLex(MITMLexicalType type, uint32_t value): type(type), value(value) {}
};

struct MITMDocumentalNode {
    MITMLexicalType type;
    std::optional<uint32_t> value;
    std::vector<std::shared_ptr<MITMDocumentalNode>> children;

    MITMDocumentalNode(MITMLexicalType type, std::optional<uint32_t> value): type(type), value(value) {}
};

class MITMDocument {
    MITMDocumentalNode root;

    MITMDocument(): root(MITMLexicalType::ROOT, std::nullopt) {}

public:
    static MITMDocument parse(std::vector<MITMLex> sequenceOfLex);

    std::vector<std::shared_ptr<MITMDocumentalNode>>::const_iterator beginOfChildren() const;
    std::vector<std::shared_ptr<MITMDocumentalNode>>::const_iterator endOfChildren() const;
};

enum class MITMOperator {
    TIMES,
    PLUS,
};

template<typename WORRIEDNESS>
struct MITMOperation {
    struct Old {};
    constexpr static Old OLD = {};
    MITMOperator oper;
    std::variant<Old, WORRIEDNESS> rhs;

    struct WithOld {
        std::function<WORRIEDNESS ()> computeNewValue;
    };

    MITMOperation(MITMOperator oper,
                  std::variant<Old, WORRIEDNESS> rhs)
    : oper(oper), rhs(rhs) {}

    WithOld withOld(WORRIEDNESS old);
};

template<typename WORRIEDNESS>
struct MITMItem {
    WORRIEDNESS value;
    MITMItem(): value(0) {}
    MITMItem(WORRIEDNESS value): value(value) {}
    void boring();
};

template<typename WORRIEDNESS>
struct MITMTest {
    uint32_t divisibleBy;
    uint32_t ifTrueToMonkey;
    uint32_t ifFalseToMonkey;

    bool operator ()(MITMItem<WORRIEDNESS> &item);
};

template<typename WORRIEDNESS>
class MITMController;
template<typename WORRIEDNESS>
class MITMMonkey {
    MITMController<WORRIEDNESS> &controller;
    uint32_t index;
    std::deque<MITMItem<WORRIEDNESS>> items;
    MITMOperation<WORRIEDNESS> operation;
    MITMTest<WORRIEDNESS> test;

    uint64_t countOfInspection;

    MITMMonkey(MITMController<WORRIEDNESS> &controller,
               uint32_t index,
               std::deque<MITMItem<WORRIEDNESS>> startingItems,
               MITMOperation<WORRIEDNESS> operation,
               MITMTest<WORRIEDNESS> test)
    : controller(controller), index(index), items(startingItems), operation(operation), test(test), countOfInspection(0) {}
    friend class MITMController<WORRIEDNESS>;

public:
    static MITMMonkey buildFrom(std::shared_ptr<MITMDocumentalNode> node,
                                MITMController<WORRIEDNESS> &controller);

    bool takeNextItem(MITMItem<WORRIEDNESS> &item);
    void inspect(MITMItem<WORRIEDNESS> &item);
    void pass(MITMItem<WORRIEDNESS> item);
    void snatch(MITMItem<WORRIEDNESS> item) { this->items.push_back(item); }
};

template<typename WORRIEDNESS>
class MITMController {
    std::vector<MITMMonkey<WORRIEDNESS>> monkeys;

public:
    static MITMController buildFrom(MITMDocument document);

    std::vector<MITMMonkey<WORRIEDNESS>> &monkeysRef() { return monkeys; }
    uint64_t monkeyBusiness() const;
};

std::variant<std::string, CppErrorCode> MITMRunPart1(std::string input);
std::variant<std::string, CppErrorCode> MITMRunPart2(std::string input);

#endif /* MonkeyInTheMiddle_hpp */
