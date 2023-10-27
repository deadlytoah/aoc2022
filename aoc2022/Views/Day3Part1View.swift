//
//  Day3Part1View.swift
//  aoc2022
//
//  Created by Hee Suk Shin on 2022/12/20.
//

import SwiftUI

struct Day3Part1View: View {
    @State private var input: String = ""
    @State private var answer: String = ""

    var body: some View {
        VStack {
            Text("Please enter the input here.")
            TextEditor(text: $input)
        }
        .navigationTitle("Day 3 Rucksack Reorganization")
        .toolbar {
            ToolbarItemGroup {
                Button("Run", action: compute)
                Text("Answer:")
                TextField("Answer", text: $answer)
            }
        }
        .padding()
    }

    private func compute() {
        if !input.isEmpty {
            do {
                let rucksacks = try decodeContentsOfRucksacks(input)
                answer = String(sumPrioritiesOfRucksacks(rucksacks))
            } catch let error {
                answer = String(describing: error)
            }
        }
    }
}

struct Day3Part1View_Previews: PreviewProvider {
    static var previews: some View {
        Day3Part1View()
    }
}
