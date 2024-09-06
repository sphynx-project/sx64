#pragma once

#include <core/device.hpp>
#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <string>
#include <unordered_map>

class SerialDevice : public Device
{
public:
    SerialDevice(const std::string &name = "Serial Device", uint64_t width = 800, uint64_t height = 600, uint64_t baseAddress = 0x8000);
    ~SerialDevice() override;

    void initialize() override;
    void update() override;
    void write(uint64_t address, uint64_t data) override;

    uint64_t getSize() const override;

private:
    SDL_Window *window;
    SDL_Renderer *renderer;
    TTF_Font *font;
    SDL_Color textColor;
    std::string textBuffer;
    uint64_t width;
    uint64_t height;
    uint64_t size;

    std::unordered_map<uint64_t, uint64_t> addressMap;
};
