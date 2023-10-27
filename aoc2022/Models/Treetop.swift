//
//  Treetop.swift
//  aoc2022
//
//  Created by Hee Suk Shin on 2023/01/03.
//

import Foundation

enum TreetopBearing {
    case north
    case south
    case west
    case east
}

struct TreetopDirection: Equatable, Hashable {
    let bearing: TreetopBearing

    static let north = TreetopDirection(from: .north)
    static let south = TreetopDirection(from: .south)
    static let west = TreetopDirection(from: .west)
    static let east = TreetopDirection(from: .east)

    init(from bearing: TreetopBearing) {
        self.bearing = bearing
    }

    var from: TreetopBearing {
        get {
            bearing
        }
    }

    var to: TreetopBearing {
        get {
            switch bearing {
            case .north:
                return .south
            case .south:
                return .north
            case .west:
                return .east
            case .east:
                return .west
            }
        }
    }
}

enum TreetopVisibilityState {
    case visible
    case hidden
    case unknown
}

struct TreetopVisibility: Equatable {
    let direction: TreetopDirection
    let visibility: TreetopVisibilityState
}

class TreetopCoord {
    let i: Int
    let j: Int

    init(_ i: Int, _ j: Int) {
        self.i = i
        self.j = j
    }

    func up() -> TreetopCoord {
        TreetopCoord(i - 1, j)
    }

    func down() -> TreetopCoord {
        TreetopCoord(i + 1, j)
    }

    func left() -> TreetopCoord {
        TreetopCoord(i, j - 1)
    }

    func right() -> TreetopCoord {
        TreetopCoord(i, j + 1)
    }
}

class TreetopSafeCoord: TreetopCoord, Equatable {
    static func == (lhs: TreetopSafeCoord, rhs: TreetopSafeCoord) -> Bool {
        lhs.i == rhs.i && lhs.j == rhs.j
    }

    static internal func unchecked(_ i: Int, _ j: Int) -> TreetopSafeCoord {
        TreetopSafeCoord(i, j)
    }

    static internal func unchecked(coord: TreetopCoord) -> TreetopSafeCoord {
        TreetopSafeCoord(coord.i, coord.j)
    }

    static internal func verified(_ coord: TreetopCoord) -> TreetopSafeCoord {
        TreetopSafeCoord(coord.i, coord.j)
    }
}

struct TreetopSafeRange {
    let start: TreetopSafeCoord
    let end: TreetopSafeCoord
    let direction: TreetopDirection

    func forEach(doing: (TreetopSafeCoord) -> ()) {
        var this = start
        while this != end {
            doing(this)
            switch direction.from {
            case .north:
                this = TreetopSafeCoord.unchecked(coord: this.down())
            case .south:
                this = TreetopSafeCoord.unchecked(coord: this.up())
            case .west:
                this = TreetopSafeCoord.unchecked(coord: this.right())
            case .east:
                this = TreetopSafeCoord.unchecked(coord: this.left())
            }
        }
        doing(end)
    }
}

class TreetopTree {
    let height: UInt8
    var score: UInt32 = 0
    var visibilities: [TreetopDirection: TreetopVisibility] = [:]

    init(_ height: UInt8) {
        self.height = height
        visibilities[.north] = TreetopVisibility(direction: .north, visibility: .unknown)
        visibilities[.south] = TreetopVisibility(direction: .south, visibility: .unknown)
        visibilities[.west] = TreetopVisibility(direction: .west, visibility: .unknown)
        visibilities[.east] = TreetopVisibility(direction: .east, visibility: .unknown)
    }

    func visibility(direction: TreetopDirection) -> TreetopVisibility {
        visibilities[direction]!
    }

    func setVisibility(_ visibility: TreetopVisibilityState, direction: TreetopDirection) {
        visibilities[direction] = TreetopVisibility(direction: direction, visibility: visibility)
    }

    func isAnyUnknown() -> Bool {
        visibilities.contains { (_k, v) in
            v.visibility == .unknown
        }
    }

    func isSomeVisible() -> Bool {
        visibilities.contains { (_k, v) in
            v.visibility == .visible
        }
    }

    static func parse(_ ch: Character) throws -> TreetopTree {
        if let height = UInt8(String(ch)) {
            let tree = TreetopTree(height)
            return tree
        } else {
            throw QuizError.parse
        }
    }
}

class TreetopMap {
    var trees: [[TreetopTree]]
    var visited: [[Bool]] = []

    init(_ map: [Substring]) throws {
        self.trees = try map
            .map { line in try line.map { ch in try TreetopTree.parse(ch) } }
    }

    func width() -> Int {
        trees[0].count
    }

    func height() -> Int {
        trees.count
    }

    func tree(at: TreetopSafeCoord) -> TreetopTree {
        trees[at.i][at.j]
    }

    func verify(coord: TreetopCoord) -> TreetopSafeCoord? {
        if coord.i >= 0 && coord.j >= 0 && coord.i < height() && coord.j < width() {
            return TreetopSafeCoord.verified(coord)
        } else {
            return nil
        }
    }

    enum FetchResult<R> {
        case conclusive(R)
        case inconclusive([TreetopSafeCoord])
    }

    func notNil<T>(_ obj: T?) throws -> T {
        if let obj = obj {
            return obj
        } else {
            throw QuizError.unexpectedNil
        }
    }

    /// Lists the cells at the edge in the given direction.
    func edgeCells(in bearing: TreetopBearing) throws -> [TreetopSafeCoord] {
        switch bearing {
        case .north:
            return try (0..<width()).map { j in
                try notNil(verify(coord: TreetopCoord(0, j)))
            }
        case .south:
            return try (0..<width()).map { j in
                try notNil(verify(coord: TreetopCoord(height() - 1, j)))
            }
        case .west:
            return try (0..<height()).map { i in
                try notNil(verify(coord: TreetopCoord(i, 0)))
            }
        case .east:
            return try (0..<height()).map { i in
                try notNil(verify(coord: TreetopCoord(i, width() - 1)))
            }
        }
    }

    func rangeUpToEnd(from start: TreetopSafeCoord, inDirection direction: TreetopDirection) throws -> TreetopSafeRange {
        let end: TreetopSafeCoord?
        switch direction.from {
        case .north:
            end = verify(coord: TreetopCoord(height() - 1, start.j))
        case .south:
            end = verify(coord: TreetopCoord(0, start.j))
        case .west:
            end = verify(coord: TreetopCoord(start.i, width() - 1))
        case .east:
            end = verify(coord: TreetopCoord(start.i, 0))
        }
        if let end = end {
            return TreetopSafeRange(start: start, end: end, direction: direction)
        } else {
            throw QuizError.logic("failed to verify the range's end coordinate")
        }
    }

    func forEachSeries(direction: TreetopDirection, doing: (TreetopSafeRange) throws -> ()) throws {
        for start in try edgeCells(in: direction.from) {
            try doing(try rangeUpToEnd(from: start, inDirection: direction))
        }
    }

    func computeVisibilitiesOf(series: TreetopSafeRange) throws {
        var closestVisible: TreetopTree? = nil
        series.forEach { coord in
            let this = tree(at: coord)
            if let closestVisibleUnwrapped = closestVisible {
                if closestVisibleUnwrapped.height >= this.height {
                    this.setVisibility(.hidden, direction: series.direction)
                } else {
                    this.setVisibility(.visible, direction: series.direction)
                    closestVisible = this
                }
            } else {
                closestVisible = this
                this.setVisibility(.visible, direction: series.direction)
            }
        }
    }

    func computeVisibilities(direction: TreetopDirection) throws {
        try forEachSeries(direction: direction) { series in
            try computeVisibilitiesOf(series: series)
        }
    }

    func countVisible() throws -> Int {
        try trees
            .map { line in
                try line.filter { tree in
                    if !tree.isAnyUnknown() {
                        return tree.isSomeVisible()
                    } else {
                        throw QuizError.logic("At least one tree's visibility is unknown")
                    }
                }
                .count
            }
            .reduce(0) { s, n in s + n }
    }

    func promèner(_ doing: (TreetopSafeCoord) throws -> ()) throws {
        for i in 0..<trees.count {
            for j in 0..<trees[0].count {
                // We know this coordinate to be safe.
                let coord = TreetopSafeCoord.unchecked(i, j)
                try doing(coord)
            }
        }
    }

    func step(from: TreetopSafeCoord, in direction: TreetopDirection) -> TreetopCoord {
        switch direction.to {
        case .north:
            return from.up()
        case .south:
            return from.down()
        case .west:
            return from.left()
        case .east:
            return from.right()
        }
    }

    func countTrees(from origin: TreetopSafeCoord, in direction: TreetopDirection, until: (TreetopSafeCoord) -> Bool) -> UInt32 {
        var this = origin
        var count: UInt32 = 0
        while let next = verify(coord: step(from: this, in: direction)) {
            count += 1
            if until(next) {
                break
            } else {
                this = next
            }
        }
        return count
    }

    func computeScenicScore(in coord: TreetopSafeCoord) {
        let this = tree(at: coord)
        let score = [TreetopDirection.north, .south, .west, .east]
            .map { direction in
                countTrees(from: coord, in: direction, until: { coord in
                    tree(at: coord).height >= this.height
                })
            }
            .reduce(1) { score, s in
                score * s
            }
        this.score = score
    }

    var scenicScores: [UInt32] {
        get {
            trees.flatMap { row in
                row.map { tree in
                    tree.score
                }
            }
        }
    }

    static func parse(_ input: String) throws -> TreetopMap {
        try TreetopMap(input.split(separator: "\n"))
    }
}
