#pragma once

#include <cstdint>
#include <vector>
#include <core/device.hpp>

class MemoryDevice : public Device
{
public:
    MemoryDevice(const std::string &name, uint64_t size, bool readOnly);
    ~MemoryDevice();

    void initialize() override;
    uint64_t read(uint64_t address) override;
    void write(uint64_t address, uint64_t data) override;

private:
    std::vector<uint8_t> memory;
    bool readOnly;
    uint64_t size;
};
