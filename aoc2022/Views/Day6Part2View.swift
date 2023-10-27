//
//  Day6Part2View.swift
//  aoc2022
//
//  Created by Hee Suk Shin on 2022/12/28.
//

import SwiftUI

struct Day6Part2View: View {
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
        .navigationTitle("Day 5 Supply Stacks Part Two")
        .padding()
    }

    func compute() {
        do {
            try computeImpl()
        } catch let error {
            alertError(error)
        }
    }

    func computeImpl() throws {
        let (startIndex, _) = try tuningTroubleSearchForMarker(in: input[...], ofLength: 14)
        answer = String(startIndex)
    }

    private func alertError(_ error: any Error) {
        self.error = error
        self.isErrorShowing = true
    }
}

struct Day6Part2View_Previews: PreviewProvider {
    static var previews: some View {
        Day6Part2View()
    }
}
