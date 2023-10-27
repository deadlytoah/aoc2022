//
//  CathodeRayTubeWrapper.mm
//  aoc2022
//
//  Created by Hee Suk Shin on 2023/01/10.
//

#import <Foundation/Foundation.h>

#import "CppError.h"
#import "CathodeRayTubeWrapper.h"

#include <string>
#include <variant>
#include "CathodeRayTube.hpp"

@implementation CRTProgram
@synthesize inner = _inner;
@synthesize hasInner = _hasInner;

+ (instancetype) programWithInner: (struct CCRTProgram)inner {
    CRTProgram *me = [[CRTProgram alloc] init];
    if (me != nil) {
        me->_inner = std::move(inner);
        me->_hasInner = true;
    }
    return me;
}

+ (instancetype) parse: (NSString *)input error: (NSError **)error {
    auto maybeProgram = CCRTParseProgram(std::string(input.UTF8String));
    if (auto inner = std::get_if<struct CCRTProgram>(&maybeProgram)) {
        return [CRTProgram programWithInner: std::move(*inner)];
    } else if (error != nil) {
        auto errorCode = std::get<enum CppErrorCode>(maybeProgram);
        *error = makeError(errorCode);
    }
    return nil;
}

- (struct CCRTProgram)moveInner {
    self->_hasInner = false;
    return std::move(self->_inner);
}

@end

@implementation CRTBitmap
@synthesize inner = _inner;

+ (instancetype)bitmapWithInner:(struct CCRTBitmap)inner {
    CRTBitmap *me = [super new];
    if (me != nil) {
        me->_inner = std::move(inner);
    }
    return me;
}

- (NSString *)description {
    return [NSString stringWithUTF8String: self->_inner.description().c_str()];
}
@end

@implementation CRTMachineState
@synthesize inner = _inner;

+ (instancetype)stateWithInner:(struct CCRTMachineState)inner {
    CRTMachineState *me = [[CRTMachineState alloc] init];
    if (me != nil) {
        me->_inner = std::move(inner);
    }
    return me;
}

- (CCRTRegisterValue)readFromRegister:(enum CCRTRegister)which {
    return self->_inner.readFromRegister(which);
}

- (CRTBitmap *)display {
    return [CRTBitmap bitmapWithInner: self->_inner.takeBitmap()];
}
@end

@implementation CRTMachine
@synthesize inner = _inner;

- (instancetype)init {
    self = [super init];
    if (self != NULL) {
        self->_inner = new CCRTMachine;
        if (self->_inner == nullptr) {
            return NULL;
        }
    }
    return self;
}

- (void)dealloc {
    if (self->_inner != nil) {
        delete self->_inner;
    }
}

- (BOOL) start: (CRTProgram *)program error: (NSError **)error {
    if ([program hasInner]) {
        self->_inner->start([program moveInner]);
        return TRUE;
    } else {
        if (error != nil) {
            *error = makeError(CppErrorCodeState);
        }
    }
    return FALSE;
}

- (CRTMachineState *)waitForClock:(CRTClockValue)clock error: (NSError**) error {
    auto maybeState = self->_inner->waitForClock(clock);
    if (auto errorCode = std::get_if<CppErrorCode>(&maybeState)) {
        if (error != nil) {
            *error = makeError(*errorCode);
        }
        return nil;
    } else {
        auto innerState = std::get<CCRTMachineState>(maybeState);
        return [CRTMachineState stateWithInner: innerState];
    }
}

@end
