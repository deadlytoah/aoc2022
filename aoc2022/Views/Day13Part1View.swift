//
//  Day13Part1View.swift
//  aoc2022
//
//  Created by Hee Suk Shin on 2023/2/7.
//

import SwiftUI

struct Day13Part1QuizImpl: QuizImpl {
    func run(_ input: String) throws -> String {
        return try DistressSignalWrapper.runPart1(input)
    }
}

typealias Day13Part1View = QuizView<Day13Part1QuizImpl>

struct Day13Part1View_Previews: PreviewProvider {
    static var previews: some View {
        Day13Part1View(title: "Quiz")
    }
}
