#include <instructions/hlt.hpp>
#include <core/sx64.hpp>

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
    }

    uint64_t HLTInstruction::getSize() const
    {
        return 1;
    }

    void HLTInstruction::action(CPU& cpu)
    {
        cpu.halt();
    }
}
