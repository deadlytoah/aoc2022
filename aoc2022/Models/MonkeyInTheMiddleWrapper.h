//
//  MonkeyInTheMiddleWrapper.h
//  aoc2022
//
//  Created by Hee Suk Shin on 2023/01/16.
//

#ifndef MonkeyInTheMiddleWrapper_h
#define MonkeyInTheMiddleWrapper_h

#import <Foundation/Foundation.h>

@interface MonkeyInTheMiddle: NSObject
+(NSString *) runPart1: (NSString*) input error: (NSError **)error;
+(NSString *) runPart2: (NSString*) input error: (NSError **)error;
@end

#endif /* MonkeyInTheMiddleWrapper_h */
