#include <core/bus.hpp>

Bus::Bus() {}

Bus::~Bus() {}

void Bus::attachDevice(std::shared_ptr<Device> device)
{
    devices.push_back(device);
}

uint64_t Bus::read(uint64_t address)
{
    for (auto &device : devices)
    {
        if (device->isEnabled())
        {
            return device->read(address);
        }
    }
    return 0;
}

void Bus::write(uint64_t address, uint64_t data)
{
    for (auto &device : devices)
    {
        if (device->isEnabled())
        {
            device->write(address, data);
        }
    }
}
