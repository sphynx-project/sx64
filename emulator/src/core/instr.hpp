#pragma once

#include <cstdint>
#include <string>
#include <memory>

namespace sx64
{
    class CPU;

    enum class InstructionType
    {
        NOP = 0x00, 
        HLT = 0x01,
        UNKNOWN
    };

    class Instruction
    {
    protected:
        uint64_t rawData;
        InstructionType type;
        std::string decodedString;

    public:
        Instruction(uint64_t data);
        virtual ~Instruction() = default;

        virtual void decode() = 0;
        virtual uint64_t getSize() const = 0;
        virtual InstructionType getType() const;
        virtual std::string getDecodedString() const;
        uint64_t getRawData() const;
        virtual void action(CPU& cpu) = 0;
    };
}
