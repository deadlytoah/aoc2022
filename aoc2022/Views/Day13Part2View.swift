//
//  Day13Part2View.swift
//  aoc2022
//
//  Created by Hee Suk Shin on 2023/2/14.
//

import SwiftUI

struct Day13Part2QuizImpl: QuizImpl {
    func run(_ input: String) throws -> String {
        return try DistressSignalWrapper.runPart2(input)
    }
}

typealias Day13Part2View = QuizView<Day13Part2QuizImpl>

struct Day13Part2View_Previews: PreviewProvider {
    static var previews: some View {
        Day13Part2View(title: "Quiz")
    }
}
