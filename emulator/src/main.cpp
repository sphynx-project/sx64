#include <iostream>
#include <core/sx64.hpp>
#include <spdlog/spdlog.h>
#include <spdlog/sinks/basic_file_sink.h>
#include <spdlog/sinks/stdout_color_sinks.h>

int main()
{
    auto file_logger = std::make_shared<spdlog::sinks::basic_file_sink_mt>("logs/sx64.log", true);
    auto console_logger = std::make_shared<spdlog::sinks::stdout_color_sink_mt>();

    auto logger = std::make_shared<spdlog::logger>("sx64_logger", spdlog::sinks_init_list{ file_logger, console_logger });
    logger->set_pattern("[%Y-%m-%d %H:%M:%S.%e] [%n] [%^%l%$] %v");
    logger->set_level(spdlog::level::debug);
    spdlog::set_default_logger(logger);

    spdlog::info("Starting sx64 CPU simulation...");

    sx64::CPU cpu;

    auto sys_bootstrap_mem = std::make_shared<MemoryDevice>("sys-bootstrap", 0x1000, true);
    spdlog::debug("Reserving 4KB of memory for sys-bootstrap");
    cpu.getBus()->attachDevice(sys_bootstrap_mem);

    auto krnl_bootstrap_mem = std::make_shared<MemoryDevice>("RAM", 0x4000, false);
    spdlog::debug("Reserving 16KB of memory for krnl-bootstrap");
    cpu.getBus()->attachDevice(krnl_bootstrap_mem);

    cpu.run();

    spdlog::info("CPU simulation finished.");

    cpu.dumpState();

    return 0;
}
