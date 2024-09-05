#include <core/device.hpp>
#include <spdlog/spdlog.h>

Device::Device(const std::string &name) : name(name), enabled(true)
{
    SPDLOG_TRACE("Device \"{}\" created", name);
}

void Device::initialize()
{
    enabled = true;
    SPDLOG_DEBUG("Device \"{}\" initialized", name);
}

void Device::reset()
{
    enabled = false;
    SPDLOG_DEBUG("Device \"{}\" reset", name);
}

void Device::update()
{
    SPDLOG_TRACE("Device \"{}\" updated", name);
}

uint64_t Device::read([[maybe_unused]] uint64_t address)
{
    SPDLOG_TRACE("Device \"{}\" read at address {:#04x}", name, address);
    return 0;
}

void Device::write([[maybe_unused]] uint64_t address, [[maybe_unused]] uint64_t data)
{
    SPDLOG_TRACE("Device \"{}\" write at address {:#04x} with data {:#018x}", name, address, data);
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
    SPDLOG_DEBUG("Device \"{}\" enabled", name);
}

void Device::disable()
{
    enabled = false;
    SPDLOG_DEBUG("Device \"{}\" disabled", name);
}
