//
//  Day7Part2View.swift
//  aoc2022
//
//  Created by Hee Suk Shin on 2022/12/29.
//

import SwiftUI

struct Day7Part2QuizImpl: QuizImpl {
    // Constants from Quiz
    let DiskSize: UInt64 = 70000000
    let SizeInNeed: UInt64 = 30000000

    func run(_ input: String) throws -> String {
        let lines = try noSpaceParseInput(input)
        let root = try NoSpaceObject.fromLinesInTerm(lines)
        root.calculateSize()
        let sizeInUse = try root.size()
        let freeSize = DiskSize - sizeInUse
        let sizeToFree = SizeInNeed - freeSize
        let answer = try root.allDirectories()
            .filter { dir in try dir.size() >= sizeToFree }
            .map { dir in try dir.size() }
            .sorted()
            .first!
        return String(answer)
    }
}

typealias Day7Part2View = QuizView<Day7Part2QuizImpl>

struct Day7Part2View_Previews: PreviewProvider {
    static var previews: some View {
        Day7Part2View(title: "Quiz")
    }
}
