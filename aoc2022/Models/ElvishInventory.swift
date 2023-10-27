//
//  ElvishInventory.swift
//  aoc2022
//
//  Created by Hee Suk Shin on 2022/12/18.
//

import Foundation

struct ElvishInventory {
    var items: [UInt32] = []
    var total: UInt32 = 0
    var error: String? = nil

    init(_ group: [Substring]) {
        self.items = group.map { s in
            if let d = UInt32(s) {
                return d
            } else {
                error = "Error converting integer to string"
                return 0
            }
        }
    }

    mutating func compute() {
        self.total = self.items.reduce(0) { s, x in
            s + x
        }
    }
}

func parseSequenceOfInventories(_ input: String) -> [ElvishInventory] {
    var inventories: [ElvishInventory] = []
    let lines = input.split(separator: "\n", omittingEmptySubsequences: false)
    var stack: [Substring] = []
    for line in lines {
        if line == "" {
            inventories.append(ElvishInventory(stack))
            stack = []
        } else {
            stack.append(line)
        }
    }
    if !stack.isEmpty {
        // This handles the case of no trailing new line character in the input.
        inventories.append(ElvishInventory(stack))
    }
    return inventories
}
