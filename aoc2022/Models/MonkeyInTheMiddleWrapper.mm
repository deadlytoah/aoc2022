//
//  MonkeyInTheMiddleWrapper.mm
//  aoc2022
//
//  Created by Hee Suk Shin on 2023/01/16.
//

#import <Foundation/Foundation.h>

#import "MonkeyInTheMiddleWrapper.h"
#import "CppError.h"

#include <string>
#include <variant>

#include "MonkeyInTheMiddle.hpp"

@implementation MonkeyInTheMiddle

+ (NSString *)runPart1:(NSString *)input error:(NSError *__autoreleasing *)error {
    auto maybeString = MITMRunPart1(std::string([input UTF8String]));
    if (auto errorCode = std::get_if<CppErrorCode>(&maybeString)) {
        if (errorCode != nil) {
            *error = makeError(*errorCode);
        }
        return nil;
    } else {
        auto answer = std::get<std::string>(maybeString);
        return [NSString stringWithUTF8String:answer.c_str()];
    }
}

+ (NSString *)runPart2:(NSString *)input error:(NSError *__autoreleasing *)error {
    auto maybeString = MITMRunPart2(std::string([input UTF8String]));
    if (auto errorCode = std::get_if<CppErrorCode>(&maybeString)) {
        if (errorCode != nil) {
            *error = makeError(*errorCode);
        }
        return nil;
    } else {
        auto answer = std::get<std::string>(maybeString);
        return [NSString stringWithUTF8String:answer.c_str()];
    }
}

@end
