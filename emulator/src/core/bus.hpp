#pragma once

#include <cstdint>
#include <vector>
#include <memory>
#include "device.hpp"

class Bus
{
public:
    Bus();
    ~Bus();

    void attachDevice(std::shared_ptr<Device> device);
    uint8_t read(uint64_t address) const;
    void write(uint64_t address, uint8_t data);
    const std::vector<std::shared_ptr<Device>> &getDevices() const;
    void enable();

private:
    std::vector<std::shared_ptr<Device>> devices;
};
