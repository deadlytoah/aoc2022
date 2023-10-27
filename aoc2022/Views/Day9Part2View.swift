//
//  Day9Part2View.swift
//  aoc2022
//
//  Created by Hee Suk Shin on 2023/1/10.
//

import SwiftUI

struct Day9Part2QuizImpl: QuizImpl {
    func run(_ input: String) throws -> String {
        let seriesOfInstructions = try RopeBridgeInstruction.parse(input)
        if let bridge = RopeBridge(length: 10) {
            for instruction in seriesOfInstructions {
                // Run the instruction one step at a time.
                for _ in 0..<instruction.steps {
                    let steppingInstruction = RopeBridgeInstruction(direction: instruction.direction)
                    bridge.follow(steppingInstruction)
                }
            }
            return String(bridge.visitedLocations.count)
        } else {
            throw QuizError.memory
        }
    }
}

typealias Day9Part2View = QuizView<Day9Part2QuizImpl>

struct Day9Part2View_Previews: PreviewProvider {
    static var previews: some View {
        Day9Part2View(title: "Quiz")
    }
}
