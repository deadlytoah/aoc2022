//
//  CppErrorCode.h
//  aoc2022
//
//  Created by Hee Suk Shin on 2023/01/07.
//

#ifndef CppErrorCode_h
#define CppErrorCode_h

#define ERROR_DOMAIN @"aoc2022"

enum CppErrorCode {
    CppErrorCodeInput = 0,

    /// There was a programming error.
    CppErrorCodeLogic,

    CppErrorCodeMemory,
    CppErrorCodeUnexpectedNil,

    /// Parsing failed.
    CppErrorCodeParse,

    /// Internal state error.
    CppErrorCodeState,

    /// Error in underlying message queue.
    CppErrorCodeMsgq,
};

#endif /* CppErrorCode_h */
