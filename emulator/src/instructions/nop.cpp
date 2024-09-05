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
        SPDLOG_TRACE("NOPInstruction decoded: {}", decodedString);
    }

    uint64_t NOPInstruction::getSize() const
    {
        return 1;
    }

    void NOPInstruction::action(CPU& cpu)
    {
        SPDLOG_TRACE("NOPInstruction action triggered. Doing nothing.");
    }
}
