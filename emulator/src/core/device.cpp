#include <spdlog/spdlog.h>
#include <core/device.hpp>

Device::Device(const std::string &name, uint64_t baseAddress)
    : name(name), enabled(true), baseAddress(baseAddress), readOnly(false)
{
    spdlog::trace("Device \"{}\" created at base address {:#x}", name, baseAddress);
}

void Device::initialize()
{
    enabled = true;
    spdlog::trace("Device \"{}\" initialized", name);
}

void Device::reset()
{
    enabled = false;
    spdlog::trace("Device \"{}\" reset", name);
}

void Device::update()
{
    spdlog::trace("Device \"{}\" updated", name);
}

uint64_t Device::read(uint64_t address) const
{
    spdlog::trace("Device \"{}\" read at address {:#x}", name, address);
    return 0;
}

void Device::write(uint64_t address, uint64_t data)
{
    spdlog::trace("Device \"{}\" write at address {:#x} with data {:#x}", name, address, data);
}

std::string Device::getName() const
{
    return name;
}

std::string Device::getPermissionStr() const
{
    return readOnly ? "r" : "r/w";
}

bool Device::isEnabled() const
{
    return enabled;
}

void Device::enable()
{
    enabled = true;
    spdlog::trace("Device \"{}\" enabled", name);
}

void Device::disable()
{
    enabled = false;
    spdlog::trace("Device \"{}\" disabled", name);
}
