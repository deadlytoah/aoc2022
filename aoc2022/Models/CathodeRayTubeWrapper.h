//
//  CathodeRayTubeWrapper.h
//  aoc2022
//
//  Created by Hee Suk Shin on 2023/01/10.
//

#ifndef CathodeRayTubeWrapper_h
#define CathodeRayTubeWrapper_h

#import <Foundation/Foundation.h>

#import "CCRTRegister.h"

typedef int32_t CRTClockValue;

@interface CRTProgram : NSObject
@property(readonly) struct CCRTProgram inner;
@property(readonly) BOOL hasInner;

+ (instancetype)programWithInner: (struct CCRTProgram)inner;
+ (instancetype)parse: (NSString *)input error: (NSError **)error;

- (struct CCRTProgram)moveInner;
@end

@interface CRTBitmap : NSObject
@property(readonly) struct CCRTBitmap inner;
@property(readonly) NSString *description;
+ (instancetype)bitmapWithInner: (struct CCRTBitmap)inner;
@end

@interface CRTMachineState : NSObject
@property(readonly) struct CCRTMachineState inner;
+ (instancetype)stateWithInner: (struct CCRTMachineState)inner;
- (CCRTRegisterValue)readFromRegister: (enum CCRTRegister) which;
- (CRTBitmap *)display;
@end

@interface CRTMachine : NSObject
@property(readonly) struct CCRTMachine *inner;
- (instancetype)init;
- (void)dealloc;
- (BOOL) start: (CRTProgram *)program error: (NSError **)error;
- (CRTMachineState *)waitForClock: (CRTClockValue)clock error: (NSError **)error;
@end

#endif /* CathodeRayTubeWrapper_h */
