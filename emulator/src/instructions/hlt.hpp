#pragma once

#include <core/instr.hpp>

namespace sx64
{
    class HLTInstruction : public Instruction
    {
    public:
        HLTInstruction(uint64_t data);
        void decode() override;
        uint64_t getSize() const override;
        void action(CPU& cpu) override;
    };
}
