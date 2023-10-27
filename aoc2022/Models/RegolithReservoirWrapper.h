//
//  RegolithReservoirWrapper.h
//  aoc2022
//
//  Created by Hee Suk Shin on 2023/02/16.
//

#ifndef RegolithReservoirWrapper_h
#define RegolithReservoirWrapper_h

@interface RegolithReservoirWrapper : NSObject

+ (NSString*)runPart1: (NSString*)input withVisualisation: (BOOL) enableVisualisation error: (NSError**)error;
+ (NSString*)runPart2: (NSString*)input withVisualisation: (BOOL) enableVisualisation error: (NSError**)error;

@end

#endif /* RegolithReservoirWrapper_h */
