#include <spdlog/spdlog.h>
#include <core/bus.hpp>
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
    spdlog::trace("Device \"{}\" attached to bus", device->getName());
    device->initialize();
    device->enable();
}

void Bus::enable()
{
    for (const auto &device : devices)
    {
        device->enable();
        spdlog::trace("Device \"{}\" enabled", device->getName());
    }
}

uint64_t Bus::read(uint64_t address) const
{
    spdlog::trace("Bus read at address {:#016x}", address);

    for (const auto &device : devices)
    {
        if (device->isEnabled())
        {
            uint64_t deviceSize = device->getSize();
            if (address < deviceSize)
            {
                uint64_t data = device->read(address);
                spdlog::trace("Read {:#x} from device \"{}\"", data, device->getName());
                return data;
            }
            address -= deviceSize;
        }
        else
        {
            spdlog::debug("Device \"{}\" is disabled", device->getName());
        }
    }

    spdlog::warn("No device found for read at address {:#016x}", address);
    g_cpu.halt();
    return 0;
}

void Bus::write(uint64_t address, uint64_t data)
{
    spdlog::trace("Bus write at address {:#016x} with data {:#016x}", address, data);

    for (const auto &device : devices)
    {
        if (device->isEnabled())
        {
            uint64_t deviceSize = device->getSize();
            if (address < deviceSize)
            {
                device->write(address, data);
                spdlog::trace("Wrote {:#x} to device \"{}\"", data, device->getName());
                return;
            }
            address -= deviceSize;
        }
        else
        {
            spdlog::debug("Device \"{}\" is disabled", device->getName());
        }
    }

    spdlog::warn("No device found for write at address {:#016x}", address);
    g_cpu.halt();
}

const std::vector<std::shared_ptr<Device>> &Bus::getDevices() const
{
    return devices;
}
