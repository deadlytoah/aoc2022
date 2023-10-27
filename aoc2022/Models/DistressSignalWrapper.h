//
//  DistressSignalWrapper.h
//  aoc2022
//
//  Created by Hee Suk Shin on 2023/02/07.
//

#ifndef DistressSignalWrapper_h
#define DistressSignalWrapper_h

#import <Foundation/Foundation.h>

@interface DistressSignalWrapper: NSObject
+(NSString *) runPart1: (NSString*) input error: (NSError **)error;
+(NSString *) runPart2: (NSString*) input error: (NSError **)error;
@end

#endif /* DistressSignalWrapper_h */
