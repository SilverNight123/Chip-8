#pragma once
#include "SDL.h"
#include<string>
#include<cstdint>
#include <iostream>


class Platform
{
public:
    Platform(std::string title, uint64_t window_width, uint64_t window_height,
        uint64_t texture_width, uint64_t texture_height);
    ~Platform();
    void Update();
    void Input();

private:
    SDL_Window* window;
    SDL_Renderer* renderer;
    SDL_Texture* texture;

};