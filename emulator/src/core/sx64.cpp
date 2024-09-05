#include <core/sx64.hpp>
#include <spdlog/spdlog.h>

namespace sx64
{
    CPU::CPU()
        : r(8, 0), sb(0), sp(0), ip(SX64_ADDR_SYS_BOOTSTRAP), fr(0), bus(std::make_shared<Bus>()), running(false)
    {
    }

    void CPU::run()
    {
        SPDLOG_INFO("sx64: starting...");
        running = true;

        while (running)
        {
            step();
        }

        SPDLOG_INFO("sx64: finished");
    }

    void CPU::fetchInstructions()
    {
        SPDLOG_INFO("Fetching instructions from IP {:#04x}", ip);

        uint8_t opcode = static_cast<uint8_t>(bus->read(ip));
        uint64_t instructionSize = 0;

        switch (opcode)
        {
        case static_cast<uint8_t>(InstructionType::HLT):
            instructionSize = 1;
            break;
        default:
            instructionSize = 8;
            break;
        }

        uint64_t instructionData = 0;
        for (size_t i = 0; i < instructionSize; ++i)
        {
            instructionData |= (bus->read(ip + i) << (i * 8));
        }

        SPDLOG_INFO("Fetched {}-byte instruction {:#016x} from address {:#04x}", instructionSize, instructionData, ip);

        auto instruction = decodeInstruction(instructionData);
        SPDLOG_INFO("Decoded instruction: {}", instruction->getDecodedString());

        instruction->action(*this);
        ip += instruction->getSize();
    }

    std::unique_ptr<Instruction> CPU::decodeInstruction(uint64_t data)
    {
        if (data == static_cast<int>(InstructionType::HLT))
        {
            return std::make_unique<HLTInstruction>(data);
        }

        // Handle other instruction types as needed
        // Example placeholder for an unknown instruction
        return nullptr;
    }

    void CPU::step()
    {
        fetchInstructions();
    }

    void CPU::halt()
    {
        running = false;
    }

    std::shared_ptr<Bus> &CPU::getBus()
    {
        return bus;
    }

    void CPU::dumpState() const
    {
        SPDLOG_INFO("CPU State Dump:");
        SPDLOG_INFO("Registers:");
        for (size_t i = 0; i < r.size(); ++i)
        {
            SPDLOG_INFO("  R{}: {:#018x}", i, r[i]);
        }
        SPDLOG_INFO("SB: {:#018x}", sb);
        SPDLOG_INFO("SP: {:#018x}", sp);
        SPDLOG_INFO("IP: {:#018x}", ip);
        SPDLOG_INFO("FR: {:#06x}", fr);
    }   
}
