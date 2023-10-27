//
//  CampCleanup.swift
//  aoc2022
//
//  Created by Hee Suk Shin on 2022/12/22.
//

import Foundation

enum CampCleanupError: Error {
    case input
}

struct CampCleanupAssignment {
    var range: [UInt32]

    init(_ s: Substring) throws {
        self.range = try s.split(separator: "-", maxSplits: 1)
            .map { t in
                if let num = UInt32(t) {
                    return num
                } else {
                    throw CampCleanupError.input
                }
            }
    }

    func includes(_ other: CampCleanupAssignment) -> Bool {
        return range[0] <= other.range[0] && range[1] >= other.range[1]
    }

    func overlaps(_ other: CampCleanupAssignment) -> Bool {
        return (range[0]...range[1]).overlaps(other.range[0]...other.range[1])
    }
}

func parsePairsOfCampCleanupAssignments(_ input: String) throws -> [[CampCleanupAssignment]] {
    try input
        .split(separator: "\n")
        .map { line in
            try line.split(separator: ",", maxSplits: 1)
                .map { s in
                    try CampCleanupAssignment(s)
                }
        }
}
