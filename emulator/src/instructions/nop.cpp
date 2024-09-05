#include <instructions/nop.hpp>
#include <core/sx64.hpp>
#include <spdlog/spdlog.h>

namespace sx64
{
    NOPInstruction::NOPInstruction(uint64_t data)
        : Instruction(data)
    {
        decode();
    }

    void NOPInstruction::decode()
    {
        type = InstructionType::NOP;
        decodedString = "NOP";
        spdlog::trace("NOPInstruction decoded: {}", decodedString);
    }

    uint64_t NOPInstruction::getSize() const
    {
        return 1;
    }

    void NOPInstruction::action([[maybe_unused]] CPU &cpu)
    {
        spdlog::trace("NOPInstruction action triggered. Doing nothing.");
    }
}
