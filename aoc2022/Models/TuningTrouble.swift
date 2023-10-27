//
//  TuningTrouble.swift
//  aoc2022
//
//  Created by Hee Suk Shin on 2022/12/27.
//

import Foundation

enum TuningTroubleError: Error {
    case missingMarker
}

private func isEachAlphabetDistinct(_ candidate: [Character]) -> Bool {
    // Search for equality in each alphabet in the candidate.
    var stack = Array(candidate)
    while !stack.isEmpty {
        let a = stack.popLast()!
        for b in stack {
            if a == b {
                return false
            }
        }
    }
    return true
}

func tuningTroubleSearchForMarker(in input: Substring, ofLength length: Int) throws -> (Int, Substring) {
    var stack: [Character] = []
    for (i, ch) in input.enumerated() {
        if stack.count < length {
            stack.append(ch)
        } else if isEachAlphabetDistinct(stack) {
            return (i, input.suffix(input.count - i))
        } else {
            stack.remove(at: 0)
            stack.append(ch)
        }
    }
    throw TuningTroubleError.missingMarker
}
