//
//  DistressSignalWrapper.mm
//  aoc2022
//
//  Created by Hee Suk Shin on 2023/02/07.
//

#import <Foundation/Foundation.h>

#import "DistressSignalWrapper.h"
#import "CppError.h"

#include <string>

#import "DistressSignal.hpp"

@implementation DistressSignalWrapper : NSObject

+(NSString *) runPart1: (NSString*) input error: (NSError **)error {
    try {
        auto answer = ds::runPart1(std::string([input UTF8String]));
        return [NSString stringWithUTF8String: answer.c_str()];
    } catch (CppErrorCode errorCode) {
        if (error != nil) {
            *error = makeError(errorCode);
        }
        return nil;
    }
}

+(NSString *) runPart2: (NSString*) input error: (NSError **)error {
    try {
        auto answer = ds::runPart2(std::string([input UTF8String]));
        return [NSString stringWithUTF8String: answer.c_str()];
    } catch (CppErrorCode errorCode) {
        if (error != nil) {
            *error = makeError(errorCode);
        }
        return nil;
    }
}

@end
