//
//  Day1View.swift
//  aoc2022
//
//  Created by Hee Suk Shin on 2022/12/18.
//

import SwiftUI

struct Day1View: View {
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
        .navigationTitle("Day 1 Calorie Counting")
        .padding()
    }

    private func compute() {
        var inventories = parseSequenceOfInventories(input)
        inventories = inventories.map {
            var inventory = $0
            inventory.compute()
            return inventory
        }
        if let largestTotal = inventories.max(by: { a, b in
            a.total < b.total
        })?.total {
            answer = String(largestTotal)
        }
    }
}

struct Day1View_Previews: PreviewProvider {
    static var previews: some View {
        Day1View()
    }
}
