#include "Platform.h"


Platform::Platform(std::string title, int x, int y, int w, int h, uint32_t flags)
{
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        std::cerr << "SDL could not initialize! SDL_Error: " << SDL_GetError() << std::endl;
       
    }
    window = SDL_CreateWindow(title.c_str(), x, y, w, h, flags);
    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);

    texture = SDL_CreateTexture(renderer,SDL_PIXELFORMAT_ARGB8888,
        SDL_TEXTUREACCESS_STREAMING, 64, 32);
    
}
SDL_Window* Platform::GetWindow()
{
    return window;
}
SDL_Renderer* Platform::GetRenderer()
{
    return renderer;
}
SDL_Texture* Platform::GetTexture()
{
    return texture;
}

void Platform::Input()
{
    
}
void Platform::Update(std::array<uint32_t , 64 * 32> v, uint32_t pitch)
{
    SDL_UpdateTexture(texture, nullptr, v.data(), pitch);
    SDL_RenderClear(renderer);
    SDL_RenderCopy(renderer, texture, nullptr, nullptr);
    SDL_RenderPresent(renderer);
}
Platform::~Platform()
{
    SDL_DestroyTexture(texture);
    SDL_DestroyWindow(window);
    SDL_Quit();

}

