//
//  Day7Part1View.swift
//  aoc2022
//
//  Created by Hee Suk Shin on 2022/12/28.
//

import SwiftUI

struct Day7Part1QuizImpl : QuizImpl {
    func run(_ input: String) throws -> String {
        let lines = try noSpaceParseInput(input)
        let root = try NoSpaceObject.fromLinesInTerm(lines)
        root.calculateSize()
        let sum = try root.allDirectories()
            .filter { dir in try dir.size() <= 100000 }
            .map { dir in try dir.size() }
            .reduce(0) { sum, n in sum + n }
        return String(sum)
    }
}

typealias Day7Part1View = QuizView<Day7Part1QuizImpl>

struct Day7Part1View_Previews: PreviewProvider {
    static var previews: some View {
        Day7Part1View(title: "Quiz")
    }
}
