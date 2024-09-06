#include <devices/serial.hpp>
#include <spdlog/spdlog.h>
#include <global.hpp>
#include <stdexcept>
#include <iostream>
#include <sstream>

SerialDevice::SerialDevice(const std::string &name, uint64_t width, uint64_t height, uint64_t baseAddress)
    : Device(name, false, baseAddress), window(nullptr), renderer(nullptr), font(nullptr), textColor{255, 255, 255, 255}, width(width), height(height)
{
    spdlog::debug("Initializing SerialDevice with width: {} and height: {}", width, height);
    size = 1;
}

SerialDevice::~SerialDevice()
{
    spdlog::trace("Destroying SerialDevice resources");

    std::string userInput;
    spdlog::info("The CPU finished executing. Press Enter to quit the serial monitor...");
    std::getline(std::cin, userInput);

    if (font)
    {
        TTF_CloseFont(font);
        spdlog::debug("Serial: Font closed");
    }
    if (renderer)
    {
        SDL_DestroyRenderer(renderer);
        spdlog::debug("Serial: Renderer destroyed");
    }
    if (window)
    {
        SDL_DestroyWindow(window);
        spdlog::debug("Serial: Window destroyed");
    }
    TTF_Quit();
    SDL_Quit();
}

void SerialDevice::initialize()
{
    spdlog::trace("Starting SDL and TTF initialization");

    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_EVENTS) < 0)
    {
        throw std::runtime_error("SDL could not initialize! SDL_Error: " + std::string(SDL_GetError()));
    }

    if (TTF_Init() == -1)
    {
        throw std::runtime_error("SDL_ttf could not initialize! TTF_Error: " + std::string(TTF_GetError()));
    }

    window = SDL_CreateWindow("sx64 Serial", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, width, height, SDL_WINDOW_SHOWN);
    if (!window)
    {
        throw std::runtime_error("Window could not be created! SDL_Error: " + std::string(SDL_GetError()));
    }

    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    if (!renderer)
    {
        throw std::runtime_error("Renderer could not be created! SDL_Error: " + std::string(SDL_GetError()));
    }

    font = TTF_OpenFont("assets/serial-font.ttf", 16);
    if (!font)
    {
        throw std::runtime_error("Failed to load font! TTF_Error: " + std::string(TTF_GetError()));
    }

    spdlog::trace("SDL and TTF initialized successfully");
    update();
}

void SerialDevice::update()
{
    spdlog::trace("Updating SerialDevice display");

    SDL_Event event;
    while (SDL_PollEvent(&event))
    {
        if (event.type == SDL_QUIT)
        {
            spdlog::info("Window close event detected");
            g_cpu.halt();
        }
    }

    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderClear(renderer);

    SDL_Color color = textColor;
    int x = 0, y = 0;

    std::istringstream textStream(textBuffer);
    std::string line;
    while (std::getline(textStream, line))
    {
        SDL_Surface *surface = TTF_RenderText_Blended(font, line.c_str(), color);
        if (!surface)
        {
            throw std::runtime_error("Unable to create text surface: " + std::string(TTF_GetError()));
        }

        SDL_Texture *texture = SDL_CreateTextureFromSurface(renderer, surface);
        if (!texture)
        {
            SDL_FreeSurface(surface);
            throw std::runtime_error("Unable to create text texture: " + std::string(SDL_GetError()));
        }

        SDL_Rect destRect = {x, y, surface->w, surface->h};
        SDL_RenderCopy(renderer, texture, nullptr, &destRect);

        SDL_DestroyTexture(texture);
        SDL_FreeSurface(surface);

        x += surface->w;

        if (static_cast<uint64_t>(x) + surface->w > width)
        {
            x = 0;
            y += surface->h;
        }

        if (static_cast<uint64_t>(y) + surface->h > height)
        {
            y -= surface->h;
        }
    }

    SDL_RenderPresent(renderer);
    spdlog::trace("Display updated");
}

void SerialDevice::write(uint64_t address, uint64_t data)
{
    spdlog::debug("Writing data: {} at address: {}", data, address);

    addressMap[address] = data;

    char c = static_cast<char>(data);
    if (std::isprint(c))
    {
        textBuffer += c;
        spdlog::trace("Added character: {}", c);
    }
    else if (c == '\n')
    {
        textBuffer += '\n';
    }

    update();
}

uint64_t SerialDevice::getSize() const
{
    return size;
}
