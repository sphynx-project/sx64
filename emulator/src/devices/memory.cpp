#include <devices/memory.hpp>
#include <spdlog/spdlog.h>
#include <cstring>

MemoryDevice::MemoryDevice(const std::string &name, uint64_t size, bool readOnly)
    : Device(name), memory(size, 0), readOnly(readOnly), size(size)
{
    SPDLOG_TRACE("MemoryDevice \"{}\" created with size {} bytes", name, size);
}

MemoryDevice::MemoryDevice(const std::string &name, uint64_t size, bool readOnly, const uint8_t *initBuffer, size_t bufferSize)
    : Device(name), memory(size, 0), readOnly(readOnly), size(size)
{
    SPDLOG_TRACE("MemoryDevice \"{}\" created with size {} bytes and initialization buffer", name, size);

    initializeWithBuffer(initBuffer, bufferSize);
}

MemoryDevice::~MemoryDevice()
{
    SPDLOG_TRACE("MemoryDevice \"{}\" destroyed", getName());
}

void MemoryDevice::initialize()
{
    std::fill(memory.begin(), memory.end(), 0);
    Device::initialize();
    SPDLOG_DEBUG("MemoryDevice \"{}\" initialized with zeros", getName());
}

void MemoryDevice::initializeWithBuffer(const uint8_t *initBuffer, size_t bufferSize)
{
    std::fill(memory.begin(), memory.end(), 0);

    size_t bytesToCopy = std::min(bufferSize, size);
    if (initBuffer != nullptr)
    {
        std::memcpy(memory.data(), initBuffer, bytesToCopy);
        SPDLOG_DEBUG("MemoryDevice \"{}\" initialized with {} bytes from buffer", getName(), bytesToCopy);
    }
    else
    {
        SPDLOG_WARN("MemoryDevice \"{}\" initialization buffer is null", getName());
    }
}

uint64_t MemoryDevice::read(uint64_t address)
{
    if (address < size)
    {
        uint64_t data = memory[address];
        SPDLOG_DEBUG("MemoryDevice \"{}\" read at address {:#04x} with data {:#018x}", getName(), address, data);
        return data;
    }

    SPDLOG_WARN("MemoryDevice \"{}\" read out of bounds at address {:#04x}", getName(), address);
    return 0;
}

void MemoryDevice::write(uint64_t address, uint64_t data)
{
    if (!readOnly && address < size)
    {
        memory[address] = static_cast<uint8_t>(data & 0xFF);
        SPDLOG_DEBUG("MemoryDevice \"{}\" wrote {:#018x} at address {:#04x}", getName(), data, address);
    }
    else
    {
        SPDLOG_WARN("MemoryDevice \"{}\" write out of bounds or read-only at address {:#04x}", getName(), address);
    }
}

uint64_t MemoryDevice::getSize() const
{
    return size;
}