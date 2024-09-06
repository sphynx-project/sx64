#pragma once

#include <vector>
#include <cstdint>
#include <memory>
#include <core/bus.hpp>
#include <devices/memory.hpp>

#define SX64_ADDR_SYS_BOOTSTRAP 0x0000

namespace sx64
{
    enum InstructionType
    {
        NOP = 0x0,
        HLT = 0x1,
        WRITE = 0x2,
        READ = 0x3,
        LDI = 0x4
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

        void fetchInstructions();

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
