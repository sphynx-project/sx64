#include <core/instr.hpp>
#include <spdlog/spdlog.h>

namespace sx64
{
    Instruction::Instruction(uint64_t data)
        : rawData(data), type(InstructionType::UNKNOWN)
    {
        SPDLOG_TRACE("Instruction created with data {:#016x}", data);
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
