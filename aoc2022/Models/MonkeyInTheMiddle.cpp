//
//  MonkeyInTheMiddle.cpp
//  aoc2022
//
//  Created by Hee Suk Shin on 2023/01/16.
//

#include <algorithm>
#include <deque>
#include <map>
#include <sstream>
#include <string>
#include <variant>

#include "MonkeyInTheMiddle.hpp"
#include "CppErrorCode.h"

std::set<uint32_t> CompressedInteger::seen {};

CompressedInteger& CompressedInteger::operator *= (uint32_t other) {
    this->reference *= other;
    for (auto [key, value] : this->remainders) {
        value *= other;
    }
    return *this;
}

CompressedInteger CompressedInteger::operator * (uint32_t other) const {
    auto copy = CompressedInteger { *this };
    copy *= other;
    return copy;
}

CompressedInteger& CompressedInteger::operator *= (CompressedInteger const& other) {
    this->reference *= other.reference;
    for (auto [key, value] : this->remainders) {
        if (other.remainders.contains(key)) {
            value *= other.remainders.at(key);
        } else {
            value *= other.reference;
        }
    }
    return *this;
}

CompressedInteger CompressedInteger::operator * (CompressedInteger const& other) const {
    auto copy = CompressedInteger { *this };
    copy *= other;
    return copy;
}

CompressedInteger& CompressedInteger::operator += (uint32_t other) {
    this->reference += other;
    for (auto [key, value] : this->remainders) {
        value += other;
    }
    return *this;
}

CompressedInteger CompressedInteger::operator + (uint32_t other) const {
    auto copy = CompressedInteger { *this };
    copy += other;
    return copy;
}

CompressedInteger& CompressedInteger::operator += (CompressedInteger const& other) {
    this->reference += other.reference;
    for (auto [key, value] : this->remainders) {
        if (other.remainders.contains(key)) {
            value += other.remainders.at(key);
        } else {
            value += other.reference;
        }
    }
    return *this;
}

CompressedInteger CompressedInteger::operator + (CompressedInteger const& other) const {
    auto copy = CompressedInteger { *this };
    copy += other;
    return copy;
}

uint32_t CompressedInteger::operator % (uint32_t const modulus) {
    if (!CompressedInteger::seen.contains(modulus)) {
        seen.insert(modulus);
    }

    for (auto const & keySeen : seen) {
        if (!this->remainders.contains(keySeen)) {
            this->remainders.emplace(keySeen, this->reference % keySeen);
        }
    }

    if (this->remainders.contains(modulus)) {
        auto remainder = this->remainders.at(modulus);
        assert(remainder < modulus);
        return remainder;
    } else {
        assert(this->reference < 10000);
        auto remainder = this->reference % modulus;
        this->remainders.emplace(modulus, remainder);
        return remainder;
    }
}

enum class TokenKind {
    SPACE,
    WORD,
    COLON,
    COMMA,
    NUMBER,
    NEW_LINE,
};

struct Token {
    struct NoValue {};

    TokenKind kind;
    std::variant<NoValue, uint32_t, std::string> value;

    Token(TokenKind kind): kind(kind), value(NoValue{}) {}
    Token(TokenKind kind, uint32_t value): kind(kind), value(value) {}
    Token(TokenKind kind, std::string value): kind(kind), value(value) {}
};

enum class TokeniserFlag {
    NEXT,
    SPACE,
    WORD,
    NUMBER,
};

struct TokeniserState {
    TokeniserFlag flag;
    std::optional<std::string> value;
};

static std::vector<Token> tokenise(std::string input) {
    TokeniserState state { TokeniserFlag::NEXT };
    std::vector<Token> sequenceOfTokens;
    std::istringstream sin { input };
    char const EOF_CHAR = std::istringstream::traits_type::eof();

    while (sin) {
        char ch = sin.get();
        switch (state.flag) {
            case TokeniserFlag::NEXT:
                switch (ch) {
                    case ' ':
                        state = { TokeniserFlag::SPACE, std::string() + ch };
                        break;

                    case ':':
                        sequenceOfTokens.emplace_back(TokenKind::COLON);
                        break;

                    case '\n':
                        sequenceOfTokens.emplace_back(TokenKind::NEW_LINE);
                        break;

                    case ',':
                        sequenceOfTokens.emplace_back(TokenKind::COMMA);
                        break;

                    case '0': case '1': case '2': case '3': case '4':
                    case '5': case '6': case '7': case '8': case '9':
                        state = { TokeniserFlag::NUMBER, std::string() + ch };
                        break;

                    case EOF_CHAR:
                        break;

                    default:
                        state = { TokeniserFlag::WORD, std::string() + ch };
                        break;
                }
                break;

            case TokeniserFlag::SPACE:
                switch (ch) {
                    case ' ':
                        state.value = state.value.value() + ch;
                        break;

                    case EOF_CHAR:
                        sequenceOfTokens.emplace_back(TokenKind::SPACE, state.value.value().size());
                        break;

                    default:
                        sequenceOfTokens.emplace_back(TokenKind::SPACE, state.value.value().size());
                        if (!sin.unget()) {
                            throw CppErrorCodeParse;
                        } else {
                            state = { TokeniserFlag::NEXT };
                        }
                        break;
                }
                break;

            case TokeniserFlag::NUMBER:
                switch (ch) {
                    case '0': case '1': case '2': case '3': case '4':
                    case '5': case '6': case '7': case '8': case '9':
                        state.value = state.value.value() + ch;
                        break;

                    case EOF_CHAR: {
                        std::istringstream numin { state.value.value() };
                        uint32_t number;
                        numin >> number;
                        sequenceOfTokens.emplace_back(TokenKind::NUMBER, number);
                        break;
                    }

                    default: {
                        std::istringstream numin { state.value.value() };
                        uint32_t number;
                        numin >> number;
                        sequenceOfTokens.emplace_back(TokenKind::NUMBER, number);
                        if (!sin.unget()) {
                            throw CppErrorCodeParse;
                        } else {
                            state = { TokeniserFlag::NEXT };
                        }
                        break;
                    }
                }
                break;

            case TokeniserFlag::WORD:
                switch (ch) {
                    case EOF_CHAR:
                        sequenceOfTokens.emplace_back(TokenKind::WORD, state.value.value());
                        break;

                    case ' ':
                    case '\n':
                    case ':':
                    case ',':
                        sequenceOfTokens.emplace_back(TokenKind::WORD, state.value.value());
                        if (!sin.unget()) {
                            throw CppErrorCodeParse;
                        } else {
                            state = { TokeniserFlag::NEXT };
                        }
                        break;

                    default:
                        state.value = state.value.value() + ch;
                        break;
                }
                break;

            default:
                throw CppErrorCodeParse;
        }
    }
    return sequenceOfTokens;
}

static std::vector<MITMLex> lex(std::vector<Token> sequenceOfTokens) {
    enum class LexerState {
        NEXT,
        BEGINNING_OF_LINE,
        WORD_STARTING,
        WORD_THROW,
        WORD_THROW_TO,
        WORD_DIVISIBLE,
        WORD_IF,
    };

    auto state = LexerState::BEGINNING_OF_LINE;
    std::vector<MITMLex> sequenceOfLex;

    for (auto token : sequenceOfTokens) {
        switch (token.kind) {
            case TokenKind::SPACE:
                if (state == LexerState::BEGINNING_OF_LINE) {
                    auto countOfIndentation = std::get<uint32_t>(token.value) / 2;
                    for (auto i = 0; i < countOfIndentation; i++) {
                        sequenceOfLex.emplace_back(MITMLexicalType::INDENTATION);
                    }
                } else {
                    // Whitespaces usually serve functions without carrying
                    // any meaning.
                }
                break;

            case TokenKind::WORD: {
                auto word = std::get<std::string>(token.value);

                switch (state) {
                    case LexerState::WORD_STARTING:
                        if (word == "items") {
                            sequenceOfLex.emplace_back(MITMLexicalType::STARTING_ITEMS);
                            state = LexerState::NEXT;
                            break;
                        } else {
                            throw CppErrorCodeParse;
                        }

                    case LexerState::WORD_DIVISIBLE:
                        if (word == "by") {
                            sequenceOfLex.emplace_back(MITMLexicalType::DIVISIBLE_BY);
                            state = LexerState::NEXT;
                            break;
                        } else {
                            throw CppErrorCodeParse;
                        }

                    case LexerState::WORD_IF:
                        if (word == "true") {
                            sequenceOfLex.emplace_back(MITMLexicalType::IF_TRUE);
                            state = LexerState::NEXT;
                            break;
                        } if (word == "false") {
                            sequenceOfLex.emplace_back(MITMLexicalType::IF_FALSE);
                            state = LexerState::NEXT;
                            break;
                        } else {
                            throw CppErrorCodeParse;
                        }

                    case LexerState::WORD_THROW:
                        if (word == "to") {
                            state = LexerState::WORD_THROW_TO;
                            break;
                        } else {
                            throw CppErrorCodeParse;
                        }

                    case LexerState::WORD_THROW_TO:
                        if (word == "monkey") {
                            sequenceOfLex.emplace_back(MITMLexicalType::THROW_TO_MONKEY);
                            state = LexerState::NEXT;
                            break;
                        } else {
                            throw CppErrorCodeParse;
                        }

                    default:
                        if (word == "Monkey") {
                            sequenceOfLex.emplace_back(MITMLexicalType::MONKEY);
                            state = LexerState::NEXT;
                        } else if (word == "Starting") {
                            state = LexerState::WORD_STARTING;
                        } else if (word == "Operation") {
                            sequenceOfLex.emplace_back(MITMLexicalType::OPERATION);
                            state = LexerState::NEXT;
                        } else if (word == "new") {
                            sequenceOfLex.emplace_back(MITMLexicalType::NEW);
                            state = LexerState::NEXT;
                        } else if (word == "=") {
                            sequenceOfLex.emplace_back(MITMLexicalType::EQUAL);
                            state = LexerState::NEXT;
                        } else if (word == "old") {
                            sequenceOfLex.emplace_back(MITMLexicalType::OLD);
                            state = LexerState::NEXT;
                        } else if (word == "+") {
                            sequenceOfLex.emplace_back(MITMLexicalType::PLUS);
                            state = LexerState::NEXT;
                        } else if (word == "*") {
                            sequenceOfLex.emplace_back(MITMLexicalType::TIMES);
                            state = LexerState::NEXT;
                        } else if (word == "Test") {
                            sequenceOfLex.emplace_back(MITMLexicalType::TEST);
                            state = LexerState::NEXT;
                        } else if (word == "divisible") {
                            state = LexerState::WORD_DIVISIBLE;
                        } else if (word == "If") {
                            state = LexerState::WORD_IF;
                        } else if (word == "throw") {
                            state = LexerState::WORD_THROW;
                        }
                }
            }
                break;

            case TokenKind::COLON:
                sequenceOfLex.emplace_back(MITMLexicalType::COLON);
                state = LexerState::NEXT;
                break;

            case TokenKind::NUMBER: {
                auto number = std::get<uint32_t>(token.value);
                sequenceOfLex.emplace_back(MITMLexicalType::NUMBER, number);
                state = LexerState::NEXT;
            }
                break;

            case TokenKind::COMMA:
                // Comma doesn't have a meaning other than acting as a
                // separator.
                state = LexerState::NEXT;
                break;

            case TokenKind::NEW_LINE:
                sequenceOfLex.emplace_back(MITMLexicalType::NEW_LINE);
                state = LexerState::BEGINNING_OF_LINE;
                break;
        }
    }

    return sequenceOfLex;
}

MITMDocument MITMDocument::parse(std::vector<MITMLex> sequenceOfLex) {
    MITMDocument document;
    std::vector<MITMDocumentalNode *> stack { &document.root };
    size_t levelOfIndentation = 0;
    for (auto lex : sequenceOfLex) {
        switch (lex.type) {
            case MITMLexicalType::INDENTATION:
            case MITMLexicalType::COLON:
                ++levelOfIndentation;
                break;

            case MITMLexicalType::NEW_LINE:
                levelOfIndentation = 0;
                break;

            default:
                // stack resizes based on the current level of indentation
                // to help creating the document tree.
                if (stack.size() <= levelOfIndentation) {
                    throw CppErrorCodeParse;
                } else {
                    stack.resize(levelOfIndentation + 1);
                    auto newNode = stack[levelOfIndentation]->children.emplace_back(new MITMDocumentalNode(lex.type, lex.value)).get();
                    stack.push_back(newNode);
                }
                break;
        }
    }
    return document;
}

std::vector<std::shared_ptr<MITMDocumentalNode>>::const_iterator MITMDocument::beginOfChildren() const {
    return this->root.children.begin();
}

std::vector<std::shared_ptr<MITMDocumentalNode>>::const_iterator MITMDocument::endOfChildren() const {
    return this->root.children.end();
}

template<typename WORRIEDNESS>
static std::deque<MITMItem<WORRIEDNESS>> readStartingItems(std::shared_ptr<MITMDocumentalNode> node) {
    if (node->type != MITMLexicalType::STARTING_ITEMS) {
        throw CppErrorCodeParse;
    } else {
        std::deque<MITMItem<WORRIEDNESS>> startingItems;
        for (auto const &child : node->children) {
            if (child->value.has_value()) {
                startingItems.emplace_back(child->value.value());
            } else {
                throw CppErrorCodeParse;
            }
        }
        return startingItems;
    }
}

template<typename WORRIEDNESS>
static MITMOperation<WORRIEDNESS> readOperation(std::shared_ptr<MITMDocumentalNode> node) {
    if (node->type != MITMLexicalType::OPERATION || node->children.size() < 5) {
        throw CppErrorCodeParse;
    } else if (node->children[0]->type != MITMLexicalType::NEW ||
               node->children[1]->type != MITMLexicalType::EQUAL ||
               node->children[2]->type != MITMLexicalType::OLD) {
        throw CppErrorCodeParse;
    } else {
        MITMOperator oper;
        switch (node->children[3]->type) {
            case MITMLexicalType::TIMES:
                oper = MITMOperator::TIMES;
                break;

            case MITMLexicalType::PLUS:
                oper = MITMOperator::PLUS;
                break;

            default:
                throw CppErrorCodeParse;
        }

        auto &rhsNode = node->children[4];
        std::variant<typename MITMOperation<WORRIEDNESS>::Old, WORRIEDNESS> rhs;
        if (rhsNode->type == MITMLexicalType::NUMBER &&
            rhsNode->value.has_value()) {
            rhs = rhsNode->value.value();
        } else if (rhsNode->type == MITMLexicalType::OLD) {
            rhs = MITMOperation<WORRIEDNESS>::OLD;
        } else {
            throw CppErrorCodeParse;
        }

        return { oper, rhs };
    }
}

template<typename WORRIEDNESS>
static MITMTest<WORRIEDNESS> readTest(std::shared_ptr<MITMDocumentalNode> node) {
    if (node->type != MITMLexicalType::TEST || node->children.size() < 4) {
        throw CppErrorCodeParse;
    } else if (node->children[0]->type != MITMLexicalType::DIVISIBLE_BY) {
        throw CppErrorCodeParse;
    } else {
        try {
            uint32_t divisibleBy = node->children[1]->value.value();
            uint32_t ifTrueToMonkey, ifFalseToMonkey;
            if (node->children[2]->type == MITMLexicalType::IF_TRUE &&
                node->children[2]->children.size() >= 2 &&
                node->children[2]->children[0]->type == MITMLexicalType::THROW_TO_MONKEY) {
                ifTrueToMonkey = node->children[2]->children[1]->value.value();
            } else {
                throw CppErrorCodeParse;
            }
            if (node->children[3]->type == MITMLexicalType::IF_FALSE &&
                node->children[3]->children.size() >= 2 &&
                node->children[3]->children[0]->type == MITMLexicalType::THROW_TO_MONKEY) {
                ifFalseToMonkey = node->children[3]->children[1]->value.value();
            } else {
                throw CppErrorCodeParse;
            }
            return { divisibleBy, ifTrueToMonkey, ifFalseToMonkey };
        } catch (std::bad_optional_access) {
            throw CppErrorCodeParse;
        }
    }
}

template<typename WORRIEDNESS>
MITMMonkey<WORRIEDNESS> MITMMonkey<WORRIEDNESS>::buildFrom(std::shared_ptr<MITMDocumentalNode> node, MITMController<WORRIEDNESS> &controller) {
    if (node->value.has_value() && node->children.size() > 2) {
        auto index = node->value.value();
        auto startingItems = readStartingItems<WORRIEDNESS>(node->children[0]);
        auto operation = readOperation<WORRIEDNESS>(node->children[1]);
        auto test = readTest<WORRIEDNESS>(node->children[2]);
        return { controller, index, startingItems, operation, test };
    } else {
        throw CppErrorCodeParse;
    }
}

template<typename WORRIEDNESS>
MITMController<WORRIEDNESS> MITMController<WORRIEDNESS>::buildFrom(MITMDocument const document) {
    MITMController<WORRIEDNESS> controller;
    auto queue = std::deque(document.beginOfChildren(), document.endOfChildren());
    while (!queue.empty()) {
        auto monkey = queue.front();
        queue.pop_front();
        if (monkey->type != MITMLexicalType::MONKEY) {
            throw CppErrorCodeParse;
        } else if (queue.empty()) {
            throw CppErrorCodeParse;
        } else {
            controller.monkeysRef().push_back(MITMMonkey<WORRIEDNESS>::buildFrom(queue.front(), controller));
            queue.pop_front();
        }
    }
    return controller;
}

template<typename WORRIEDNESS>
typename MITMOperation<WORRIEDNESS>::WithOld MITMOperation<WORRIEDNESS>::withOld(WORRIEDNESS old) {
    auto &rhs = this->rhs;
    switch (this->oper) {
        case MITMOperator::TIMES:
            if (std::holds_alternative<MITMOperation::Old>(rhs)) {
                return {
                    [old] () {
                        return old * old;
                    }
                };
            } else {
                return {
                    [rhs, old] () {
                        return old * std::get<WORRIEDNESS>(rhs);
                    }
                };
            }

        case MITMOperator::PLUS:
            if (std::holds_alternative<MITMOperation::Old>(rhs)) {
                return {
                    [old] () {
                        return old + old;
                    }
                };
            } else {
                return {
                    [rhs, old] () {
                        return old + std::get<WORRIEDNESS>(rhs);
                    }
                };
            }

        default:
            throw CppErrorCodeState;
    }
}

template<typename WORRIEDNESS>
void MITMItem<WORRIEDNESS>::boring() {
    this->value /= 3;
}

template<typename WORRIEDNESS>
bool MITMTest<WORRIEDNESS>::operator()(MITMItem<WORRIEDNESS> &item) {
    return (item.value % this->divisibleBy) == 0;
}

template<typename WORRIEDNESS>
bool MITMMonkey<WORRIEDNESS>::takeNextItem(MITMItem<WORRIEDNESS> &item) {
    if (this->items.empty()) {
        return false;
    } else {
        item = this->items.front();
        this->items.pop_front();
        return true;
    }
}

template<typename WORRIEDNESS>
void MITMMonkey<WORRIEDNESS>::pass(MITMItem<WORRIEDNESS> item) {
    if (this->test(item)) {
        this->controller
            .monkeysRef()[this->test.ifTrueToMonkey]
            .snatch(std::move(item));
    } else {
        this->controller
            .monkeysRef()[this->test.ifFalseToMonkey]
            .snatch(std::move(item));
    }
}

template<typename WORRIEDNESS>
void MITMMonkey<WORRIEDNESS>::inspect(MITMItem<WORRIEDNESS> &item) {
    WORRIEDNESS newValue = this->operation
        .withOld(item.value)
        .computeNewValue();
    item.value = newValue;
    ++this->countOfInspection;
}

template<typename WORRIEDNESS>
uint64_t MITMController<WORRIEDNESS>::monkeyBusiness() const {
    std::vector<uint64_t> countsOfInspections;
    for (auto const &monkey : this->monkeys) {
        countsOfInspections.push_back(monkey.countOfInspection);
    }
    std::sort(countsOfInspections.begin(), countsOfInspections.end());
    uint64_t accumulation = countsOfInspections.back();
    countsOfInspections.pop_back();
    accumulation *= countsOfInspections.back();
    return accumulation;
}

template<typename WORRIEDNESS, bool MONKEY_GETS_BORED, int REPETITION>
static std::variant<std::string, CppErrorCode> MITMRun(std::string input) {
    try {
        auto controller = MITMController<WORRIEDNESS>::buildFrom(MITMDocument::parse(lex(tokenise(input))));
        for (auto i = 0; i < REPETITION; i++) {
            for (auto& monkey : controller.monkeysRef()) {
                MITMItem<WORRIEDNESS> item;
                while (monkey.takeNextItem(item)) {
                    monkey.inspect(item);
                    if (MONKEY_GETS_BORED) {
                        item.boring();
                    }
                    monkey.pass(item);
                }
            }
        }
        std::ostringstream sout;
        sout << controller.monkeyBusiness();
        return std::move(sout).str();
    } catch (CppErrorCode errorCode) {
        return errorCode;
    }
}

std::variant<std::string, CppErrorCode> MITMRunPart1(std::string input) {
    return MITMRun<uint32_t, true, 20>(input);
}

std::variant<std::string, CppErrorCode> MITMRunPart2(std::string input) {
    return MITMRun<CompressedInteger, false, 10000>(input);
}
