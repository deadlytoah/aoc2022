//
//  Day11Part1View.swift
//  aoc2022
//
//  Created by Hee Suk Shin on 2023/1/10.
//

import SwiftUI

struct Day11Part1QuizImpl: QuizImpl {
    func run(_ input: String) throws -> String {
        return try MonkeyInTheMiddle.runPart1(input)
    }
}

typealias Day11Part1View = QuizView<Day11Part1QuizImpl>

struct Day11Part1View_Previews: PreviewProvider {
    static var previews: some View {
        Day11Part1View(title: "Quiz")
    }
}
