//
//  Day4Part2View.swift
//  aoc2022
//
//  Created by Hee Suk Shin on 2022/12/22.
//

import SwiftUI

struct Day4Part2View: View {
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
        .navigationTitle("Day 4 Camp Cleanup Part Two")
        .padding()
    }

    func compute() {
        do {
            let pairsOfCampCleanupAssignments = try parsePairsOfCampCleanupAssignments(input)
            answer = String(pairsOfCampCleanupAssignments
                .filter { pair in
                    pair[0].overlaps(pair[1])
                }
                .count)
        } catch let error {
            answer = error.localizedDescription
        }
    }
}

struct Day4Part2View_Previews: PreviewProvider {
    static var previews: some View {
        Day4Part2View()
    }
}
