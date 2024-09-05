#pragma once

#include <cstdint>
#include <vector>
#include <memory>
#include <core/device.hpp>

class Bus
{
public:
    Bus();
    ~Bus();

    void attachDevice(std::shared_ptr<Device> device);
    uint64_t read(uint64_t address);
    void write(uint64_t address, uint64_t data);
    
    const std::vector<std::shared_ptr<Device>>& getDevices() const;

private:
    std::vector<std::shared_ptr<Device>> devices;
};
