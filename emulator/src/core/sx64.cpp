#include <global.hpp>
#include <core/sx64.hpp>
#include <spdlog/spdlog.h>
#include <stdexcept>

namespace sx64
{
    CPU::CPU()
        : r(8, 0), sb(0), sp(0), ip(SX64_ADDR_SYS_BOOTSTRAP), fr(0), bus(std::make_shared<Bus>()), running(false)
    {
        spdlog::trace("CPU initialized with IP: {:#016x}", ip);
    }

    void CPU::run()
    {
        spdlog::info("sx64: Starting CPU execution...");
        bus->enable();
        running = true;

        auto targetCycleDuration = std::chrono::microseconds(1);
        auto cycleStart = std::chrono::high_resolution_clock::now();

        uint64_t cycleCount = 0;
        uint64_t totalElapsedTime = 0;

        while (running)
        {
            auto cycleBegin = std::chrono::high_resolution_clock::now();

            spdlog::trace("Cycle {} Start: IP = {:#018x}, SP = {:#018x}, SB = {:#018x}, FR = {:#06x}",
                          cycleCount + 1, ip, sp, sb, fr);

            step();

            auto cycleEnd = std::chrono::high_resolution_clock::now();
            auto elapsedTime = std::chrono::duration_cast<std::chrono::microseconds>(cycleEnd - cycleBegin);

            totalElapsedTime += elapsedTime.count();
            cycleCount++;

            auto cycleDuration = std::chrono::high_resolution_clock::now() - cycleStart;
            auto remainingTime = targetCycleDuration - cycleDuration;
            cycleStart = std::chrono::high_resolution_clock::now();

            if (remainingTime > std::chrono::microseconds(0))
            {
                std::this_thread::sleep_for(remainingTime);
            }
            else
            {
                spdlog::trace("Running behind schedule; no sleep.");
                cycleStart = std::chrono::high_resolution_clock::now();
            }

            spdlog::trace("Cycle {} End: Cycle duration: {} µs, Target duration: {} µs",
                          cycleCount, elapsedTime.count(), targetCycleDuration.count());

            if (cycleCount % 1000 == 0)
            {
                double averageCycleDuration = static_cast<double>(totalElapsedTime) / cycleCount;
                double performance = (targetCycleDuration.count() / averageCycleDuration) * 100.0;
                spdlog::debug("CPU Performance: {:.2f}%", performance);
            }
            spdlog::trace("-------------------------");
        }

        if (cycleCount > 0)
        {
            double averageCycleDuration = static_cast<double>(totalElapsedTime) / cycleCount;
            double performance = (targetCycleDuration.count() / averageCycleDuration) * 100.0;
            spdlog::info("sx64: CPU execution finished");
            spdlog::info("Final CPU Performance: {:.2f}% ({} cycles)", performance, cycleCount);
        }
        else
        {
            spdlog::debug("sx64: CPU execution finished with no cycles run.");
        }
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
            spdlog::debug("  R{}: {:#018x}", i, r[i]);
        }
        spdlog::debug("SB: {:#018x}", sb);
        spdlog::debug("SP: {:#018x}", sp);
        spdlog::debug("IP: {:#018x}", ip);
        spdlog::debug("FR: {:#06x}", fr);

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
