//
//  Day3Part2View.swift
//  aoc2022
//
//  Created by Hee Suk Shin on 2022/12/21.
//

import SwiftUI

struct Day3Part2View: View {
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
        .navigationTitle("Day 3 Rucksack Reorganization Part Two")
        .padding()
    }

    func compute() {
        do {
            let groupsOfElves = try decodeRucksacksForGroupsOfElves(input)
            answer = String(try groupsOfElves
                .map { group in
                    try group
                        .intersectingItem()
                        .getPriority()
                }
                .reduce(0) { s, priority in
                    s + priority
                })
        } catch let error {
            answer = error.localizedDescription
        }
    }
}

struct Day3Part2View_Previews: PreviewProvider {
    static var previews: some View {
        Day3Part2View()
    }
}
