//
//  RegolithReservoirWrapper.m
//  aoc2022
//
//  Created by Hee Suk Shin on 2023/02/16.
//

#import <Foundation/Foundation.h>

#import "RegolithReservoirWrapper.h"

#include <optional>
#include <utility>

#include "RegolithReservoir.hpp"

@implementation RegolithReservoirWrapper

+ (NSString*)runPart1: (NSString*)input withVisualisation: (BOOL) enableVisualisation error: (NSError**)error {
    auto answer = rr::runPart1(std::move([input cStringUsingEncoding:NSUTF8StringEncoding]), enableVisualisation);
    return [NSString stringWithCString: answer.c_str() encoding:NSUTF8StringEncoding];
}

+ (NSString*)runPart2: (NSString*)input withVisualisation: (BOOL) enableVisualisation error: (NSError**)error {
    auto answer = rr::runPart2(std::move([input cStringUsingEncoding:NSUTF8StringEncoding]), enableVisualisation);
    return [NSString stringWithCString: answer.c_str() encoding:NSUTF8StringEncoding];
}

@end
