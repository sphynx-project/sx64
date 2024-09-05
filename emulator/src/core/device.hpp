#pragma once

#include <cstdint>
#include <string>

class Device
{
public:
    Device(const std::string &name = "Unnamed Device");
    virtual ~Device() = default;

    virtual void initialize();
    virtual void reset();
    virtual void update();
    virtual uint64_t read(uint64_t address);
    virtual void write(uint64_t address, uint64_t data);

    virtual uint64_t getSize() const = 0;

    std::string getName() const;
    bool isEnabled() const;
    void enable();
    void disable();

private:
    std::string name;
    bool enabled;
};
