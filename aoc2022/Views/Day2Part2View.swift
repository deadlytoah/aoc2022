//
//  Day2Part2View.swift
//  aoc2022
//
//  Created by Hee Suk Shin on 2022/12/19.
//

import SwiftUI

struct Day2Part2View: View {
    @State private var input: String = ""
    @State private var answer: String = ""

    var body: some View {
        HStack {
            VStack {
                Text("Please enter the input here.")
                TextEditor(text: $input)
            }
            VStack {
                Button("Total Score", action: compute)
                Text("Total Score:")
                TextField("Total Score", text: $answer)
            }
        }
        .navigationTitle("Day 2 Rock Paper Scissors Part Two")
        .padding()
    }

    private func compute() {
        do {
            let strategyGuide = try parseStrategyGuideOfRPCTourney(forPartTwo: input)
            answer = String(strategyGuide.computeTotalScore())
        } catch let error {
            answer = String(describing: error)
        }
    }
}

struct Day2Part2View_Previews: PreviewProvider {
    static var previews: some View {
        Day2Part2View()
    }
}
