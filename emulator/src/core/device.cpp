#include <core/device.hpp>

Device::Device(const std::string &name) : name(name), enabled(true) {}

void Device::initialize()
{
    enabled = true;
}

void Device::reset()
{
    enabled = false;
}

void Device::update()
{
}

uint64_t Device::read([[maybe_unused]] uint64_t address)
{
    return 0;
}

void Device::write([[maybe_unused]] uint64_t address, [[maybe_unused]] uint64_t data)
{
}

std::string Device::getName() const
{
    return name;
}

bool Device::isEnabled() const
{
    return enabled;
}

void Device::enable()
{
    enabled = true;
}

void Device::disable()
{
    enabled = false;
}
