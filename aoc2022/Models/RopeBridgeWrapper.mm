//
//  RopeBridgeWrapper.m
//  aoc2022
//
//  Created by Hee Suk Shin on 2023/01/05.
//

#import <Foundation/Foundation.h>

#import "RopeBridgeWrapper.h"

#import "CppError.h"
#import "RopeBridge.hpp"
#import "RBDirection.h"

#include <cstddef>
#include <string>
#include <variant>

@implementation RopeBridgeInstruction
@synthesize inner = _inner;

+ (instancetype)instructionWithInner: (struct RBInstruction) val {
    RopeBridgeInstruction *me = [[super alloc] init];
    if (me != NULL) {
        me->_inner = val;
    }
    return me;
}

+ (instancetype)instructionWithDirection: (enum RBDirection) direction {
    RopeBridgeInstruction *me = [[super alloc] init];
    if (me != NULL) {
        me->_inner = {direction, 1};
    }
    return me;
}

- (uint32_t)steps {
    return self->_inner.steps;
}

- (enum RBDirection)direction {
    return self->_inner.direction;
}

+ (NSArray<RopeBridgeInstruction *> *) parse: (NSString *) input error: (NSError **) error {
    auto maybeInstructions = RBParseInstructions(std::string([input UTF8String]));
    if (const auto instructions = std::get_if<std::vector<RBInstruction>>(&maybeInstructions)) {
        NSMutableArray<RopeBridgeInstruction *> *instArray = [NSMutableArray array];
        if (instArray == NULL) {
            if (error != NULL) {
                *error = makeError(CppErrorCodeMemory);
            }
            return NULL;
        } else {
            for (auto inst: *instructions) {
                RopeBridgeInstruction *wrapperOfInst = [RopeBridgeInstruction instructionWithInner: inst];
                if (wrapperOfInst == NULL) {
                    if (error != NULL) {
                        *error = makeError(CppErrorCodeMemory);
                    }
                    return NULL;
                } else {
                    [instArray addObject: wrapperOfInst];
                }
            }
            return instArray;
        }
    } else {
        if (error != NULL) {
            *error = makeError(std::get<CppErrorCode>(maybeInstructions));
        }
        return NULL;
    }
}
@end

@implementation RopeBridgeLocation
@synthesize inner = _inner;

+ (instancetype)locationWithInner: (struct RBLocation)inner {
    RopeBridgeLocation *me = [[RopeBridgeLocation alloc] init];
    if (me != NULL) {
        me->_inner = inner;
    }
    return me;
}

- (BOOL)isEqual:(id)other {
    RopeBridgeLocation *o = other;
    return self->_inner.isEqual(o->_inner);
}

- (NSUInteger)hash
{
    return self->_inner.hash();
}

- (int32_t) x {
    return self->_inner.x;
}

- (int32_t) y {
    return self->_inner.y;
}

- (BOOL)isTouching: (RopeBridgeLocation *)other {
    return self->_inner.isTouching(other->_inner);
}
@end

@implementation RopeBridgeHead
@synthesize inner = _inner;

- (instancetype) init {
    self = [super init];
    if (self) {
        self->_inner = new RBHead;
        if (self->_inner == nullptr) {
            return NULL;
        }
    }
    return self;
}

- (void) dealloc {
    if (self->_inner != nullptr) {
        delete self->_inner;
    }
}

- (RopeBridgeLocation *) location {
    return [RopeBridgeLocation locationWithInner: self->_inner->location];
}

- (void) follow: (RopeBridgeInstruction *) instruction {
    self->_inner->follow([instruction inner]);
}
@end

@implementation RopeBridgeTail
@synthesize inner = _inner;

- (instancetype) initWithLocation: (RopeBridgeLocation *) location {
    self = [super init];
    if (self) {
        self->_inner = new RBTail { [location inner] };
        if (self->_inner == nullptr) {
            return NULL;
        }

        self->_inner->addVisitedLocation(self->_inner->location);
    }
    return self;
}

- (void) dealloc {
    if (self->_inner != nullptr) {
        delete self->_inner;
    }
}

- (RopeBridgeLocation *) location {
    return [RopeBridgeLocation locationWithInner: self->_inner->location];
}

- (NSArray<RopeBridgeLocation *> *) visitedLocations {
    NSMutableArray<RopeBridgeLocation *> *locations = [NSMutableArray array];
    if (locations != NULL) {
        for (auto location : self->_inner->getVisitedLocations()) {
            [locations addObject: [RopeBridgeLocation locationWithInner: location]];
        }
    }
    return locations;
}

- (BOOL) isTouching: (RopeBridgeHead *) head {
    return [[head location] isTouching: [self location]];
}

- (void) follow: (RopeBridgeHead *) head {
    self->_inner->follow(*[head inner]);
    self->_inner->addVisitedLocation(self->_inner->location);
}
@end

@implementation RopeBridge
@synthesize inner = _inner;

- (instancetype) initWithLength:(size_t)length {
    self = [super init];
    if (self) {
        self->_inner = new RBBridge { length };
        if (self->_inner == nullptr) {
            return NULL;
        }
    }
    return self;
}

- (void) dealloc {
    if (self->_inner != nullptr) {
        delete self->_inner;
    }
}

- (void) follow: (RopeBridgeInstruction *) instruction {
    self->_inner->follow([instruction inner]);
}

- (NSArray<RopeBridgeLocation *> *) visitedLocations {
    NSMutableArray<RopeBridgeLocation *> *locations = [NSMutableArray array];
    if (locations != NULL) {
        for (auto location : self->_inner->getVisitedLocations()) {
            [locations addObject: [RopeBridgeLocation locationWithInner: location]];
        }
    }
    return locations;
}
@end
