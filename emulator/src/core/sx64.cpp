#include <core/sx64.hpp>
#include <spdlog/spdlog.h>
#include <global.hpp>

// Instructions
#include <instructions/hlt.hpp>
#include <instructions/nop.hpp>

namespace sx64
{
    CPU::CPU()
        : r(8, 0), sb(0), sp(0), ip(SX64_ADDR_SYS_BOOTSTRAP), fr(0), bus(std::make_shared<Bus>()), running(false)
    {
        spdlog::trace("CPU initialized with IP: {:#04x}", ip);
    }

    void CPU::run()
    {
        spdlog::info("sx64: Starting CPU execution...");
        bus->enable();
        running = true;

        while (running)
        {
            step();
        }

        spdlog::info("sx64: CPU execution finished");
    }

    void CPU::fetchInstructions()
    {
        spdlog::trace("Fetching instructions from IP {:#04x}", ip);

        uint8_t opcode = static_cast<uint8_t>(bus->read(ip));
        uint64_t instructionSize = 0;

        switch (opcode)
        {
        case static_cast<uint8_t>(InstructionType::NOP):
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
            if (ip + i >= bus->getDevices().back()->getSize())
            {
                spdlog::error("Instruction read out of bounds at IP {:#04x}", ip + i);
                halt();
                return;
            }
            instructionData |= (bus->read(ip + i) << (i * 8));
        }

        spdlog::trace("Fetched {}-byte instruction {:#016x} from address {:#04x}", instructionSize, instructionData, ip);

        auto instruction = decodeInstruction(instructionData);
        if (instruction)
        {
            spdlog::debug("Decoded instruction: {}", instruction->getDecodedString());
            instruction->action(*this);
            ip += instruction->getSize();
        }
        else
        {
            spdlog::critical("Unknown instruction at IP {:#016x} ({:#04x})", ip, instructionData);
            halt();
        }
    }

    std::unique_ptr<Instruction> CPU::decodeInstruction(uint64_t data)
    {
        spdlog::trace("Decoding instruction data: {:#016x}", data);

        if (data == static_cast<int>(InstructionType::HLT))
        {
            return std::make_unique<HLTInstruction>(data);
        }
        else if (data == static_cast<int>(InstructionType::NOP))
        {
            return std::make_unique<NOPInstruction>(data);
        }

        return nullptr;
    }

    void CPU::step()
    {
        spdlog::trace("CPU stepping. Current IP: {:#04x}", ip);
        fetchInstructions();
    }

    void CPU::halt()
    {
        spdlog::info("CPU halt requested");
        running = false;
    }

    std::shared_ptr<Bus> &CPU::getBus()
    {
        return bus;
    }

    void CPU::dumpState() const
    {
        spdlog::info("CPU State Dump:");
        spdlog::info("Registers:");
        for (size_t i = 0; i < r.size(); ++i)
        {
            spdlog::info("  R{}: {:#018x}", i, r[i]);
        }
        spdlog::info("SB: {:#018x}", sb);
        spdlog::info("SP: {:#018x}", sp);
        spdlog::info("IP: {:#018x}", ip);
        spdlog::info("FR: {:#06x}", fr);

        spdlog::info("Memory Layout:");

        uint64_t startAddress = 0;
        for (const auto &device : bus->getDevices())
        {
            uint64_t size = device->getSize();
            uint64_t endAddress = startAddress + size - 1;

            if (size == 0)
            {
                endAddress = startAddress;
            }

            std::string sizeStr;

            if (size >= 1024 * 1024 * 1024)
                sizeStr = fmt::format("{:.2f} GB", size / static_cast<double>(1024 * 1024 * 1024));
            else if (size >= 1024 * 1024)
                sizeStr = fmt::format("{:.2f} MB", size / static_cast<double>(1024 * 1024));
            else if (size >= 1024)
                sizeStr = fmt::format("{:.2f} KB", size / static_cast<double>(1024));
            else
                sizeStr = fmt::format("{} B", size);

            spdlog::info(" - {:#018x} -> {:#018x} ({:<8}) {:<3}: {}",
                         startAddress,
                         endAddress,
                         sizeStr,
                         device->getPermissionStr(),
                         device->getName());

            startAddress = endAddress;
        }
    }

}