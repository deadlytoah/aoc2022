//
//  RuckSack.swift
//  aoc2022
//
//  Created by Hee Suk Shin on 2022/12/20.
//

import Foundation

enum RucksackError: Error {
    case argument
    case input
}

struct RucksackItem: Hashable {
    var priority: UInt32

    init(_ item: Character) throws {
        if let code = item.asciiValue {
            if code >= 65 && code <= 90 {
                self.priority = UInt32(code - 65 + 27)
            } else if code >= 97 && code <= 122 {
                self.priority = UInt32(code - 97 + 1)
            } else {
                throw RucksackError.argument
            }
        } else {
            throw RucksackError.argument
        }
    }

    func getPriority() -> UInt32 {
        return self.priority
    }
}

struct RucksackCompartment {
    var items: [RucksackItem]

    init(_ items: [RucksackItem]) {
        self.items = items
    }

    func intersect(_ other: RucksackCompartment) -> [RucksackItem] {
        let set1 = Set(self.items)
        let set2 = Set(other.items)
        return Array(set1.intersection(set2))
    }

    static func decode(_ s: Substring) throws -> RucksackCompartment {
        let items: [RucksackItem] = try s.map { c in try RucksackItem(c) }
        return RucksackCompartment(items)
    }
}

struct Rucksack {
    var compartment1: RucksackCompartment
    var compartment2: RucksackCompartment

    init(_ compartment1: RucksackCompartment, _ compartment2: RucksackCompartment) {
        self.compartment1 = compartment1
        self.compartment2 = compartment2
    }

    func getPriority() -> UInt32 {
        compartment1
            .intersect(compartment2)
            .reduce(0) { s, item in
                s + item.getPriority()
            }
    }

    func itemSet() -> Set<RucksackItem> {
        return Set(self.compartment1.items).union(Set(self.compartment2.items))
    }

    func intersectingItems(_ other: Rucksack) -> Set<RucksackItem> {
        return self.itemSet().intersection(other.itemSet())
    }

    static func decode(_ s: Substring) throws -> Rucksack {
        guard s.count % 2 == 0 else {
            throw RucksackError.input
        }
        let mid = s.count / 2
        let compartment1 = try RucksackCompartment.decode(s.prefix(mid))
        let compartment2 = try RucksackCompartment.decode(s.suffix(mid))
        return Rucksack(compartment1, compartment2)
    }
}

struct RucksackGroupOfElves {
    var rucksacks: [Rucksack]

    init(_ rucksacks: [Rucksack]) {
        self.rucksacks = rucksacks
    }

    func intersectingItem() throws -> RucksackItem {
        let items =  self.rucksacks[0].intersectingItems(self.rucksacks[1]).intersection(self.rucksacks[2].itemSet())
        if items.isEmpty || items.count > 1 {
            throw RucksackError.input
        } else {
            return items.first!
        }
    }
}

func decodeRucksacksForGroupsOfElves(_ input: String) throws -> [RucksackGroupOfElves] {
    var groups: [RucksackGroupOfElves] = []
    var stack: [Rucksack] = []
    for line in input.split(separator: "\n") {
        stack.append(try Rucksack.decode(line))
        if stack.count >= 3 {
            groups.append(RucksackGroupOfElves(stack))
            stack = []
        }
    }
    return groups
}

func decodeContentsOfRucksacks(_ input: String) throws -> [Rucksack] {
    var rucksacks: [Rucksack] = []
    let lines = input.split(separator: "\n")
    for line in lines {
        rucksacks.append(try Rucksack.decode(line))
    }
    return rucksacks
}

func sumPrioritiesOfRucksacks(_ rucksacks: [Rucksack]) -> UInt32 {
    return rucksacks.reduce(0) { s, rucksack in
        s + rucksack.getPriority()
    }
}
