//
//  ContentView.swift
//  aoc2022
//
//  Created by Hee Suk Shin on 2022/12/18.
//

import SwiftUI

struct ContentView: View {
    @State private var path: NavigationPath = NavigationPath()

    struct Selection: Hashable {
        let day: UInt32
        let part: UInt32

        func hash(into hasher: inout Hasher) {
            hasher.combine(day)
            hasher.combine(part)
        }

        static func == (lhs: Self, rhs: Self) -> Bool {
            lhs.day == rhs.day && lhs.part == rhs.part
        }
    }

    struct QuizPart: Identifiable {
        let id = UUID()
        let number: UInt32
    }

    struct Quiz: Identifiable {
        let id = UUID()
        let day: UInt32
        let title: String
        let parts: [QuizPart]
    }

    private let quizes: [Quiz] = [
        Quiz(day: 1, title: "Calorie Counting", parts: [
            QuizPart(number: 1),
            QuizPart(number: 2)]),
        Quiz(day: 2, title: "Rock Paper Scissors", parts: [
            QuizPart(number: 1),
            QuizPart(number: 2)]),
        Quiz(day: 3, title: "Rucksack Reorganization", parts: [
            QuizPart(number: 1),
            QuizPart(number: 2)]),
        Quiz(day: 4, title: "Camp Cleanup", parts: [
            QuizPart(number: 1),
            QuizPart(number: 2)]),
        Quiz(day: 5, title: "Supply Stacks", parts: [
            QuizPart(number: 1),
            QuizPart(number: 2)]),
        Quiz(day: 6, title: "Tuning Trouble", parts: [
            QuizPart(number: 1),
            QuizPart(number: 2)]),
        Quiz(day: 7, title: "No Space Left On Device", parts: [
            QuizPart(number: 1),
            QuizPart(number: 2)]),
        Quiz(day: 8, title: "Treetop Tree House", parts: [
            QuizPart(number: 1),
            QuizPart(number: 2)]),
        Quiz(day: 9, title: "Rope Bridge", parts: [
            QuizPart(number: 1),
            QuizPart(number: 2)]),
        Quiz(day: 10, title: "Cathode-Ray Tube", parts: [
            QuizPart(number: 1),
            QuizPart(number: 2)]),
        Quiz(day: 11, title: "Monkey in the Middle", parts: [
            QuizPart(number: 1),
            QuizPart(number: 2)]),
        Quiz(day: 12, title: "Hill Climbing Algorithm", parts: [
            QuizPart(number: 1),
            QuizPart(number: 2)]),
        Quiz(day: 13, title: "Distress Signal", parts: [
            QuizPart(number: 1),
            QuizPart(number: 2)]),
        Quiz(day: 14, title: "Regolith Reservoir", parts: [
            QuizPart(number: 1),
            QuizPart(number: 2)]),
    ]

    var body: some View {
        NavigationStack(path: $path) {
            List(quizes) { quiz in
                Section(header: Text("Day \(quiz.day) \(quiz.title)")) {
                    ForEach(quiz.parts) { part in
                        NavigationLink("Part \(part.number)", value: Selection(day: quiz.day, part: part.number))
                    }
                }
            }
            .navigationTitle("Advent of Code 2022")
            .navigationDestination(for: Selection.self) { selection in
                switch selection {
                case Selection(day: 1, part: 1):
                    Day1View()
                case Selection(day: 1, part: 2):
                    Day1Part2View()
                case Selection(day: 2, part: 1):
                    Day2Part1View()
                case Selection(day: 2, part: 2):
                    Day2Part2View()
                case Selection(day: 3, part: 1):
                    Day3Part1View()
                case Selection(day: 3, part: 2):
                    Day3Part2View()
                case Selection(day: 4, part: 1):
                    Day4Part1View()
                case Selection(day: 4, part: 2):
                    Day4Part2View()
                case Selection(day: 5, part: 1):
                    Day5Part1View()
                case Selection(day: 5, part: 2):
                    Day5Part2View()
                case Selection(day: 6, part: 1):
                    Day6Part1View()
                case Selection(day: 6, part: 2):
                    Day6Part2View()
                case Selection(day: 7, part: 1):
                    Day7Part1View(title: title(forSelection: selection)!)
                case Selection(day: 7, part: 2):
                    Day7Part2View(title: title(forSelection: selection)!)
                case Selection(day: 8, part: 1):
                    Day8Part1View(title: title(forSelection: selection)!)
                case Selection(day: 8, part: 2):
                    Day8Part2View(title: title(forSelection: selection)!)
                case Selection(day: 9, part: 1):
                    Day9Part1View(title: title(forSelection: selection)!)
                case Selection(day: 9, part: 2):
                    Day9Part2View(title: title(forSelection: selection)!)
                case Selection(day: 10, part: 1):
                    Day10Part1View(title: title(forSelection: selection)!)
                case Selection(day: 10, part: 2):
                    Day10Part2View(path: $path, title: title(forSelection: selection)!)
                case Selection(day: 11, part: 1):
                    Day11Part1View(title: title(forSelection: selection)!)
                case Selection(day: 11, part: 2):
                    Day11Part2View(title: title(forSelection: selection)!)
                case Selection(day: 12, part: 1):
                    Day12Part1View(title: title(forSelection: selection)!)
                case Selection(day: 12, part: 2):
                    Day12Part2View(title: title(forSelection: selection)!)
                case Selection(day: 13, part: 1):
                    Day13Part1View(title: title(forSelection: selection)!)
                case Selection(day: 13, part: 2):
                    Day13Part2View(title: title(forSelection: selection)!)
                case Selection(day: 14, part: 1):
                    Day14Part1View(path: $path, title: title(forSelection: selection)!)
                case Selection(day: 14, part: 2):
                    Day14Part2View(path: $path, title: title(forSelection: selection)!)
                default:
                    Text("Error: unknown quiz item")
                }
            }
            .navigationDestination(for: LargeOutput.self) { largeOutput in
                LargeOutputView(text: largeOutput.text)
            }
            .navigationDestination(for: Visualisation.self) { visualisation in
                VisualisationView(context: visualisation.context, steps: (Int(visualisation.answer) ?? 0) + 1, answer: visualisation.answer)
            }
        }
    }

    func title(forSelection selection: Selection) -> String? {
        let day = selection.day
        let part = selection.part
        return self.quizes
            .filter { quiz in
                quiz.day == day
            }
            .map { quiz in
                "\(quiz.title) Day \(day) Part \(part)"
            }
            .first
    }
}

struct ContentView_Previews: PreviewProvider {
    static var previews: some View {
        ContentView()
    }
}
