//
//  Day14Part2View.swift
//  aoc2022
//
//  Created by Hee Shin on 2023/7/31.
//

import SwiftUI
import SwiftyZeroMQ

struct Day14Part2View: View {
    @State private var input: String = ""
    @State private var isErrorShowing: Bool = false
    @State private var messageDescribingError: String? = nil

    @Binding var path: NavigationPath

    let title: String

    var body: some View {
        VStack {
            Text("Please enter the input below.")
            TextEditor(text: $input)
                .fontDesign(.monospaced)
        }
        .toolbar {
            ToolbarItemGroup {
                Button("Run", action: compute)
            }
        }
        .alert("Error", isPresented: $isErrorShowing, presenting: messageDescribingError)
        { details in
            // Keep the default OK button.
        } message: { error in
            Text(error.description)
        }
        .navigationTitle(title)
        .padding()
    }

    func compute() {
        do {
            let context = try SwiftyZeroMQ.Context();
            let answer = try run(input, withVisualisation: true)
            path.append(Visualisation(context: context, answer: answer))
        } catch let error as QuizError {
            alertError(error.description)
        } catch let error {
            alertError(error.localizedDescription)
        }
    }

    private func alertError(_ message: String) {
        self.messageDescribingError = message
        self.isErrorShowing = true
    }

    func run(_ input: String, withVisualisation: Bool) throws -> String {
        return try RegolithReservoirWrapper.runPart2(input, withVisualisation: withVisualisation)
    }
}
