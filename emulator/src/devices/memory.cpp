#include <spdlog/spdlog.h>
#include <devices/memory.hpp>

MemoryDevice::MemoryDevice(const std::string &name, uint64_t size, bool readOnly)
    : Device(name), memory(size, 0), size(size), readOnly(readOnly)
{
    spdlog::trace("MemoryDevice \"{}\" created with size {} bytes", name, size);
}

MemoryDevice::MemoryDevice(const std::string &name, uint64_t size, bool readOnly, const uint8_t *initBuffer, size_t bufferSize)
    : Device(name), memory(size, 0), size(size), readOnly(readOnly)
{
    spdlog::trace("MemoryDevice \"{}\" created with size {} bytes and initialization buffer", name, size);
    initializeWithBuffer(initBuffer, bufferSize);
}

MemoryDevice::~MemoryDevice()
{
    spdlog::trace("MemoryDevice \"{}\" destroyed", getName());
}

void MemoryDevice::initialize()
{
    std::fill(memory.begin(), memory.end(), 0);
    Device::initialize();
    spdlog::debug("MemoryDevice \"{}\" initialized with zeros", getName());
}

void MemoryDevice::initializeWithBuffer(const uint8_t *initBuffer, size_t bufferSize)
{
    std::fill(memory.begin(), memory.end(), 0);
    if (initBuffer && bufferSize > 0)
    {
        size_t bytesToCopy = std::min(bufferSize, size);
        std::memcpy(memory.data(), initBuffer, bytesToCopy);
        spdlog::debug("MemoryDevice \"{}\" initialized with {} bytes from buffer", getName(), bytesToCopy);
    }
    else
    {
        spdlog::warn("MemoryDevice \"{}\" initialization buffer is null or buffer size is zero", getName());
    }
}

uint64_t MemoryDevice::read(uint64_t address) const
{
    if (address < size)
    {
        uint64_t data = memory[address];
        spdlog::debug("MemoryDevice \"{}\" read at address {:#x} with data {:#x}", getName(), address, data);
        return data;
    }
    spdlog::warn("MemoryDevice \"{}\" read out of bounds at address {:#x}", getName(), address);
    return 0;
}

void MemoryDevice::write(uint64_t address, uint64_t data)
{
    if (!readOnly && address < size)
    {
        memory[address] = static_cast<uint8_t>(data & 0xFF);
        spdlog::debug("MemoryDevice \"{}\" wrote {:#x} at address {:#x}", getName(), data, address);
    }
    else
    {
        spdlog::warn("MemoryDevice \"{}\" write out of bounds or read-only at address {:#x}", getName(), address);
    }
}

uint64_t MemoryDevice::getSize() const
{
    return size;
}
