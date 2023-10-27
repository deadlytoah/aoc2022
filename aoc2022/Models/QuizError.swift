//
//  QuizError.swift
//  aoc2022
//
//  Created by Hee Suk Shin on 2022/12/30.
//

import Foundation

enum QuizError: Error {
    /// Parsing succeeded, but there was a problem in the input.
    case input(String)

    /// There was a programming error.
    case logic(String)

    case memory

    case unexpectedNil

    /// Parsing failed.
    case parse

    case message

    case timeout

    var description: String {
        get {
            switch self {
            case .input(let s):
                return "Input error: \(s)"
            case .logic(let s):
                return "Logic error: \(s)"
            case .memory:
                return "Out of memory"
            case .unexpectedNil:
                return "Unexpected nil"
            case .parse:
                return "Parse error"
            case .message:
                return "Message error"
            case .timeout:
                return "Timed out"
            }
        }
    }
}
