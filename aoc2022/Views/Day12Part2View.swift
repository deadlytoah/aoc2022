//
//  Day12Part2View.swift
//  aoc2022
//
//  Created by Hee Suk Shin on 2023/2/7.
//

import SwiftUI

struct Day12Part2QuizImpl: QuizImpl {
    func run(_ input: String) throws -> String {
        return try HillClimbingAlgorithmWrapper.runPart2(input)
    }
}

typealias Day12Part2View = QuizView<Day12Part2QuizImpl>

struct Day12Part2View_Previews: PreviewProvider {
    static var previews: some View {
        Day12Part2View(title: "Quiz")
    }
}
