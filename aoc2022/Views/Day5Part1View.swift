//
//  Day5Part1View.swift
//  aoc2022
//
//  Created by Hee Suk Shin on 2022/12/25.
//

import SwiftUI

struct Day5Part1View: View {
    @State private var input: String = ""
    @State private var answer: String = ""

    var body: some View {
        VStack {
            Text("Please enter the input below.")
            TextEditor(text: $input)
        }
        .toolbar {
            ToolbarItemGroup {
                Button("Run", action: compute)
                Text("Answer")
                TextField("Answer", text: $answer)
            }
        }
        .navigationTitle("Day 5 Supply Stacks Part One")
        .padding()
    }

    func compute() {
        do {
            let input = try SupplyStacks.parse(input)
            var supplyStacks = input.supplyStacks
            let instructions = try parseInstructionsForSupplyStacks(input.rest)
            for instruction in instructions {
                supplyStacks = instruction.execute(supplyStacks)
            }
            answer = String(supplyStacks
                .stacks
                .map { stack in stack.top() })
        } catch let error {
            input.append("\n" + error.localizedDescription)
        }
    }
}

struct Day5Part1View_Previews: PreviewProvider {
    static var previews: some View {
        Day5Part1View()
    }
}
