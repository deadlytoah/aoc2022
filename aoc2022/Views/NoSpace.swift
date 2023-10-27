//
//  NoSpace.swift
//  aoc2022
//
//  Created by Hee Suk Shin on 2022/12/29.
//

import Foundation

enum NoSpaceCommand {
    case cd(Substring)
    case ls

    static func parse(_ line: Substring) throws -> Self {
        let s = line.split(separator: " ", maxSplits: 1)
        let head = s.first
        let rest = s.last
        switch head {
        case "cd":
            if let rest = rest {
                return .cd(rest)
            } else {
                throw QuizError.parse
            }
        case "ls":
            return .ls
        default:
            throw QuizError.parse
        }
    }
}

enum NoSpaceLineInTerm {
    case command(NoSpaceCommand)
    case dir(Substring)
    case file(Substring, UInt64)

    static func parse(_ line: Substring) throws -> Self {
        let s = line.split(separator: " ", maxSplits: 1)
        let head = s.first!
        let rest = s.last!
        switch head {
        case "$":
            return .command(try NoSpaceCommand.parse(rest))
        case "dir":
            return .dir(rest)
        default:
            if let size = UInt64(head) {
                return .file(rest, size)
            } else {
                throw QuizError.parse
            }
        }
    }
}

enum NoSpaceObjectInner {
    case dir(String, UInt64?, [NoSpaceObject])
    case file(String, UInt64)

    func getName() -> String {
        switch self {
        case .dir(let name, _, _):
            return name
        case .file(let name, _):
            return name
        }
    }

    func getSize() -> UInt64? {
        switch self {
        case .dir(_, let size, _):
            return size
        case .file(_, let size):
            return size
        }
    }

    func getDir(_ name: Substring) -> NoSpaceObject? {
        if case .dir(_, _, let children) = self {
            return children.first { child in child.getName() == name }
        } else {
            return nil
        }
    }

    func directories() throws -> [NoSpaceObject] {
        if case .dir(_, _, let children) = self {
            return children.filter { child in child.isDir() }
        } else {
            throw QuizError.logic("object is not a directory")
        }
    }

    func size() throws -> UInt64 {
        switch self {
        case .dir(_, let size, _):
            if let size = size {
                return size
            } else {
                throw QuizError.logic("size is not known yet")
            }
        case .file(_, let size):
            return size
        }
    }

    func isDir() -> Bool {
        if case .dir(_, _, _) = self {
            return true
        } else {
            return false
        }
    }

    func isFile() -> Bool {
        if case .file(_, _) = self {
            return true
        } else {
            return false
        }
    }
}

class NoSpaceObject {
    var inner: NoSpaceObjectInner

    init(_ inner: NoSpaceObjectInner) {
        self.inner = inner
    }

    func getName() -> String {
        self.inner.getName()
    }

    func getSize() -> UInt64? {
        self.inner.getSize()
    }

    func getDir(_ name: Substring) -> NoSpaceObject? {
        self.inner.getDir(name)
    }

    func directories() throws -> [NoSpaceObject] {
        try self.inner.directories()
    }

    func allDirectories() throws -> [NoSpaceObject] {
        var directories = try self.directories()
        for subdir in directories {
            directories.append(contentsOf: try subdir.allDirectories())
        }
        return directories
    }

    func size() throws -> UInt64 {
        try self.inner.size()
    }

    func isDir() -> Bool {
        self.inner.isDir()
    }

    func isFile() -> Bool {
        self.inner.isFile()
    }

    func calculateSize() {
        switch self.inner {
        case .dir(let name, var size, let children):
            if let _ = size {
                // skip calculation as we already know the size of this item.
            } else {
                for i in 0..<children.count {
                    children[i].calculateSize()
                }

                // child.getSize() would never return a non-nil value as
                // the for loop above would have done the calculations.
                size = children
                    .map { child in child.getSize() }
                    .reduce(0) { sum, sz in sum + sz! }

                self.inner = .dir(name, size, children)
            }
        case .file(_, _):
            break
        }
    }

    private func addChild(_ child: NoSpaceObject) throws {
        if case .dir(let name, let size, var children) = self.inner {
            children.append(child)
            self.inner = .dir(name, size, children)
        } else {
            throw QuizError.logic("object is not a directory")
        }
    }

    func existsDir(_ name: Substring) throws {
        if case .dir(_, _, let children) = self.inner {
            if children.allSatisfy({ obj in !obj.isDir() || obj.getName() != name }) {
                try self.addChild(.dir(String(name), nil, []))
            } else {
                // OK, we have seen the directory already.
            }
        } else {
            throw QuizError.logic("object is not a directory")
        }
    }

    func existsFile(_ name: Substring, _ size: UInt64) throws {
        if case .dir(_, _, let children) = self.inner {
            if children.allSatisfy({ obj in !obj.isFile() || obj.getName() != name }) {
                try self.addChild(.file(String(name), size))
            } else {
                // OK, we have seen the directory already.
            }
        } else {
            throw QuizError.logic("object is not a directory")
        }
    }

    static func dir(_ name: String, _ size: UInt64?, _ children: [NoSpaceObject]) -> NoSpaceObject {
        NoSpaceObject(.dir(name, size, children))
    }

    static func file(_ name: String, _ size: UInt64) -> NoSpaceObject {
        NoSpaceObject(.file(name, size))
    }

    static func fromLinesInTerm(_ lines: [NoSpaceLineInTerm]) throws -> NoSpaceObject {
        var stack: [NoSpaceObject] = [NoSpaceObject.dir("/", nil, [])]
        for line in lines {
            switch line {
            case .command(let command):
                switch command {
                case .cd(let dest):
                    if stack.last!.isDir() {
                        if dest == "/" {
                            stack.removeSubrange(1...)
                        } else if dest == ".." {
                            if stack.count > 1 {
                                let _ = stack.popLast()
                            } else {
                                // we are at root
                            }
                        } else {
                            if let dir = stack.last?.getDir(dest) {
                                stack.append(dir)
                            } else {
                                // expected the directory to exist.
                                throw QuizError.input("object is not a directory")
                            }
                        }
                    } else {
                        // only expecting directories in stack
                        throw QuizError.logic("object is not a directory")
                    }
                case .ls:
                    // no-op at the moment
                    break
                }
            case .dir(let dir):
                if let this = stack.last {
                    try this.existsDir(dir)
                } else {
                    // expect stack to contain at least one item, the root.
                    throw QuizError.logic("unexpected empty stack")
                }
            case let .file(name, size):
                if let this = stack.last {
                    try this.existsFile(name, size)
                } else {
                    // expect stack to contain at least one item, the root.
                    throw QuizError.logic("unexpected empty stack")
                }
            }
        }
        return stack.first!
    }
}

func noSpaceParseInput(_ input: String) throws -> [NoSpaceLineInTerm] {
    try input
        .split(separator: "\n")
        .map { line in
            try NoSpaceLineInTerm.parse(line)
        }
}
