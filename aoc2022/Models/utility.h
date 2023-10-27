//
//  utility.h
//  aoc2022
//
//  Created by Hee Suk Shin on 2023/02/10.
//

#ifndef utility_h
#define utility_h

template<typename T> using Ref = std::reference_wrapper<T>;

// helper type for the visitor
template<class... Ts> struct overloaded : Ts... { using Ts::operator()...; };
// explicit deduction guide (not needed as of C++20)
template<class... Ts> overloaded(Ts...) -> overloaded<Ts...>;

#endif /* utility_h */
