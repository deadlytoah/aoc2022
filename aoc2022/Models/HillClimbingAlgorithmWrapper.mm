//
//  HillClimbingAlgorithmWrapper.mm
//  aoc2022
//
//  Created by Hee Suk Shin on 2023/01/25.
//

#import <Foundation/Foundation.h>

#import "HillClimbingAlgorithmWrapper.h"
#import "CppError.h"

#include <string>

#include "HillClimbingAlgorithm.hpp"

@implementation HillClimbingAlgorithmWrapper : NSObject 

+(NSString *) runPart1: (NSString*) input error: (NSError **)error {
    try {
        auto answer = hca::runPart1(std::string([input UTF8String]));
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
        auto answer = hca::runPart2(std::string([input UTF8String]));
        return [NSString stringWithUTF8String: answer.c_str()];
    } catch (CppErrorCode errorCode) {
        if (error != nil) {
            *error = makeError(errorCode);
        }
        return nil;
    }
}

@end
