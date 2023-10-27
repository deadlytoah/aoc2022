//
//  SupplyStacks.swift
//  aoc2022
//
//  Created by Hee Suk Shin on 2022/12/25.
//

import Foundation

enum SupplyStacksToken {
    case space
    case openingSquareBracket
    case closingSquareBracket
    case letter(Character)
    case digit(UInt8)
    case newLine
}

enum SupplyStacksInstructionsToken {
    case move
    case from
    case to
    case number(UInt32)
    case newLine
}

enum SupplyStacksSymbol {
    case empty
    case cargo(Character)
    case num(UInt32)

    func isCargo() -> Bool {
        switch self {
        case .cargo(_):
            return true
        default:
            return false
        }
    }

    func toCharacter() -> Character {
        switch self {
        case .cargo(let ch):
            return ch
        case .num(let n):
            return String(n).first!
        case .empty:
            return " "
        }
    }
}

enum SupplyStacksError: Error {
    case input
    case state
}

extension Collection where Element == [SupplyStacksSymbol] {
    func transposed() -> [[SupplyStacksSymbol]] {
        var stacks: [[SupplyStacksSymbol]] = []
        (0..<self.first!.count).forEach { _ in stacks.append([]) }
        for line in self {
            for (i, symbol) in line.enumerated() {
                stacks[i].append(symbol)
            }
        }
        return stacks
    }
}

struct SupplyStack {
    var stack: [Character]

    init(_ stack: [Character]) {
        self.stack = stack
    }

    func take(_ howMany: UInt32) -> (Self, [Character]) {
        (SupplyStack(Array(self.stack.prefix(self.stack.count - Int(howMany)))),
         self.stack.suffix(Int(howMany)))
    }

    func append(_ cargos: [Character]) -> Self {
        SupplyStack(self.stack + cargos)
    }

    func top() -> Character {
        return stack.last!
    }
}

struct SupplyStacks {
    var stacks: [SupplyStack] = []

    init(_ stacks: [SupplyStack]) {
        self.stacks = stacks
    }

    func take(from: UInt32, howMany: UInt32) -> (Self, [Character]) {
        var cargos: [Character] = []
        let stacks = self.stacks
            .enumerated()
            .map { (i, stack) in
                if i == from - 1 {
                    let (stack, c) = stack.take(howMany)
                    cargos = c
                    return stack
                } else {
                    return stack
                }
            }
        return (SupplyStacks(stacks), cargos)
    }

    func put(on: UInt32, cargos: [Character]) -> Self {
        SupplyStacks(self.stacks
            .enumerated()
            .map { (i, stack) in
                if i == on - 1 {
                    return stack.append(cargos)
                } else {
                    return stack
                }
            })
    }

    static func tokenise(_ input: Substring) throws -> [SupplyStacksToken] {
        return try input
            .map { ch in
                switch ch {
                case " ":
                    return .space
                case "[":
                    return .openingSquareBracket
                case "]":
                    return .closingSquareBracket
                case "\n":
                    return .newLine
                case "0"..."9":
                    return .digit(UInt8(String(ch))!)
                case "a"..."z", "A"..."Z":
                    return .letter(ch)
                default:
                    throw SupplyStacksError.input
                }
            }
    }

    static func parseToSymbols(_ tokens: [SupplyStacksToken]) throws -> [[SupplyStacksSymbol]] {
        enum SupplyStacksState {
            case ready
            case next
            case emptyOrNumber
            case empty
            case number(UInt32)
            case cargo(UInt32, Character?)
        }

        var state: SupplyStacksState = .ready
        var line: [SupplyStacksSymbol] = []
        var coll: [[SupplyStacksSymbol]] = []
        for token in tokens {
            switch token {
            case .space:
                switch state {
                case .next:
                    state = .ready
                case .ready:
                    state = .emptyOrNumber
                case .emptyOrNumber:
                    state = .empty
                case .empty:
                    line.append(.empty)
                    state = .next
                case .number(let d):
                    line.append(.num(d))
                    state = .next
                default:
                    throw SupplyStacksError.state
                }
            case .openingSquareBracket:
                switch state {
                case .ready:
                    state = .cargo(1, nil)
                default:
                    throw SupplyStacksError.state
                }
            case .closingSquareBracket:
                switch state {
                case .cargo(2, let ch):
                    line.append(.cargo(ch!))
                    state = .next
                default:
                    throw SupplyStacksError.state
                }
            case .newLine:
                switch state {
                case .next:
                    coll.append(line)
                    line = []
                    state = .ready
                default:
                    throw SupplyStacksError.state
                }
            case .digit(let d):
                switch state {
                case .emptyOrNumber:
                    state = .number(UInt32(d))
                default:
                    throw SupplyStacksError.state
                }
            case .letter(let ch):
                switch state {
                case .cargo(1, _):
                    state = .cargo(2, ch)
                default:
                    throw SupplyStacksError.state
                }
            }
        }
        if !line.isEmpty {
            coll.append(line)
        }
        return coll
    }

    static func parse(_ input: String) throws -> (supplyStacks: SupplyStacks, rest: Substring) {
        let input = input.split(separator: "\n\n", maxSplits: 1)
        let rest = input[1]
        let supplyStacks = SupplyStacks(
            try parseToSymbols(try tokenise(input[0]))
                .reversed()
                .transposed()
                .map { stack in
                    SupplyStack(stack
                        .filter { symbol in symbol.isCargo() }
                        .map { symbol in symbol.toCharacter() })
                })
        return (supplyStacks, rest)
    }
}

struct InstructionForSupplyStacks {
    var move: UInt32
    var from: UInt32
    var to: UInt32

    init(_ move: UInt32, _ from: UInt32, _ to: UInt32) {
        self.move = move
        self.from = from
        self.to = to
    }

    func execute(_ supplyStacks: SupplyStacks) -> SupplyStacks {
        let (supplyStacks, cargos) = supplyStacks.take(from: from, howMany: move)
        return supplyStacks.put(on: to, cargos: cargos.reversed())
    }

    func execute(forPartTwo supplyStacks: SupplyStacks) -> SupplyStacks {
        let (supplyStacks, cargos) = supplyStacks.take(from: from, howMany: move)
        return supplyStacks.put(on: to, cargos: cargos)
    }
}

func tokeniseInstructionsForSupplyStacks(_ input: Substring) throws -> [SupplyStacksInstructionsToken] {
    enum TokeniseState {
        case ready
        case next
        case m
        case mo
        case mov
        case move
        case f
        case fr
        case fro
        case from
        case t
        case to
        case number(String)
    }

    var tokens: [SupplyStacksInstructionsToken] = []
    var state: TokeniseState = .ready
    for ch in input {
        switch ch {
        case " ":
            switch state {
            case .move:
                tokens.append(.move)
                state = .next
            case .from:
                tokens.append(.from)
                state = .next
            case .to:
                tokens.append(.to)
                state = .next
            case .number(let s):
                tokens.append(.number(UInt32(s)!))
                state = .next
            default:
                throw SupplyStacksError.state
            }
        case "\n":
            switch state {
            case .number(let s):
                tokens.append(.number(UInt32(s)!))
                tokens.append(.newLine)
                state = .ready
            default:
                throw SupplyStacksError.state
            }
        case "m":
            switch state {
            case .ready:
                state = .m
            case .fro:
                state = .from
            default:
                throw SupplyStacksError.state
            }
        case "o":
            switch state {
            case .m:
                state = .mo
            case .fr:
                state = .fro
            case .t:
                state = .to
            default:
                throw SupplyStacksError.state
            }
        case "v":
            switch state {
            case .mo:
                state = .mov
            default:
                throw SupplyStacksError.state
            }
        case "e":
            switch state {
            case .mov:
                state = .move
            default:
                throw SupplyStacksError.state
            }
        case "f":
            switch state {
            case .next:
                state = .f
            default:
                throw SupplyStacksError.state
            }
        case "r":
            switch state {
            case .f:
                state = .fr
            default:
                throw SupplyStacksError.state
            }
        case "t":
            switch state {
            case .next:
                state = .t
            default:
                throw SupplyStacksError.state
            }
        case "0"..."9":
            switch state {
            case .number(let s):
                let t = s + String(ch)
                state = .number(t)
            case .next:
                state = .number(String(ch))
            default:
                throw SupplyStacksError.state
            }
        default:
            throw SupplyStacksError.state
        }
    }

    // handle the case where the final new line is missing
    switch state {
    case .number(let s):
        tokens.append(.number(UInt32(s)!))
    case .ready:
        ()
    default:
        throw SupplyStacksError.state
    }
    return tokens
}

func parseInstructionsForSupplyStacks(_ input: Substring) throws -> [InstructionForSupplyStacks] {
    enum ParseState {
        case ready
        case move(UInt32, UInt32?)
        case from(UInt32, UInt32, UInt32?)
        case to(UInt32, UInt32, UInt32, UInt32?)
    }

    var instructions: [InstructionForSupplyStacks] = []
    let tokens = try tokeniseInstructionsForSupplyStacks(input)
    var state: ParseState = .ready
    for token in tokens {
        switch token {
        case .move:
            switch state {
            case .ready:
                state = .move(1, nil)
            default:
                throw SupplyStacksError.state
            }
        case .number(let n):
            switch state {
            case .move(1, nil):
                state = .move(2, n)
            case .from(1, let m, nil):
                state = .from(2, m, n)
            case .to(1, let l, let m, nil):
                state = .to(2, l, m, n)
                instructions.append(InstructionForSupplyStacks(l, m, n))
            default:
                throw SupplyStacksError.state
            }
        case .from:
            switch state {
            case .move(2, let n):
                state = .from(1, n!, nil)
            default:
                throw SupplyStacksError.state
            }
        case .to:
            switch state {
            case .from(2, let n, let m):
                state = .to(1, n, m!, nil)
            default:
                throw SupplyStacksError.state
            }
        case .newLine:
            switch state {
            case .to(2, _, _, _):
                state = .ready
            default:
                throw SupplyStacksError.state
            }
        }
    }
    return instructions
}
