//
//  Day11Part1View.swift
//  aoc2022
//
//  Created by Hee Suk Shin on 2023/1/10.
//

import SwiftUI

struct Day11Part2QuizImpl: QuizImpl {
    func run(_ input: String) throws -> String {
        return try MonkeyInTheMiddle.runPart2(input)
    }
}

typealias Day11Part2View = QuizView<Day11Part2QuizImpl>

struct Day11Part2View_Previews: PreviewProvider {
    static var previews: some View {
        Day11Part2View(title: "Quiz")
    }
}
