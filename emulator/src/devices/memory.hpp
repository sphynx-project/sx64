#pragma once

#include <cstdint>
#include <vector>
#include <core/device.hpp>

class MemoryDevice : public Device
{
public:
    MemoryDevice(const std::string &name, uint64_t size, bool readOnly, uint64_t baseAddress = 0);

    void initialize() override;
    void reset() override;
    void update() override;
    uint64_t read(uint64_t address) const override;
    void write(uint64_t address, uint64_t data) override;
    uint64_t getSize() const override;
    void initializeWithBuffer(const uint8_t *buffer, uint64_t size);

private:
    std::vector<uint64_t> memory;
    uint64_t size;
};
