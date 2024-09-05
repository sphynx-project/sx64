#include <iostream>
#include <fstream>
#include <vector>
#include <core/sx64.hpp>
#include <spdlog/spdlog.h>
#include <spdlog/sinks/basic_file_sink.h>
#include <spdlog/sinks/stdout_color_sinks.h>

const char *program_version = "sx64 Emulator 1.0";
const char *program_bug_address = "<kevin@alavik.se>";

void print_help()
{
    std::cout << "Usage: sx64-generic-emu [OPTIONS...] [IMAGE]\n\n"
              << "Options:\n"
              << "  -h, --help               Show this help message\n"
              << "  -v, --verbose            Enables debug logging\n"
              << "  -vv, --extra-verbose     Enables extra debug logging\n"
              << "  -V, --version            Show version information\n"
              << "  -b, -s, --sys-bootstrap  Specify the system bootstrap image\n"
              << "  -bios                    Alias for system bootstrap image\n";
}

void print_version()
{
    std::cout << program_version << "\n"
              << "Copyright © Kevin Alavik (shittydev.com) <kevin@alavik.se> <kevin@shittydev.com>\n"
              << "The sx64 CPU design and emulator is part of the Sphynx Projects / OS, a continuing development by Kevin Alavik.\n"
              << "For additional details, visit github.com/sphynxos or sphynx.shittydev.com.\n";
}

int main(int argc, char **argv)
{
    auto file_logger = std::make_shared<spdlog::sinks::basic_file_sink_mt>("logs/sx64.log", true);
    auto console_logger = std::make_shared<spdlog::sinks::stdout_color_sink_mt>();

    auto logger = std::make_shared<spdlog::logger>("sx64_logger", spdlog::sinks_init_list{file_logger, console_logger});
    logger->set_pattern("[%Y-%m-%d %H:%M:%S.%e] [%n] [%^%l%$] %v");
    spdlog::set_default_logger(logger);

    std::string sys_bootstrap;
    std::string krnl_bootstrap;

    spdlog::trace("Starting argument parsing");

    for (int i = 1; i < argc; ++i)
    {
        std::string arg = argv[i];

        spdlog::trace("Processing argument: {}", arg);

        if (arg == "-h" || arg == "--help")
        {
            print_help();
            return 0;
        }
        else if (arg == "-V" || arg == "--version")
        {
            print_version();
            return 0;
        }
        else if (arg == "-v" || arg == "--verbose")
        {
            logger->set_level(spdlog::level::debug);
            spdlog::info("Verbose logging enabled");
        }
        else if (arg == "-vv" || arg == "--extra-verbose")
        {
            logger->set_level(spdlog::level::trace);
            spdlog::info("Extra verbose logging enabled");
        }
        else if (arg == "-b" || arg == "-s" || arg == "--sys-bootstrap" || arg == "-bios")
        {
            if (i + 1 < argc)
            {
                sys_bootstrap = argv[++i];
                spdlog::debug("System bootstrap image set to: {}", sys_bootstrap);
            }
            else
            {
                spdlog::error("--sys-bootstrap option requires an argument.");
                return 1;
            }
        }
        else
        {
            if (krnl_bootstrap.empty())
            {
                krnl_bootstrap = arg;
                spdlog::debug("Kernel bootstrap image set to: {}", krnl_bootstrap);
            }
            else if (arg[0] == '-')
            {
                spdlog::error("Unknown argument: \"{}\"", arg);
                return 1;
            }
            else
            {
                spdlog::error("Only one positional argument is allowed.");
                return 1;
            }
        }
    }

    if (sys_bootstrap.empty())
    {
        spdlog::error("System bootstrap image is required.");
        return 1;
    }

    if (krnl_bootstrap.empty())
    {
        spdlog::error("Kernel bootstrap image is required.");
        return 1;
    }

    spdlog::info("Starting sx64 Emulator...");

    sx64::CPU cpu;

    auto sys_bootstrap_mem = std::make_shared<MemoryDevice>("sys-bootstrap", 0x1000, true);
    cpu.getBus()->attachDevice(sys_bootstrap_mem);
    spdlog::debug("System bootstrap memory device attached: 4096 bytes");

    std::ifstream sys_image_file(sys_bootstrap, std::ios::binary | std::ios::ate);
    if (sys_image_file.is_open())
    {
        std::streamsize size = sys_image_file.tellg();
        sys_image_file.seekg(0, std::ios::beg);
        spdlog::debug("System bootstrap image size: {} bytes", size);

        std::vector<uint8_t> buffer(size);
        if (sys_image_file.read(reinterpret_cast<char *>(buffer.data()), size))
        {
            spdlog::trace("Loading system bootstrap image from: {}", sys_bootstrap);
            sys_bootstrap_mem->initializeWithBuffer(buffer.data(), size);
            spdlog::debug("System bootstrap image loaded successfully");
        }
        else
        {
            spdlog::error("Failed to read system bootstrap image");
            return 1;
        }
    }
    else
    {
        spdlog::error("Failed to open system bootstrap image: {}", sys_bootstrap);
        return 1;
    }

    std::ifstream image_file(krnl_bootstrap, std::ios::binary | std::ios::ate);
    if (image_file.is_open())
    {
        std::streamsize size = image_file.tellg();
        image_file.seekg(0, std::ios::beg);
        spdlog::debug("Kernel bootstrap image size: {} bytes", size);

        auto krnl_bootstrap_mem = std::make_shared<MemoryDevice>("krnl-bootstrap", size, false);
        cpu.getBus()->attachDevice(krnl_bootstrap_mem);

        std::vector<uint8_t> buffer(size);
        if (image_file.read(reinterpret_cast<char *>(buffer.data()), size))
        {
            spdlog::trace("Loading kernel bootstrap image from: {}", krnl_bootstrap);
            krnl_bootstrap_mem->initializeWithBuffer(buffer.data(), size);
            spdlog::debug("Kernel bootstrap image loaded successfully");
        }
        else
        {
            spdlog::error("Failed to read kernel bootstrap image");
            return 1;
        }
    }
    else
    {
        spdlog::error("Failed to open kernel bootstrap image: {}", krnl_bootstrap);
        return 1;
    }

    spdlog::info("Running CPU simulation...");
    cpu.run();
    spdlog::info("CPU simulation finished.");

    spdlog::info("Dumping CPU state...");
    cpu.dumpState();

    return 0;
}
