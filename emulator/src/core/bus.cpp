#include <core/bus.hpp>
#include <spdlog/spdlog.h>
#include <global.hpp>

Bus::Bus()
{
    spdlog::trace("Bus created");
}

Bus::~Bus()
{
    spdlog::trace("Bus destroyed");
}

void Bus::attachDevice(std::shared_ptr<Device> device)
{
    devices.push_back(device);
    spdlog::debug("Device {} attached to bus", device->getName());
    device->initialize();
    device->enable();
}

void Bus::enable()
{
    for (auto &device : devices)
    {
        device->enable();
        spdlog::debug("Device {} enabled", device->getName());
    }
}

uint64_t Bus::read(uint64_t address)
{
    spdlog::trace("Bus read at address {:#04x}", address);

    for (auto &device : devices)
    {
        if (device->isEnabled())
        {
            uint64_t deviceSize = device->getSize();
            if (address < deviceSize)
            {
                uint64_t data = device->read(address);
                spdlog::debug("Read {:#018x} from device {} (address range: 0 to {:#04x})", data, device->getName(), deviceSize - 1);
                return data;
            }
            address -= deviceSize;
        }
        else
        {
            spdlog::debug("Device {} is disabled (address range: 0 to {:#04x})", device->getName(), device->getSize() - 1);
        }
    }

    spdlog::warn("No device found for read at address {:#04x} after checking all devices", address);
    g_cpu.halt();
    return 0;
}

void Bus::write(uint64_t address, uint64_t data)
{
    spdlog::trace("Bus write at address {:#04x} with data {:#018x}", address, data);

    for (auto &device : devices)
    {
        if (device->isEnabled())
        {
            uint64_t deviceSize = device->getSize();
            if (address < deviceSize)
            {
                device->write(address, data);
                spdlog::debug("Wrote {:#018x} to device {} (address range: 0 to {:#04x})", data, device->getName(), deviceSize - 1);
                return;
            }
            address -= deviceSize;
        }
        else
        {
            spdlog::debug("Device {} is disabled (address range: 0 to {:#04x})", device->getName(), device->getSize() - 1);
        }
    }

    spdlog::warn("No device found for write at address {:#04x} after checking all devices", address);
    g_cpu.halt();
}

const std::vector<std::shared_ptr<Device>> &Bus::getDevices() const
{
    return devices;
}
