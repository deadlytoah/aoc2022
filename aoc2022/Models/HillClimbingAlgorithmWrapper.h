//
//  HillClimbingAlgorithmWrapper.h
//  aoc2022
//
//  Created by Hee Suk Shin on 2023/01/25.
//

#ifndef HillClimbingAlgorithmWrapper_h
#define HillClimbingAlgorithmWrapper_h

#import <Foundation/Foundation.h>

@interface HillClimbingAlgorithmWrapper: NSObject
+(NSString *) runPart1: (NSString*) input error: (NSError **)error;
+(NSString *) runPart2: (NSString*) input error: (NSError **)error;
@end

#endif /* HillClimbingAlgorithmWrapper_h */
