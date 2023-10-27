//
//  RopeBridgeWrapper.h
//  aoc2022
//
//  Created by Hee Suk Shin on 2023/01/05.
//

#ifndef RopeBridgeWrapper_h
#define RopeBridgeWrapper_h

#import <Foundation/Foundation.h>

#import "RBDirection.h"

@interface RopeBridgeInstruction : NSObject
@property(readonly) struct RBInstruction inner;
@property(readonly) uint32_t steps;
@property(readonly) enum RBDirection direction;

+ (instancetype)instructionWithInner: (struct RBInstruction) val;
+ (instancetype)instructionWithDirection: (enum RBDirection) direction;

+ (NSArray<RopeBridgeInstruction *> *) parse: (NSString *) input error: (NSError **) error;
@end

@interface RopeBridgeLocation : NSObject
@property(readonly) struct RBLocation inner;
@property(readonly) int32_t x;
@property(readonly) int32_t y;

+ (instancetype)locationWithInner: (struct RBLocation)inner;

- (BOOL)isTouching: (RopeBridgeLocation *)other;
@end

@interface RopeBridgeHead : NSObject
@property(readonly) RopeBridgeLocation *location;
@property(readonly) struct RBHead *inner;

- (instancetype) init;
- (void) dealloc;

- (void) follow: (RopeBridgeInstruction *) instruction;
@end

@interface RopeBridgeTail : NSObject
@property(readonly) struct RBTail *inner;
@property(readonly) RopeBridgeLocation *location;
@property(readonly) NSArray<RopeBridgeLocation *> *visitedLocations;

- (instancetype) initWithLocation: (RopeBridgeLocation *) location;
- (void) dealloc;

- (BOOL) isTouching: (RopeBridgeHead *) head;
- (void) follow: (RopeBridgeHead *) head;

@end

@interface RopeBridge : NSObject
@property(readonly) struct RBBridge *inner;
@property(readonly) NSArray<RopeBridgeLocation *> *visitedLocations;

- (instancetype) initWithLength: (size_t)length;
- (void) dealloc;

- (void) follow: (RopeBridgeInstruction *) instruction;
@end

#endif /* RopeBridgeWrapper_h */
