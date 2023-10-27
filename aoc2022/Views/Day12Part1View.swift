//
//  Day12Part1View.swift
//  aoc2022
//
//  Created by Hee Suk Shin on 2023/1/25.
//

import SwiftUI

struct Day12Part1QuizImpl: QuizImpl {
    func run(_ input: String) throws -> String {
        return try HillClimbingAlgorithmWrapper.runPart1(input)
    }
}

typealias Day12Part1View = QuizView<Day12Part1QuizImpl>

struct Day12Part1View_Previews: PreviewProvider {
    static var previews: some View {
        Day12Part1View(title: "Quiz")
    }
}
