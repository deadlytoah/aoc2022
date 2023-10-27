//
//  RockPaperScissors.swift
//  aoc2022
//
//  Created by Hee Suk Shin on 2022/12/19.
//

import Foundation

enum RPSError: Error {
    case parsingFailed
}

enum RPS {
    case rock
    case paper
    case scissors

    func score() -> UInt32 {
        switch self {
        case .rock:
            return 1
        case .paper:
            return 2
        case .scissors:
            return 3
        }
    }
    static func fromString(_ s: Substring) throws -> RPS {
        switch s {
        case "A", "X":
            return RPS.rock
        case "B", "Y":
            return RPS.paper
        case "C", "Z":
            return RPS.scissors
        default:
            throw RPSError.parsingFailed
        }
    }
}

enum RPSResultOfStep {
    case win
    case loss
    case tie

    init(_ s: Substring) throws {
        switch s {
        case "X":
            self = .loss
        case "Y":
            self = .tie
        case "Z":
            self = .win
        default:
            throw RPSError.parsingFailed
        }
    }
}

struct RPSStep {
    var theirs: RPS
    var mine: RPS?
    var result: RPSResultOfStep?

    init(_ line: Substring) throws {
        let tokens = line.split(separator: " ", maxSplits: 1)
        let theirs = try RPS.fromString(tokens[0])
        let mine = try RPS.fromString(tokens[1])
        self.theirs = theirs
        self.mine = mine
        self.result = computeResult()
    }

    init(forPartTwo line: Substring) throws {
        let tokens = line.split(separator: " ", maxSplits: 1)
        let theirs = try RPS.fromString(tokens[0])
        let result = try RPSResultOfStep(tokens[1])
        self.theirs = theirs
        self.result = result
        self.mine = deduceMine()
    }

    func deduceMine() -> RPS {
        switch result! {
        case .win:
            switch self.theirs {
            case .rock:
                return .paper
            case .paper:
                return .scissors
            case .scissors:
                return .rock
            }
        case .loss:
            switch self.theirs {
            case .rock:
                return .scissors
            case .paper:
                return .rock
            case .scissors:
                return .paper
            }
        case .tie:
            return theirs
        }
    }

    func computeResult() -> RPSResultOfStep {
        if theirs == mine {
            return .tie
        } else if (theirs == .rock && mine == .scissors) ||
                    (theirs == .paper && mine == .rock) ||
                    (theirs == .scissors && mine == .paper) {
            return .loss
        } else {
            return .win
        }
    }

    func resultScore() -> UInt32 {
        switch result! {
        case .win:
            return 6
        case .tie:
            return 3
        case .loss:
            return 0
        }
    }

    func computeScore() -> UInt32 {
        return mine!.score() + resultScore()
    }
}

struct RPSStrategyGuide {
    var steps: [RPSStep]

    func computeTotalScore() -> UInt32 {
        return steps.map { step in
            step.computeScore()
        }.reduce(0) { s, score in
            s + score
        }
    }
}

func parseStrategyGuideOfRPCTourney(_ input: String) throws -> RPSStrategyGuide {
    var steps: [RPSStep] = []
    let lines = input.split(separator: "\n", omittingEmptySubsequences: false)
    for line in lines {
        steps.append(try RPSStep(line))
    }
    return RPSStrategyGuide(steps: steps)
}

func parseStrategyGuideOfRPCTourney(forPartTwo input: String) throws -> RPSStrategyGuide {
    var steps: [RPSStep] = []
    let lines = input.split(separator: "\n", omittingEmptySubsequences: false)
    for line in lines {
        steps.append(try RPSStep(forPartTwo: line))
    }
    return RPSStrategyGuide(steps: steps)
}
