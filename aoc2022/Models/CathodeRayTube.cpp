//
//  CathodeRayTube.cpp
//  aoc2022
//
//  Created by Hee Suk Shin on 2023/01/11.
//

#include <iostream>
#include <optional>
#include <sstream>
#include <string>
#include <tuple>
#include <variant>
#include <vector>

#include "CppErrorCode.h"
#include "CathodeRayTube.hpp"

std::variant<CCRTInstruction, CppErrorCode> CCRTInstruction::from_tokens(std::optional<std::string> instruction, std::optional<std::string> argument0) {
    if (!instruction.has_value()) {
        return CppErrorCodeParse;
    } else {
        if (instruction == "noop") {
            if (argument0.has_value()) {
                return CppErrorCodeParse;
            } else {
                return CCRTInstruction { CCRTInstructionKindNoop, std::nullopt };
            }
        } else if (instruction == "addx") {
            if (!argument0.has_value()) {
                return CppErrorCodeParse;
            } else {
                std::istringstream sin { argument0.value() };
                CCRTArgument valueOfArgument0;
                sin >> valueOfArgument0;
                return CCRTInstruction { CCRTInstructionKindAddx, valueOfArgument0 };
            }
        }
    }
    abort();
}

static std::vector<std::tuple<std::optional<std::string>, std::optional<std::string>>> split(std::string const &input) {
    std::istringstream sin { input };
    std::vector<std::tuple<std::optional<std::string>, std::optional<std::string>>> lines;
    std::string line;
    while (std::getline(sin, line)) {
        std::istringstream sin { line };
        std::string instruction;
        sin >> instruction;
        std::string argument0;
        sin >> argument0;
        std::optional<std::string> optionalInstruction;
        std::optional<std::string> optionalArgument0;
        if (instruction == "") {
            optionalInstruction = std::nullopt;
        } else {
            optionalInstruction = instruction;
        }
        if (argument0 == "") {
            optionalArgument0 = std::nullopt;
        } else {
            optionalArgument0 = argument0;
        }
        lines.emplace_back(std::make_tuple(optionalInstruction, optionalArgument0));
    }
    return lines;
}

std::variant<CCRTProgram, CppErrorCode> CCRTParseProgram(std::string const &input) {
    auto lines = split(input);
    std::vector<CCRTInstruction> listing;
    for (auto [instruction, argument0] : lines) {
        auto maybeInstruction = CCRTInstruction::from_tokens(instruction, argument0);
        if (std::holds_alternative<CppErrorCode>(maybeInstruction)) {
            return std::get<CppErrorCode>(maybeInstruction);
        } else {
            listing.emplace_back(std::get<CCRTInstruction>(maybeInstruction));
        }
    }
    return CCRTProgram { listing };
}

size_t CCRTProgram::size() const {
    return this->listing.size();
}

CCRTInstruction const & CCRTProgram::operator [](size_t counter) const {
    return this->listing[counter];
}

std::vector<bool>::reference CCRTBitmap::operator[](size_t index) {
   return this->value[index];
}

std::string CCRTBitmap::description() const {
    std::ostringstream buffer;
    for (auto pixel = this->value.begin(); pixel != this->value.end(); pixel++) {
        if (*pixel) {
            buffer << CCRTBitmap::ON_CHARACTER;
        } else {
            buffer << CCRTBitmap::OFF_CHARACTER;
        }
        auto index = 1 + std::distance(this->value.begin(), pixel);
        if (index % CCRTBitmap::ROW_WIDTH == 0) {
            buffer << std::endl;
        }
    }
    return std::move(buffer).str();
}

CCRTRange CCRTSprite::range(size_t max) const {
    std::vector<size_t> r;
    ssize_t begin, end;
    begin = this->location - CCRTSprite::WING;
    end = this->location + CCRTSprite::WING;
    return CCRTRange { begin, end };
}

void CCRTSprite::move(size_t location) {
    this->location = location;
}

void CCRTSprite::drawOn(CCRTDisplay &display) const {
    auto range = this->range(display.bitmap.size());
    if (range.includes(display.cycle % CCRTBitmap::ROW_WIDTH)) {
        display.bitmap[display.cycle] = true;
    }
}

CCRTDisplay &CCRTDisplay::moveSprite(size_t location) {
    this->sprite.move(location);
    return *this;
}

void CCRTDisplay::draw() {
    this->sprite.drawOn(*this);
}

CCRTMachineState::CCRTMachineState(CCRTMachine const &from) {
    std::copy(from.display.bitmap.value.begin(), from.display.bitmap.value.end(), this->bitmap.value.begin());
    std::copy_n(from.registers, CCRTRegisterCount, this->registers);
}

CCRTRegisterValue CCRTMachineState::readFromRegister(CCRTRegister which) const {
    return this->registers[which];
}

CCRTBitmap CCRTMachineState::takeBitmap() {
    return std::move(this->bitmap);
}

void CCRTMachine::start(CCRTProgram program) {
    this->program = std::move(program);
    this->counter = 0;
    this->clock = 1;
    for (int i = 0; i < CCRTRegisterCount; i++) {
        this->registers[i] = 1;
    }
    if (!this->fetchInstruction()) {
        this->stop();
    }
}

void CCRTMachine::stop() {
    this->program = std::nullopt;
}

std::variant<CCRTMachineState, CppErrorCode> CCRTMachine::waitForClock(CCRTClockValue clock) {
    if (this->clock > clock) {
        return CppErrorCodeState;
    } else {
        while (this->clock < clock) {
            auto maybeError = this->tick();
            if (maybeError.has_value()) {
                return maybeError.value();
            }
        }
        return this->snapshot();
    }
}

CCRTInstructionStatus CCRTMachine::tickInstruction() {
    --this->countdownForInstruction;
    if (this->countdownForInstruction <= 0) {
        return CCRTInstructionStatusComplete;
    } else {
        return CCRTInstructionStatusNotYet;
    }
}

bool CCRTMachine::fetchNextInstruction() {
    ++this->counter;
    return this->fetchInstruction();
}

bool CCRTMachine::fetchInstruction() {
    auto const & program = this->program.value();
    if (this->counter < program.size()) {
        auto const & instruction = program[this->counter];
        switch (instruction.kind) {
            case CCRTInstructionKindAddx:
                this->countdownForInstruction = 2;
                break;

            case CCRTInstructionKindNoop:
                this->countdownForInstruction = 1;
                break;
        }
        return true;
    } else {
        return false;
    }
}

std::optional<CppErrorCode> CCRTMachine::tick() {
    if (this->program.has_value()) {
        this->display
            .updateCycle((this->clock - 1) % CCRTBitmap::SIZE)
            .moveSprite(this->registers[CCRTRegisterX])
            .draw();
        ++this->clock;
        switch (this->tickInstruction()) {
            case CCRTInstructionStatusComplete:
                this->apply();
                if (!this->fetchNextInstruction()) {
                    this->stop();
                }
                break;

            case CCRTInstructionStatusNotYet:
                break;
        }
    } else {
        return CppErrorCodeState;
    }
    return std::nullopt;
}

void CCRTMachine::apply() {
    auto const & program = this->program.value();
    auto const & instruction = program[counter];
    switch (instruction.kind) {
        case CCRTInstructionKindAddx:
            this->registers[CCRTRegisterX] += instruction.argument0.value();
            break;

        case CCRTInstructionKindNoop:
            break;
    }
}

CCRTMachineState CCRTMachine::snapshot() const {
    CCRTMachineState state(*this);
    return state;
}
