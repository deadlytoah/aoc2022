//
//  VisualisationView.swift
//  aoc2022
//
//  Created by Hee Suk Shin on 2023/02/19.
//

import SwiftUI
import SwiftyZeroMQ

struct VisualisationView: View {
    @State var context: SwiftyZeroMQ.Context
    @State var step: Double = 0
    @State var steps: Int
    @State var text: String = ""
    @State var answer: String

    static let Timeout: TimeInterval = 200.0

    var body: some View {
        ScrollView(.vertical) {
            ScrollView(.horizontal) {
                TextEditor(text: $text)
                    .frame(width: 4000, height: 4000)
                    .fontDesign(.monospaced)
                    .padding()
            }
        }
        .navigationTitle("Visualisation")
        .toolbar {
            ToolbarItemGroup {
                Text("Answer:")
                TextField("Answer", text: $answer)
                Text("Step to Visualise:")
                TextField("Step", value: $step, formatter: NumberFormatter(), onCommit: { updateVisualisation() })
                if steps > 1 {
                    Slider(value: $step, in: 0...Double(steps - 1), step: Double.Stride(steps / 20), onEditingChanged: { editing in
                        if !editing {
                            updateVisualisation()
                        }
                    })
                    .frame(width: 150)
                    Button("Previous") {
                        if self.step > 0 {
                            self.step = self.step - 1
                            self.updateVisualisation()
                        }
                    }
                    Button("Next") {
                        if Int(self.step) < self.steps - 1 {
                            self.step = self.step + 1
                            self.updateVisualisation()
                        }
                    }
                }
            }
        }
        .onAppear(perform: updateVisualisation)
        .onDisappear {
            do {
                let socket = try context.socket(SwiftyZeroMQ.SocketType.request)
                try socket.connect("tcp://localhost:22143")
                try stopServiceOfVisualisation(socket, VisualisationView.Timeout)
            } catch {
                NSLog("There was an error stopping the service")
            }
        }
    }

    func stopServiceOfVisualisation(_ socket: SwiftyZeroMQ.Socket, _ timeout: TimeInterval) throws {
        try socket.send(string: "STOP")

        let poller = SwiftyZeroMQ.Poller()
        try poller.register(socket: socket, flags: .pollIn)

        // Wait for reply message
        let updates = try poller.poll(timeout: timeout)
        if updates[socket] == SwiftyZeroMQ.PollFlags.pollIn {
            let _ = try socket.recv()!
        } else {
            throw QuizError.timeout
        }
    }

    func fetchVisualisation(_ socket: SwiftyZeroMQ.Socket, _ step: Int, _ timeout: TimeInterval) throws -> String {
        // Create multipart request message
        var request = Array<Data>()
        request.append("GET".data(using: String.Encoding.utf8)!)
        request.append(String(step).data(using: String.Encoding.utf8)!)

        // Send request message
        try socket.sendMultipart(parts: request)

        let poller = SwiftyZeroMQ.Poller()
        try poller.register(socket: socket, flags: .pollIn)

        // Wait for reply message
        let updates = try poller.poll(timeout: timeout)
        if updates[socket] == SwiftyZeroMQ.PollFlags.pollIn {
            let response = try socket
                .recvMultipart()
                .map { data in
                    String(data: data, encoding: .utf8)!
                }

            // Check if reply message is an error
            if response[0] == "ERROR" {
                throw QuizError.logic("Visualisation fetch failed for step \(step): ERROR")
            } else if response[0] == "OK" {
                return response[1]
            } else {
                throw QuizError.logic("Unexpected reply message: \(response[0])")
            }
        } else {
            throw QuizError.timeout
        }
    }

    func updateVisualisation() {
        // Update the visualisation based on the current step
        let step = Int(self.step)
        if step >= 0 && step < Int(self.steps) {
            do {
                let socket = try context.socket(SwiftyZeroMQ.SocketType.request)
                try socket.connect("tcp://localhost:22143")
                self.text = try self.fetchVisualisation(socket, step, VisualisationView.Timeout)
            } catch {
                self.text = "There was an error fetching visualisation data."
            }
        }
    }
}
