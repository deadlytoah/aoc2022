//
//  Day10Part2View.swift
//  aoc2022
//
//  Created by Hee Suk Shin on 2023/1/13.
//

import SwiftUI

struct Day10Part2QuizImpl: QuizImpl {
    func run(_ input: String) throws -> String {
        let program = try CRTProgram.parse(input)
        guard let machine = CRTMachine() else {
            throw QuizError.memory
        }
        try machine.start(program)
        let answer = try machine
            .wait(forClock: 240)
            .display()
            .description!
        return answer
    }
}

typealias Day10Part2View = QuizWithLargeOutputView<Day10Part2QuizImpl>
