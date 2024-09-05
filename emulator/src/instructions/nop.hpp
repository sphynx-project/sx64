#pragma once

#include <core/instr.hpp>

namespace sx64
{
    class NOPInstruction : public Instruction
    {
    public:
        NOPInstruction(uint64_t data);
        void decode() override;
        uint64_t getSize() const override;
        void action(CPU& cpu) override;
    };
}
