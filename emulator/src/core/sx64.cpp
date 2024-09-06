#include <global.hpp>
#include <core/sx64.hpp>
#include <spdlog/spdlog.h>
#include <stdexcept>
#include <chrono>
#include <thread>
#include <memory>

namespace sx64
{
    CPU::CPU()
        : r(8, 0), sb(0), sp(0), ip(SX64_ADDR_SYS_BOOTSTRAP), fr(0), bus(std::make_shared<Bus>()), running(false), lastStepTime(std::chrono::steady_clock::now())
    {
        spdlog::trace("CPU initialized with IP: {:#016x}", ip);
    }

    void CPU::setFlag(Flag flag)
    {
        fr |= flag;
    }

    void CPU::clearFlag(Flag flag)
    {
        fr &= ~flag;
    }

    bool CPU::isFlagSet(Flag flag) const
    {
        return (fr & flag) != 0;
    }

    void CPU::fetchInstructions()
    {
        spdlog::trace("Fetching instructions from IP {:#016x}", ip);

        uint8_t opcode = static_cast<uint8_t>(bus->read(ip));
        ip += 1;

        switch (opcode)
        {
        case static_cast<uint8_t>(InstructionType::NOP):
            spdlog::debug("NOP @ {:#016x}", ip);
            break;

        case static_cast<uint8_t>(InstructionType::HLT):
            spdlog::debug("HLT @ {:#016x}", ip);
            halt();
            break;

        case static_cast<uint8_t>(InstructionType::WRITE):
        {
            uint8_t regIn = static_cast<uint8_t>(bus->read(ip));
            ip += 1;

            uint64_t address = 0;
            for (size_t i = 0; i < 8; ++i)
            {
                address |= static_cast<uint64_t>(bus->read(ip + i)) << (i * 8);
            }
            ip += 8;

            uint64_t valueToWrite = getRegister(regIn);
            spdlog::debug("WRITE @ {:#016x}, Register R{} = {:#018x}", address, regIn, valueToWrite);
            bus->write(address, static_cast<uint8_t>(valueToWrite));

            break;
        }

        case static_cast<uint8_t>(InstructionType::READ):
        {
            uint8_t regOut = static_cast<uint8_t>(bus->read(ip));
            ip += 1;

            uint64_t address = 0;
            for (size_t i = 0; i < 8; ++i)
            {
                address |= static_cast<uint64_t>(bus->read(ip + i)) << (i * 8);
            }
            ip += 8;

            uint8_t valueRead = bus->read(address);
            setRegister(regOut, valueRead);
            spdlog::debug("READ @ {:#016x}, Register R{} = {:#018x}", address, regOut, valueRead);

            break;
        }

        case static_cast<uint8_t>(InstructionType::LDI):
        {
            uint8_t regOut = static_cast<uint8_t>(bus->read(ip));
            ip += 1;

            uint64_t immediateValue = 0;
            for (size_t i = 0; i < 8; ++i)
            {
                immediateValue |= static_cast<uint64_t>(bus->read(ip + i)) << (i * 8);
            }
            ip += 8;

            setRegister(regOut, immediateValue);
            spdlog::debug("LDI @ {:#016x}, Register R{} = {:#018x}", ip, regOut, immediateValue);

            break;
        }

        case static_cast<uint8_t>(InstructionType::ADD):
        {
            uint8_t dest = static_cast<uint8_t>(bus->read(ip));
            ip += 1;
            uint8_t src = static_cast<uint8_t>(bus->read(ip));
            ip += 1;
            uint64_t result = r[dest] + r[src];
            r[dest] = result;

            if (result == 0)
            {
                setFlag(ZERO);
            }
            else
            {
                clearFlag(ZERO);
            }

            if (static_cast<int64_t>(result) < 0)
            {
                setFlag(NEGATIVE);
            }
            else
            {
                clearFlag(NEGATIVE);
            }

            spdlog::debug("ADD R{} += R{} -> {:#018x}", dest, src, r[dest]);
            break;
        }

        case static_cast<uint8_t>(InstructionType::SUB):
        {
            uint8_t dest = static_cast<uint8_t>(bus->read(ip));
            ip += 1;
            uint8_t src = static_cast<uint8_t>(bus->read(ip));
            ip += 1;
            uint64_t result = r[dest] - r[src];
            r[dest] = result;

            if (result == 0)
            {
                setFlag(ZERO);
            }
            else
            {
                clearFlag(ZERO);
            }

            if (static_cast<int64_t>(result) < 0)
            {
                setFlag(NEGATIVE);
            }
            else
            {
                clearFlag(NEGATIVE);
            }

            spdlog::debug("SUB R{} -= R{} -> {:#018x}", dest, src, r[dest]);
            break;
        }

        case static_cast<uint8_t>(InstructionType::MUL):
        {
            uint8_t dest = static_cast<uint8_t>(bus->read(ip));
            ip += 1;
            uint8_t src = static_cast<uint8_t>(bus->read(ip));
            ip += 1;
            uint64_t result = r[dest] * r[src];
            r[dest] = result;

            if (result == 0)
            {
                setFlag(ZERO);
            }
            else
            {
                clearFlag(ZERO);
            }

            if (static_cast<int64_t>(result) < 0)
            {
                setFlag(NEGATIVE);
            }
            else
            {
                clearFlag(NEGATIVE);
            }

            spdlog::debug("MUL R{} *= R{} -> {:#018x}", dest, src, r[dest]);
            break;
        }

        case static_cast<uint8_t>(InstructionType::DIV):
        {
            uint8_t dest = static_cast<uint8_t>(bus->read(ip));
            ip += 1;
            uint8_t src = static_cast<uint8_t>(bus->read(ip));
            ip += 1;

            if (r[src] != 0)
            {
                uint64_t result = r[dest] / r[src];
                r[dest] = result;

                if (result == 0)
                {
                    setFlag(ZERO);
                }
                else
                {
                    clearFlag(ZERO);
                }

                if (static_cast<int64_t>(result) < 0)
                {
                    setFlag(NEGATIVE);
                }
                else
                {
                    clearFlag(NEGATIVE);
                }

                spdlog::debug("DIV R{} /= R{} -> {:#018x}", dest, src, r[dest]);
            }
            else
            {
                spdlog::critical("Division by zero @ {:#016x}", ip);
                halt();
            }
            break;
        }

        case static_cast<uint8_t>(InstructionType::PUSH):
        {
            uint8_t reg = static_cast<uint8_t>(bus->read(ip));
            ip += 1;
            sp -= 8;
            bus->write(sp, static_cast<uint8_t>(r[reg]));
            spdlog::debug("PUSH R{} -> Stack @ {:#018x}", reg, sp);
            break;
        }

        case static_cast<uint8_t>(InstructionType::POP):
        {
            uint8_t reg = static_cast<uint8_t>(bus->read(ip));
            ip += 1;
            r[reg] = bus->read(sp);
            sp += 8;
            spdlog::debug("POP Stack -> R{} = {:#018x}", reg, r[reg]);
            break;
        }

        case static_cast<uint8_t>(InstructionType::JMP):
        {
            uint64_t address = 0;
            for (size_t i = 0; i < 8; ++i)
            {
                address |= static_cast<uint64_t>(bus->read(ip + i)) << (i * 8);
            }
            ip = address;
            spdlog::debug("JMP -> {:#018x}", ip);
            break;
        }

        case static_cast<uint8_t>(InstructionType::CMP):
        {
            uint8_t reg1 = static_cast<uint8_t>(bus->read(ip));
            ip += 1;
            uint8_t reg2 = static_cast<uint8_t>(bus->read(ip));
            ip += 1;
            if (r[reg1] == r[reg2])
            {
                setFlag(ZERO);
            }
            else
            {
                clearFlag(ZERO);
            }

            if (static_cast<int64_t>(r[reg1] - r[reg2]) < 0)
            {
                setFlag(NEGATIVE);
            }
            else
            {
                clearFlag(NEGATIVE);
            }

            spdlog::debug("CMP R{} == R{} -> FR = {}", reg1, reg2, fr);
            break;
        }

        case static_cast<uint8_t>(InstructionType::JE):
        {
            uint64_t address = 0;
            for (size_t i = 0; i < 8; ++i)
            {
                address |= static_cast<uint64_t>(bus->read(ip + i)) << (i * 8);
            }
            ip += 8;
            if (isFlagSet(ZERO))
            {
                ip = address;
                spdlog::debug("JE -> {:#018x}", ip);
            }
            break;
        }

        case static_cast<uint8_t>(InstructionType::JNE):
        {
            uint64_t address = 0;
            for (size_t i = 0; i < 8; ++i)
            {
                address |= static_cast<uint64_t>(bus->read(ip + i)) << (i * 8);
            }
            ip += 8;
            if (!isFlagSet(ZERO))
            {
                ip = address;
                spdlog::debug("JNE -> {:#018x}", ip);
            }
            break;
        }

        default:
            spdlog::critical("Unknown instruction at IP {:#016x} ({:#04x})", ip, opcode);
            halt();
            break;
        }
    }

    void CPU::step()
    {
        spdlog::trace("CPU stepping. Current IP: {:#016x}", ip);
        fetchInstructions();
    }

    void CPU::run()
    {
        using namespace std::chrono;

        running = true;
        constexpr int64_t targetTimePerCycle = 1000000 / 1000000; // 1 MHz clock speed

        while (running)
        {
            auto start = steady_clock::now();
            step();
            auto end = steady_clock::now();

            auto stepDuration = duration_cast<microseconds>(end - start).count();
            int64_t sleepDuration = targetTimePerCycle - stepDuration;

            if (sleepDuration > 0)
            {
                std::this_thread::sleep_for(microseconds(sleepDuration));
            }

            spdlog::trace("Step completed in {} microseconds. Sleeping for {} microseconds to maintain 1 MHz.", stepDuration, sleepDuration > 0 ? sleepDuration : 0);
        }
    }

    void CPU::halt()
    {
        spdlog::debug("CPU halt requested");
        running = false;
    }

    std::shared_ptr<Bus> &CPU::getBus()
    {
        return bus;
    }

    void CPU::setRegister(size_t index, uint64_t value)
    {
        if (index >= r.size())
        {
            throw std::out_of_range("Register index out of range");
        }
        r[index] = value;
    }

    uint64_t CPU::getRegister(size_t index) const
    {
        if (index >= r.size())
        {
            throw std::out_of_range("Register index out of range");
        }
        return r[index];
    }

    void CPU::dumpState() const
    {
        spdlog::debug("CPU State Dump:");
        spdlog::debug("Registers:");
        for (size_t i = 0; i < r.size(); ++i)
        {
            spdlog::debug("  R{}: {:#018x} ({})", i, r[i], r[i]);
        }
        spdlog::debug("SB: {:#018x} ({})", sb, sb);
        spdlog::debug("SP: {:#018x} ({})", sp, sp);
        spdlog::debug("IP: {:#018x} ({})", ip, ip);
        spdlog::debug("FR: {:#06x} ({})", fr, fr);

        spdlog::debug("Memory Layout:");

        for (const auto &device : bus->getDevices())
        {
            uint64_t baseAddress = device->getBaseAddress();
            uint64_t size = device->getSize();
            uint64_t endAddress = baseAddress + size - 1;

            std::string sizeStr;

            if (size >= 1024 * 1024 * 1024)
                sizeStr = fmt::format("{:.2f} GB", size / static_cast<double>(1024 * 1024 * 1024));
            else if (size >= 1024 * 1024)
                sizeStr = fmt::format("{:.2f} MB", size / static_cast<double>(1024 * 1024));
            else if (size >= 1024)
                sizeStr = fmt::format("{:.2f} KB", size / static_cast<double>(1024));
            else
                sizeStr = fmt::format("{} B", size);

            spdlog::debug(" - {:#018x} -> {:#018x} ({}) {} : {}",
                          baseAddress,
                          endAddress,
                          sizeStr,
                          device->getPermissionStr(),
                          device->getName());
        }
    }
}
