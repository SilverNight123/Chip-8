#pragma once
#include "SDL.h"
#include<string>
#include<cstdint>
#include <iostream>
#include<array>

class Platform
{
public:
    Platform(std::string title, int x, int y, int w, int h, uint32_t flags);
    ~Platform();

    void Update(std::array<uint32_t , 64 * 32> v, uint32_t pitch);
  
    void Input();
    SDL_Renderer* GetRenderer();
    SDL_Texture* GetTexture();
    SDL_Window* GetWindow();

private:
    SDL_Window* window;
    SDL_Renderer* renderer;
    SDL_Texture* texture;

};