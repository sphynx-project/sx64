#include <iostream>
#include <fstream>
#include <vector>
#include <core/sx64.hpp>
#include <spdlog/spdlog.h>
#include <spdlog/sinks/basic_file_sink.h>
#include <spdlog/sinks/stdout_color_sinks.h>
#include <cctype>
#include <unordered_map>

#include <global.hpp>

sx64::CPU cpu;
sx64::CPU &g_cpu = cpu;

const char *program_version = "sx64 Emulator 1.0";
const char *program_bug_address = "<kevin@alavik.se>";

void print_help()
{
    std::cout << "Usage: sx64-generic-emu [OPTIONS...]\n\n"
              << "Options:\n"
              << "  -h, --help               Show this help message\n"
              << "  -v, --verbose            Enables debug logging\n"
              << "  -vv, --extra-verbose     Enables extra debug logging\n"
              << "  -V, --version            Show version information\n"
              << "  -bi, --boot-image        Specify the system bootstrap image (ROM image)\n"
              << "  -ri, --ram-image         Specify the RAM image (kernel bootstrap image)\n"
              << "  -rs, --ram-size          Specify RAM size (e.g., 2G, 512M, 1GiB) (default: 2GB)\n";
}

void print_version()
{
    std::cout << program_version << "\n"
              << "Copyright © Kevin Alavik (shittydev.com) <kevin@alavik.se> <kevin@shittydev.com>\n"
              << "The sx64 CPU design and emulator is part of the Sphynx Projects / OS, a continuing development by Kevin Alavik.\n"
              << "For additional details, visit github.com/sphynxos or sphynx.shittydev.com.\n";
}

size_t parse_ram_size(const std::string &size_str)
{
    std::unordered_map<std::string, size_t> multipliers = {
        {"K", 1024ULL},
        {"M", 1024ULL * 1024},
        {"G", 1024ULL * 1024 * 1024},
        {"T", 1024ULL * 1024 * 1024 * 1024},
        {"KiB", 1024ULL},
        {"MiB", 1024ULL * 1024},
        {"GiB", 1024ULL * 1024 * 1024},
        {"TiB", 1024ULL * 1024 * 1024 * 1024},
        {"KB", 1000ULL},
        {"MB", 1000ULL * 1000},
        {"GB", 1000ULL * 1000 * 1000},
        {"TB", 1000ULL * 1000 * 1000 * 1000}};

    size_t value = 0;
    size_t i = 0;

    while (i < size_str.size() && std::isdigit(size_str[i]))
    {
        value = value * 10 + (size_str[i] - '0');
        i++;
    }

    std::string suffix = size_str.substr(i);
    if (suffix.empty())
    {
        return value;
    }

    auto it = multipliers.find(suffix);
    if (it != multipliers.end())
    {
        return value * it->second;
    }
    else
    {
        throw std::invalid_argument("Invalid size suffix: " + suffix);
    }
}

int main(int argc, char **argv)
{
    auto file_logger = std::make_shared<spdlog::sinks::basic_file_sink_mt>("logs/sx64.log", true);
    auto console_logger = std::make_shared<spdlog::sinks::stdout_color_sink_mt>();

    auto logger = std::make_shared<spdlog::logger>("sx64_logger", spdlog::sinks_init_list{file_logger, console_logger});
    logger->set_pattern("[%Y-%m-%d %H:%M:%S.%e] [%n] [%^%l%$] %v");
    logger->set_level(spdlog::level::info);
    spdlog::set_default_logger(logger);

    std::string sys_bootstrap;
    std::string krnl_bootstrap;
    size_t ram_size = parse_ram_size("2M");

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
        }
        else if (arg == "-vv" || arg == "--extra-verbose")
        {
            logger->set_level(spdlog::level::trace);
        }
        else if (arg == "-bi" || arg == "--boot-image")
        {
            if (i + 1 < argc)
            {
                sys_bootstrap = argv[++i];
                spdlog::debug("System bootstrap image set to: {}", sys_bootstrap);
            }
            else
            {
                spdlog::error("--boot-image option requires an argument.");
                return 1;
            }
        }
        else if (arg == "-ri" || arg == "--ram-image")
        {
            if (i + 1 < argc)
            {
                krnl_bootstrap = argv[++i];
                spdlog::debug("Kernel bootstrap image set to: {}", krnl_bootstrap);
            }
            else
            {
                spdlog::error("--ram-image option requires an argument.");
                return 1;
            }
        }
        else if (arg == "-rs" || arg == "--ram-size")
        {
            if (i + 1 < argc)
            {
                try
                {
                    ram_size = parse_ram_size(argv[++i]);
                    spdlog::debug("RAM size set to: {} bytes", ram_size);
                }
                catch (const std::exception &e)
                {
                    spdlog::error("Invalid RAM size specified: {}", e.what());
                    return 1;
                }
            }
            else
            {
                spdlog::error("--ram-size option requires an argument.");
                return 1;
            }
        }
        else
        {
            spdlog::error("Unknown argument: \"{}\"", arg);
            return 1;
        }
    }

    if (sys_bootstrap.empty())
    {
        spdlog::error("System bootstrap image (--boot-image) is required.");
        print_help();
        return 1;
    }

    spdlog::debug("Starting sx64 Emulator...");

    sx64::CPU cpu;
    g_cpu = cpu;

    auto sys_bootstrap_mem = std::make_shared<MemoryDevice>("sys-bootstrap", 0x1000, true);
    cpu.getBus()->attachDevice(sys_bootstrap_mem);
    spdlog::debug("System bootstrap memory device attached: 4096 bytes");

    std::ifstream sys_image_file(sys_bootstrap, std::ios::binary | std::ios::ate);
    if (sys_image_file.is_open())
    {
        std::streamsize size = sys_image_file.tellg();
        sys_image_file.seekg(0, std::ios::beg);
        spdlog::trace("System bootstrap image size: {} bytes", size);
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

    auto ram_mem = std::make_shared<MemoryDevice>("Generic (RAM)", ram_size, false);
    cpu.getBus()->attachDevice(ram_mem);
    spdlog::trace("RAM memory device attached: {} bytes", ram_size);

    if (!krnl_bootstrap.empty())
    {
        std::ifstream image_file(krnl_bootstrap, std::ios::binary | std::ios::ate);
        if (image_file.is_open())
        {
            std::streamsize size = image_file.tellg();
            image_file.seekg(0, std::ios::beg);
            spdlog::debug("Kernel bootstrap image size: {} bytes", size);

            std::vector<uint8_t> buffer(size);
            if (image_file.read(reinterpret_cast<char *>(buffer.data()), size))
            {
                spdlog::trace("Loading kernel bootstrap image into RAM from: {}", krnl_bootstrap);
                ram_mem->initializeWithBuffer(buffer.data(), size);
                spdlog::debug("Kernel bootstrap image loaded into RAM successfully");
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
    }
    else
    {
        spdlog::debug("No kernel bootstrap image provided, initializing RAM with zeros");
        ram_mem->initialize();
    }

    spdlog::debug("Running CPU simulation...");
    cpu.run();
    spdlog::debug("CPU simulation finished.");

    spdlog::debug("Dumping CPU state...");
    cpu.dumpState();

    return 0;
}