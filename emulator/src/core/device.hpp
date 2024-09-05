#pragma once

#include <cstdint>
#include <string>
#include <memory>

class Device
{
public:
    Device(const std::string &name = "Unnamed Device", bool readOnly = false);
    virtual ~Device() = default;

    virtual void initialize();
    virtual void reset();
    virtual void update();
    virtual uint64_t read(uint64_t address) const;
    virtual void write(uint64_t address, uint64_t data);

    virtual uint64_t getSize() const = 0;

    std::string getName() const;
    std::string getPermissionStr() const;
    bool isEnabled() const;
    void enable();
    void disable();

protected:
    std::string name;
    bool enabled;

private:
    bool readOnly;
};
