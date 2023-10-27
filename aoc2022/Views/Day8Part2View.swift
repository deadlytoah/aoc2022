//
//  Day8Part2View.swift
//  aoc2022
//
//  Created by Hee Suk Shin on 2023/1/3.
//

import SwiftUI

struct Day8Part2QuizImpl: QuizImpl {
    func run(_ input: String) throws -> String {
        let map = try TreetopMap.parse(input)
        try map.promèner { coord in
            map.computeScenicScore(in: coord)
        }
        let answer = map
            .scenicScores
            .max()!
        return String(answer)
    }
}

typealias Day8Part2View = QuizView<Day8Part2QuizImpl>

struct Day8Part2View_Previews: PreviewProvider {
    static var previews: some View {
        Day8Part2View(title: "Quiz")
    }
}
