//
//  Day2View.swift
//  aoc2022
//
//  Created by Hee Suk Shin on 2022/12/18.
//

import SwiftUI

struct Day1Part2View: View {
    @State private var input: String = ""
    @State private var answer: String = ""

    var body: some View {
        HStack {
            VStack {
                Text("Please enter the input here.")
                TextEditor(text: $input)
            }
            VStack {
                Button("Find Answer", action: compute)
                Text("Answer:")
                TextField("Answer", text: $answer)
            }
        }
        .navigationTitle("Day 1 Calorie Counting - Part Two")
        .padding()
    }

    private func compute() {
        var inventories = parseSequenceOfInventories(input)
        inventories = inventories.map {
            var inventory = $0
            inventory.compute()
            return inventory
        }
        inventories = inventories.sorted { a, b in
            a.total > b.total
        }
        if inventories.count >= 3 {
            answer = String(inventories[0].total + inventories[1].total + inventories[2].total)
        } else {
            answer = "error"
        }
    }
}

struct Day1Part2View_Previews: PreviewProvider {
    static var previews: some View {
        Day1Part2View()
    }
}
