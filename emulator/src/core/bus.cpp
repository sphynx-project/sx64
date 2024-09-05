#include <core/bus.hpp>
#include <spdlog/spdlog.h>

Bus::Bus()
{
    SPDLOG_TRACE("Bus created");
}

Bus::~Bus()
{
    SPDLOG_TRACE("Bus destroyed");
}

void Bus::attachDevice(std::shared_ptr<Device> device)
{
    devices.push_back(device);
    SPDLOG_DEBUG("Device {} attached to bus", device->getName());
}
uint64_t Bus::read(uint64_t address)
{
    SPDLOG_TRACE("Bus read at address {:#04x}", address);

    for (auto &device : devices)
    {
        if (device->isEnabled())
        {
            uint64_t deviceSize = device->getSize();
            if (address < deviceSize)
            {
                uint64_t data = device->read(address);
                SPDLOG_DEBUG("Read {:#018x} from device {}", data, device->getName());
                return data;
            }
            address -= deviceSize;
        }
    }

    SPDLOG_WARN("No device found for read at address {:#04x}", address);
    return 0;
}

void Bus::write(uint64_t address, uint64_t data)
{
    SPDLOG_TRACE("Bus write at address {:#04x} with data {:#018x}", address, data);

    for (auto &device : devices)
    {
        if (device->isEnabled())
        {
            uint64_t deviceSize = device->getSize();
            if (address < deviceSize)
            {
                device->write(address, data);
                SPDLOG_DEBUG("Wrote {:#018x} to device {}", data, device->getName());
                return;
            }
            address -= deviceSize;
        }
    }

    SPDLOG_WARN("No device found for write at address {:#04x}", address);
}


const std::vector<std::shared_ptr<Device>>& Bus::getDevices() const
{
    return devices;
}
