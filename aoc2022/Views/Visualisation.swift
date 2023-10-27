//
//  Visualisation.swift
//  aoc2022
//
//  Created by Hee Suk Shin on 2023/02/19.
//

import SwiftUI
import SwiftyZeroMQ

struct Visualisation: Hashable {
    var context: SwiftyZeroMQ.Context
    var answer: String
}
