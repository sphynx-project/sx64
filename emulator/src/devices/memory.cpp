#include <devices/memory.hpp>
#include <spdlog/spdlog.h>
#include <cstring>
#include <vector>

MemoryDevice::MemoryDevice(const std::string &name, uint64_t size, bool readOnly, uint64_t baseAddress)
    : Device(name, readOnly, baseAddress), memory(size, 0), size(size)
{
    spdlog::trace("MemoryDevice \"{}\" created with size {:#x} bytes", name, size);
}

void MemoryDevice::initialize()
{
    Device::initialize();
    std::fill(memory.begin(), memory.end(), 0);
}

void MemoryDevice::reset()
{
    Device::reset();
    std::fill(memory.begin(), memory.end(), 0);
}

void MemoryDevice::update()
{
    Device::update();
}

uint8_t MemoryDevice::read(uint64_t address) const
{
    if (address < size)
    {
        return memory[address];
    }
    spdlog::error("Read out of bounds in MemoryDevice \"{}\"", getName());
    return 0;
}

void MemoryDevice::write(uint64_t address, uint8_t data)
{
    if (!isReadOnly() && address < size)
    {
        memory[address] = data;
    }
    else
    {
        spdlog::error("Write out of bounds or to read-only memory in MemoryDevice \"{}\"", getName());
    }
}

uint64_t MemoryDevice::getSize() const
{
    return size;
}

void MemoryDevice::initializeWithBuffer(const uint8_t *initBuffer, size_t bufferSize)
{
    std::fill(memory.begin(), memory.end(), 0);
    if (initBuffer && bufferSize > 0)
    {
        size_t bytesToCopy = std::min(bufferSize, size);
        std::memcpy(memory.data(), initBuffer, bytesToCopy);
        spdlog::trace("MemoryDevice \"{}\" initialized with {} bytes from buffer", getName(), bytesToCopy);
    }
    else
    {
        spdlog::warn("MemoryDevice \"{}\" initialization buffer is null or buffer size is zero", getName());
    }
}
