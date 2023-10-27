//
//  LargeOutputView.swift
//  aoc2022
//
//  Created by Hee Suk Shin on 2023/01/13.
//

import SwiftUI

struct LargeOutputView: View {
    @State private var text: String

    init(text: String) {
        self.text = text
    }

    var body: some View {
        TextEditor(text: $text)
            .fontDesign(.monospaced)
            .padding()
    }
}
