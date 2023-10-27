//
//  Day8Part1View.swift
//  aoc2022
//
//  Created by Hee Suk Shin on 2022/12/29.
//

import SwiftUI

struct Day8Part1QuizImpl: QuizImpl {
    func run(_ input: String) throws -> String {
        let map = try TreetopMap.parse(input)
        for direction in [TreetopDirection.north, .south, .west, .east] {
            try map.computeVisibilities(direction: direction)
        }
        return String(try map.countVisible())
    }
}

typealias Day8Part1View = QuizView<Day8Part1QuizImpl>

struct Day8Part1View_Previews: PreviewProvider {
    static var previews: some View {
        Day8Part1View(title: "Quiz")
    }
}
