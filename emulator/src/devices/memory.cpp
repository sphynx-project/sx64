#include <devices/memory.hpp>

MemoryDevice::MemoryDevice(const std::string &name, uint64_t size, bool readOnly)
    : Device(name), memory(size, 0), readOnly(readOnly), size(size) {}

MemoryDevice::~MemoryDevice() {}

void MemoryDevice::initialize()
{
    std::fill(memory.begin(), memory.end(), 0);
    Device::initialize();
}

uint64_t MemoryDevice::read(uint64_t address)
{
    if (address < size)
    {
        return memory[address];
    }
    return 0;
}

void MemoryDevice::write(uint64_t address, uint64_t data)
{
    if (!readOnly && address < size)
    {
        memory[address] = static_cast<uint8_t>(data & 0xFF);
    }
}
