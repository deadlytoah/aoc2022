//
//  QuizView.swift
//  aoc2022
//
//  Created by Hee Suk Shin on 2022/12/29.
//

import SwiftUI

protocol QuizImpl {
    init()
    func run(_ input: String) throws -> String
}

struct QuizView<IMPL: QuizImpl>: View {
    @State private var input: String = ""
    @State private var answer: String = ""
    @State private var isErrorShowing: Bool = false
    @State private var messageDescribingError: String? = nil

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
                Text("Answer")
                TextField("Answer", text: $answer)
            }
        }
        .alert("Error", isPresented: $isErrorShowing, presenting: messageDescribingError)
        { details in
            // Keep the default OK button.
        } message: { error in
            Text(error.description)
        }
        .navigationTitle(self.title)
        .padding()
    }

    func compute() {
        do {
            answer = try quizImpl.run(input)
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
