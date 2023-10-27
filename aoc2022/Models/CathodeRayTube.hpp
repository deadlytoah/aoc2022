//
//  CathodeRayTube.hpp
//  aoc2022
//
//  Created by Hee Suk Shin on 2023/01/11.
//

#ifndef CathodeRayTube_hpp
#define CathodeRayTube_hpp

#include <optional>
#include <variant>
#include <vector>
#include "CppErrorCode.h"
#include "CCRTRegister.h"

enum CCRTInstructionKind {
    CCRTInstructionKindNoop,
    CCRTInstructionKindAddx,
};

typedef int32_t CCRTArgument;
typedef int32_t CCRTClockValue;

enum CCRTInstructionStatus {
    CCRTInstructionStatusNotYet,
    CCRTInstructionStatusComplete,
};

struct CCRTInstruction {
    CCRTInstructionKind kind;
    std::optional<CCRTArgument> argument0;

    static std::variant<CCRTInstruction, CppErrorCode> from_tokens(std::optional<std::string> instruction, std::optional<std::string> argument0);
};

struct CCRTProgram {
    std::vector<CCRTInstruction> listing;
    size_t size() const;
    CCRTInstruction const & operator [] (size_t counter) const;
};

struct CCRTRange {
    ssize_t begin, end;

    bool includes(ssize_t offset) {
        return offset >= begin && offset <= end;
    }
};

struct CCRTDisplay;
struct CCRTSprite {
    const size_t WING = 1;
    size_t location;
    void move(size_t location);
    void drawOn(CCRTDisplay &display) const;

private:
    CCRTRange range(size_t max) const;
};

struct CCRTBitmap {
    static const size_t SIZE = 240;
    static const size_t ROW_WIDTH = 40;
    const char ON_CHARACTER = '#';
    const char OFF_CHARACTER = '.';
    std::vector<bool> value;

    CCRTBitmap(): value(SIZE, false) {}
    CCRTBitmap(CCRTBitmap const &other) {
        this->value = std::vector(other.value.begin(), other.value.end());
    }
    CCRTBitmap(CCRTBitmap &&other) {
        this->value = std::move(other.value);
    }

    void operator =(CCRTBitmap &&other) {
        this->value = std::move(other.value);
    }

    std::vector<bool>::reference operator [](size_t index);

    size_t size() const {
        return this->value.size();
    }

    std::string description() const;
};

struct CCRTDisplay {
    size_t cycle;
    CCRTSprite sprite;
    CCRTBitmap bitmap;

    CCRTDisplay &updateCycle(size_t cycle) {
        this->cycle = cycle;
        return *this;
    }
    CCRTDisplay &moveSprite(size_t location);
    void draw();
};

struct CCRTMachine;
struct CCRTMachineState {
    CCRTRegisterValue registers[CCRTRegisterCount];
    CCRTBitmap bitmap;
    CCRTRegisterValue readFromRegister(CCRTRegister which) const;
    CCRTBitmap takeBitmap();

    CCRTMachineState(CCRTMachine const &from);
    CCRTMachineState() {}
};

struct CCRTMachine {
    std::optional<CCRTProgram> program;
    size_t counter;
    size_t clock;
    ssize_t countdownForInstruction;
    CCRTRegisterValue registers[CCRTRegisterCount];

    CCRTDisplay display;

    void start(CCRTProgram program);
    void stop();
    std::variant<CCRTMachineState, CppErrorCode> waitForClock(CCRTClockValue clock);
    CCRTMachineState snapshot() const;

private:
    CCRTInstructionStatus tickInstruction();
    bool fetchNextInstruction();
    bool fetchInstruction();
    std::optional<CppErrorCode> tick();
    void apply();
};

std::variant<CCRTProgram, CppErrorCode> CCRTParseProgram(std::string const &input);

#endif /* CathodeRayTube_hpp */
