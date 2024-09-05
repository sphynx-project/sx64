#include <instructions/hlt.hpp>
#include <core/sx64.hpp>
#include <spdlog/spdlog.h>

namespace sx64
{
    HLTInstruction::HLTInstruction(uint64_t data)
        : Instruction(data)
    {
        decode();
    }

    void HLTInstruction::decode()
    {
        type = InstructionType::HLT;
        decodedString = "HLT";
        spdlog::trace("HLTInstruction decoded: {}", decodedString);
    }

    uint64_t HLTInstruction::getSize() const
    {
        return 1;
    }

    void HLTInstruction::action(CPU &cpu)
    {
        spdlog::trace("HLTInstruction action triggered. Halting CPU.");
        cpu.halt();
    }
}
