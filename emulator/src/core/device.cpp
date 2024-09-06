#include <spdlog/spdlog.h>
#include <core/device.hpp>

Device::Device(const std::string &name, bool readOnly, uint64_t baseAddress)
    : name(name), enabled(true), baseAddress(baseAddress), readOnly(readOnly)
{
    spdlog::trace("Device \"{}\" created (Permissions {}, Base Address {:#016x})", name, getPermissionStr(), baseAddress);
}

void Device::initialize()
{
    spdlog::trace("Device \"{}\" initialized", name);
}

void Device::reset()
{
    spdlog::trace("Device \"{}\" reset", name);
}

void Device::update()
{
    spdlog::trace("Device \"{}\" updated", name);
}

uint64_t Device::read([[maybe_unused]] uint64_t address) const
{
    spdlog::error("Read operation not supported on device \"{}\"", name);
    return 0;
}

void Device::write([[maybe_unused]] uint64_t address, [[maybe_unused]] uint64_t data)
{
    if (readOnly)
    {
        spdlog::error("Write operation not supported on read-only device \"{}\"", name);
    }
    else
    {
        spdlog::error("Write operation not supported on device \"{}\"", name);
    }
}

std::string Device::getName() const
{
    return name;
}

std::string Device::getPermissionStr() const
{
    return readOnly ? "Read-Only" : "Read-Write";
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

uint64_t Device::getBaseAddress() const
{
    return baseAddress;
}

bool Device::isReadOnly() const
{
    return readOnly;
}