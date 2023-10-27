//
//  QuizWithLargeOutputView.swift
//  aoc2022
//
//  Created by Hee Suk Shin on 2023/01/13.
//

import SwiftUI

struct QuizWithLargeOutputView<IMPL: QuizImpl>: View {
    @State private var input: String = ""
    @State private var isErrorShowing: Bool = false
    @State private var messageDescribingError: String? = nil

    @Binding var path: NavigationPath

    let title: String
    private let quizImpl: QuizImpl = IMPL()

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
        .navigationTitle("Day 10 Cathode-Ray Tube Part Two")
        .padding()
    }

    func compute() {
        do {
            let answer = try quizImpl.run(input)
            path.append(LargeOutput(text: answer))
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
}
