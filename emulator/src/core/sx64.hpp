#pragma once

#include <vector>
#include <cstdint>
#include <memory>
#include <core/bus.hpp>
#include <devices/memory.hpp>
#include <chrono>

#define SX64_ADDR_SYS_BOOTSTRAP 0x0000

namespace sx64
{
    enum InstructionType
    {
        NOP = 0x00,
        HLT = 0x01,
        WRITE = 0x02,
        READ = 0x03,
        LDI = 0x04,
        ADD = 0x05,
        SUB = 0x06,
        MUL = 0x07,
        DIV = 0x08,
        PUSH = 0x09,
        POP = 0x0A,
        JMP = 0x0B,
        CMP = 0x0C,
        JE = 0x0D,
        JNE = 0x0E
    };

    enum Flag
    {
        ZERO = 0x01,     // Zero flag
        NEGATIVE = 0x02, // Negative flag
        CARRY = 0x04,    // Carry flag
        OVERFLOW = 0x08  // Overflow flag
    };

    class CPU
    {
    private:
        std::vector<uint64_t> r; // General purpose registers R0-R7
        uint64_t sb;             // Stack Base
        uint64_t sp;             // Stack Pointer
        uint64_t ip;             // Instruction Pointer
        uint16_t fr;             // Flags Register
        std::shared_ptr<Bus> bus;
        bool running;
        std::chrono::steady_clock::time_point lastStepTime;

        void fetchInstructions();
        void setFlag(Flag flag);
        void clearFlag(Flag flag);
        bool isFlagSet(Flag flag) const;

    public:
        CPU();
        void run();
        void step();
        void halt();

        std::shared_ptr<Bus> &getBus();
        void setRegister(size_t index, uint64_t value);
        uint64_t getRegister(size_t index) const;
        void dumpState() const;
    };
}
