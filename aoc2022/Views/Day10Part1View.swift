//
//  Day10Part1View.swift
//  aoc2022
//
//  Created by Hee Suk Shin on 2023/1/10.
//

import SwiftUI

struct Day10Part1QuizImpl: QuizImpl {
    func run(_ input: String) throws -> String {
        let program = try CRTProgram.parse(input)
        guard let machine = CRTMachine() else {
            throw QuizError.memory
        }
        try machine.start(program)
        let interestingTimings: [CRTClockValue] = [20, 60, 100, 140, 180, 220]
        let answer = try interestingTimings
            .map { timing in
                let x = try machine
                    .wait(forClock: timing)
                    .read(from: CCRTRegisterX)
                return x * timing
            }
            .reduce(0) { s, n in s + n }
        return String(answer)
    }
}

typealias Day10Part1View = QuizView<Day10Part1QuizImpl>

struct Day10Part1View_Previews: PreviewProvider {
    static var previews: some View {
        Day10Part1View(title: "Quiz")
    }
}
