#pragma once

#include <vector>
#include <cstdint>
#include <memory>
#include <core/bus.hpp>
#include <devices/memory.hpp>
#include <core/instr.hpp>

#define SX64_ADDR_SYS_BOOTSTRAP 0x0000

namespace sx64
{
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
        std::unique_ptr<Instruction> decodeInstruction(uint64_t data);

    public:
        CPU();
        void run();
        void step();
        void halt();

        std::shared_ptr<Bus> &getBus();
        void dumpState() const;
    };
}
