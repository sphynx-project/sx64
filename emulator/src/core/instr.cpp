#include <core/instr.hpp>

namespace sx64
{
    Instruction::Instruction(uint64_t data)
        : rawData(data), type(InstructionType::UNKNOWN)
    {
    }

    InstructionType Instruction::getType() const
    {
        return type;
    }

    std::string Instruction::getDecodedString() const
    {
        return decodedString;
    }

    uint64_t Instruction::getRawData() const
    {
        return rawData;
    }

    uint64_t Instruction::getSize() const
    {
        return 0;
    }
}
