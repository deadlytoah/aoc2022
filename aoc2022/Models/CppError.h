//
//  CppError.h
//  aoc2022
//
//  Created by Hee Suk Shin on 2023/01/08.
//

#ifndef CppError_h
#define CppError_h

#import <Foundation/Foundation.h>

#import "CppErrorCode.h"

inline NSError *makeError(enum CppErrorCode code) {
    NSString *message;
    switch (code) {
        case CppErrorCodeInput:
            message = @"Input error in C++ code";
            break;
        case CppErrorCodeLogic:
            message = @"Logic error in C++ code";
            break;
        case CppErrorCodeMemory:
            message = @"Out of memory in C++ code";
            break;
        case CppErrorCodeUnexpectedNil:
            message = @"Unexpected nil in C++ code";
            break;
        case CppErrorCodeParse:
            message = @"Parse error in C++ code";
            break;
        case CppErrorCodeState:
            message = @"Internal state error in C++ code";
            break;
        default:
            message = [NSString stringWithFormat:@"Unknown error code %d", code];
    }
    return [NSError errorWithDomain:ERROR_DOMAIN code:code userInfo:@{NSLocalizedDescriptionKey: message}];
}

#endif /* CppError_h */
