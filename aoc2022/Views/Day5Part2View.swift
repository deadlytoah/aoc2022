//
//  Day5Part2View.swift
//  aoc2022
//
//  Created by Hee Suk Shin on 2022/12/26.
//

import SwiftUI

struct Day5Part2View: View {
    @State private var input: String = ""
    @State private var answer: String = ""
    @State private var isErrorShowing: Bool = false
    @State private var error: Error? = nil

    var body: some View {
        VStack {
            Text("Please enter the input below.")
            TextEditor(text: $input)
                .fontDesign(.monospaced)
        }
        .toolbar {
            ToolbarItemGroup {
                Button("Run", action: compute)
                Text("Answer")
                TextField("Answer", text: $answer)
            }
        }
        .alert("Error", isPresented: $isErrorShowing, presenting: error)
        { details in
            // Keep the default OK button.
        } message: { error in
            Text(error.localizedDescription)
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
                supplyStacks = instruction.execute(forPartTwo: supplyStacks)
            }
            answer = String(supplyStacks
                .stacks
                .map { stack in stack.top() })
        } catch let error {
            alertError(error)
        }
    }

    private func alertError(_ error: any Error) {
        self.error = error
        self.isErrorShowing = true
    }
}

struct Day5Part2View_Previews: PreviewProvider {
    static var previews: some View {
        Day5Part2View()
    }
}
