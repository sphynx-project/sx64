#include <core/device.hpp>
#include <spdlog/spdlog.h>

Device::Device(const std::string &name, uint64_t baseAddress)
    : name(name), enabled(true), baseAddress(baseAddress)
{
    spdlog::trace("Device \"{}\" created at base address {:#04x}", name, baseAddress);
}

void Device::initialize()
{
    enabled = true;
    spdlog::debug("Device \"{}\" initialized", name);
}

void Device::reset()
{
    enabled = false;
    spdlog::debug("Device \"{}\" reset", name);
}

void Device::update()
{
    spdlog::trace("Device \"{}\" updated", name);
}

uint64_t Device::read([[maybe_unused]] uint64_t address)
{
    spdlog::trace("Device \"{}\" read at address {:#04x}", name, address);
    return 0;
}

void Device::write([[maybe_unused]] uint64_t address, [[maybe_unused]] uint64_t data)
{
    spdlog::trace("Device \"{}\" write at address {:#04x} with data {:#018x}", name, address, data);
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
    spdlog::debug("Device \"{}\" enabled", name);
}

void Device::disable()
{
    enabled = false;
    spdlog::debug("Device \"{}\" disabled", name);
}

std::string Device::getPermissionStr() const
{
    return readOnly ? "r" : "r/w";
}
