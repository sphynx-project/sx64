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
    spdlog::trace("Device \"{}\" attached to bus at base address {:#016x}", device->getName(), device->getBaseAddress());
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

uint8_t Bus::read(uint64_t address) const
{
    spdlog::trace("Bus read at address {:#016x}", address);

    for (const auto &device : devices)
    {
        uint64_t deviceBaseAddress = device->getBaseAddress();
        uint64_t deviceSize = device->getSize();
        if (address >= deviceBaseAddress && address < deviceBaseAddress + deviceSize)
        {
            if (device->isEnabled())
            {
                uint64_t offset = address - deviceBaseAddress;
                uint64_t data = device->read(offset);
                spdlog::trace("Read {:#x} from device \"{}\"", data, device->getName());
                return data;
            }
            else
            {
                spdlog::debug("Device \"{}\" is disabled", device->getName());
            }
        }
    }

    spdlog::warn("No device found for read at address {:#016x}", address);
    g_cpu.halt();
    return 0;
}

void Bus::write(uint64_t address, uint8_t data)
{
    spdlog::trace("Bus write at address {:#016x} with data {:#016x}", address, data);

    for (const auto &device : devices)
    {
        uint64_t deviceBaseAddress = device->getBaseAddress();
        uint64_t deviceSize = device->getSize();
        if (address >= deviceBaseAddress && address < deviceBaseAddress + deviceSize)
        {
            if (device->isEnabled())
            {
                uint64_t offset = address - deviceBaseAddress;
                device->write(offset, data);
                spdlog::trace("Wrote {:#x} to device \"{}\"", data, device->getName());
                return;
            }
            else
            {
                spdlog::debug("Device \"{}\" is disabled", device->getName());
            }
        }
    }

    spdlog::warn("No device found for write at address {:#016x}", address);
    g_cpu.halt();
}

const std::vector<std::shared_ptr<Device>> &Bus::getDevices() const
{
    return devices;
}
