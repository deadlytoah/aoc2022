//
//  DistressSignal.cpp
//  aoc2022
//
//  Created by Hee Suk Shin on 2023/02/07.
//

#include <algorithm>
#include <optional>
#include <sstream>
#include <stack>
#include <string>
#include <utility>
#include <variant>
#include <vector>

#include "CppErrorCode.h"
#include "utility.h"
#include "DistressSignal.hpp"

namespace ds {

namespace token {

struct Space {};
struct OpenSquareBracket {};
struct CloseSquareBracket {};
struct Comma {};
using Number = int;
using Word = std::string;

}

using Token = std::variant<
token::Space,
token::OpenSquareBracket,
token::CloseSquareBracket,
token::Comma,
token::Number,
token::Word>;

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

static auto tokenise(std::string&& input) {
    TokeniserState state { TokeniserFlag::NEXT };
    std::vector<Token> sequenceOfTokens;
    std::istringstream sin { std::move(input) };
    char const EOF_CHAR = std::istringstream::traits_type::eof();

    while (sin) {
        char ch = sin.get();
        switch (state.flag) {
            case TokeniserFlag::NEXT:
                switch (ch) {
                    case ' ':
                        state = { TokeniserFlag::SPACE, std::string() + ch };
                        break;

                    case ',':
                        sequenceOfTokens.emplace_back(token::Comma {});
                        break;

                    case '[':
                        sequenceOfTokens.emplace_back(token::OpenSquareBracket {});
                        break;

                    case ']':
                        sequenceOfTokens.emplace_back(token::CloseSquareBracket {});
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
                        sequenceOfTokens.emplace_back(token::Space {});
                        break;

                    default:
                        sequenceOfTokens.emplace_back(token::Space {});
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
                        int number;
                        numin >> number;
                        sequenceOfTokens.emplace_back(token::Number{ number });
                        break;
                    }

                    default: {
                        std::istringstream numin { state.value.value() };
                        int number;
                        numin >> number;
                        sequenceOfTokens.emplace_back(token::Number { number });
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
                        sequenceOfTokens.emplace_back(token::Word { state.value.value() });
                        break;

                    case ' ':
                    case '\n':
                    case ':':
                    case ',':
                    case '[':
                    case ']':
                        sequenceOfTokens.emplace_back(token::Word { state.value.value() });
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

auto Packet::parse(std::string&& input) {
    std::stack<List> stack;
    auto tokens = tokenise(std::move(input));
    for (auto& token : tokens) {
        auto maybeList = std::visit(overloaded {
            [&stack] (token::OpenSquareBracket _) -> std::optional<List> {
                stack.emplace();
                return std::nullopt;
            },
            [&stack] (token::CloseSquareBracket _) -> std::optional<List> {
                if (stack.empty()) {
                    throw CppErrorCodeParse;
                } else {
                    auto list = std::move(stack.top());
                    stack.pop();
                    if (!stack.empty()) {
                        stack.top().emplace_back(std::move(list));
                        return std::nullopt;
                    } else {
                        return list;
                    }
                }
            },
            [] (token::Comma _) -> std::optional<List> { return std::nullopt; },
            [&stack] (token::Number number) -> std::optional<List> {
                stack.top().emplace_back(number);
                return std::nullopt;
            },
            [] (auto _) -> std::optional<List> {
                throw CppErrorCodeParse;
            },
        }, token);
        if (maybeList) {
            return Packet { std::move(*maybeList) };
        }
    }
    throw CppErrorCodeParse;
}

auto Packet::parseMany(std::string&& input) {
    std::string line;
    std::istringstream sin { std::move(input) };
    std::vector<std::optional<Packet>> packets {};
    while (std::getline(sin, line)) {
        if (line != "") {
            packets.emplace_back(Packet::parse(std::move(line)));
        } else {
            packets.push_back(std::nullopt);
        }
    }
    return packets;
}

auto Signal::parse(std::string input) {
    enum struct Expecting {
        Line1,
        Line2,
        Separator,
    } state = Expecting::Line1;
    std::string line, left;
    std::istringstream sin { std::move(input) };
    std::vector<Signal> signals;
    int index = 0;
    while (std::getline(sin, line)) {
        switch (state) {
            case Expecting::Line1:
                left = std::move(line);
                state = Expecting::Line2;
                break;

            case Expecting::Line2:
                ++index;
                signals.emplace_back(index,
                                     Packet::parse(std::move(left)),
                                     Packet::parse(std::move(line)));
                state = Expecting::Separator;
                break;

            case Expecting::Separator:
                state = Expecting::Line1;
                break;
        }
    }
    if (state == Expecting::Line2) {
        // Error - incomplete input.
        throw CppErrorCodeParse;
    } else {
        return signals;
    }
}

int compare(Element const& left, Element const& right) {
    enum struct Action {
        Next,
        Order,
        Disorder,
    };

    std::stack<std::pair<Element const, Element const>> stack;
    stack.emplace(left, right);
    while (!stack.empty()) {
        auto top = stack.top();
        Element const left = std::move(top.first);
        Element const right = std::move(top.second);
        stack.pop();

        Action action = std::visit(overloaded {
            [] (Value left, Value right) {
                if (left < right) {
                    return Action::Order;
                } else if (right < left) {
                    return Action::Disorder;
                } else {
                    return Action::Next;
                }
            },
            [&stack] (List left, List right) {
                for (auto& pair : Reversed { Zipped { left, right } }) {
                    stack.push(std::move(pair));
                }
                return Action::Next;
            },
            [&stack] (List left, Value right) {
                Element rightAsList {right};
                stack.emplace(std::move(left), std::move(rightAsList));
                return Action::Next;
            },
            [&stack] (Value left, List right) {
                Element leftAsList {left};
                stack.emplace(std::move(leftAsList), std::move(right));
                return Action::Next;
            },
            [] (NoValue left, List right) {
                return Action::Order;
            },
            [] (NoValue left, Value right) {
                return Action::Order;
            },
            [] (List left, NoValue right) {
                return Action::Disorder;
            },
            [] (Value left, NoValue right) {
                return Action::Disorder;
            },
            [] (auto left, auto right) -> Action {
                throw CppErrorCodeLogic;
            },
        }, std::move(left), std::move(right));

        switch (action) {
            case Action::Next:
                continue;
            case Action::Order:
                return -1;
            case Action::Disorder:
                return 1;
            default:
                throw CppErrorCodeLogic;
        }
    }
    // This would be the case where they are equal.
    return 0;
}

bool operator < (Element const& left, Element const& right) {
    return compare(left, right) < 0;
}

bool operator == (Packet const& left, Packet const& right) {
    return compare(left.root, right.root) == 0;
}

bool Signal:: checkOrder()const{
    if (this->left.root < this->right.root) {
        return true;
    } else {
        return false;
    }
}

Zipped::Zipped(List& left, List& right) {
   auto leftIterator = std::begin(left);
   auto rightIterator = std::begin(right);
   for (; leftIterator != std::end(left) && rightIterator != std::end(right);
        ++leftIterator, ++rightIterator) {
       zipped.emplace_back(std::move(*leftIterator), std::move(*rightIterator));
   }
    if (leftIterator != std::end(left)) {
        for (; leftIterator != std::end(left); ++leftIterator) {
            zipped.emplace_back(std::move(*leftIterator), NoValue {});
        }
    }
    if (rightIterator != std::end(right)) {
        for (; rightIterator != std::end(right); ++rightIterator) {
            zipped.emplace_back(NoValue {}, std::move(*rightIterator));
        }
    }
}

std::string runPart1(std::string&& input) {
    auto answer = 0;
    auto const signals = Signal::parse(std::move(input));
    for (auto const& signal : signals) {
        if (signal.checkOrder()) {
            answer += signal.index;
        }
    }

    std::ostringstream sout;
    sout << answer;
    return std::move(sout).str();
}

std::string runPart2(std::string&& input) {
    auto divider1 = Packet::parse("[[2]]");
    auto divider2 = Packet::parse("[[6]]");

    auto packetsWithSpacing = Packet::parseMany(std::move(input));
    std::vector<Packet> packets { divider1, divider2 };

    // Eliminate empty lines in the input.
    for (auto& packet : packetsWithSpacing) {
        if (packet) {
            packets.push_back(std::move(*packet));
        }
    }

    std::sort(std::begin(packets),
              std::end(packets),
              [] (auto& left, auto& right) {
        return left.root < right.root;
    });

    auto marker1 = std::find(std::begin(packets), std::end(packets), divider1);
    auto marker2 = std::find(std::begin(packets), std::end(packets), divider2);
    auto answer = (1 + std::distance(std::begin(packets), marker1)) * (1 + std::distance(std::begin(packets), marker2));

    std::ostringstream sout;
    sout << answer;
    return std::move(sout).str();
}

}
