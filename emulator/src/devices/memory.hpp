#pragma once

#include <core/device.hpp>
#include <vector>
#include <cstdint>
#include <string>

class MemoryDevice : public Device
{
public:
    MemoryDevice(const std::string &name, uint64_t size, bool readOnly);
    MemoryDevice(const std::string &name, uint64_t size, bool readOnly, const uint8_t *initBuffer, size_t bufferSize);
    ~MemoryDevice() override;

    void initialize() override;
    void initializeWithBuffer(const uint8_t *initBuffer = nullptr, size_t bufferSize = 0);

    uint64_t read(uint64_t address) const override;
    void write(uint64_t address, uint64_t data) override;

    uint64_t getSize() const override;

private:
    std::vector<uint8_t> memory;
    uint64_t size;
    bool readOnly;
};
