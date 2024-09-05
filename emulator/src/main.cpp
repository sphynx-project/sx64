#include <iostream>
#include <core/sx64.hpp>
#include <spdlog/spdlog.h>

int main()
{
    spdlog::set_level(spdlog::level::debug);
    spdlog::set_pattern("[%H:%M:%S - %@] %^%-6l%$ | %v");
    sx64::CPU cpu;
    cpu.run();
    return 0;
}