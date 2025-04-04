#include "Platform.h"


Platform::Platform(std::string title, uint64_t window_width, uint64_t window_height,
    uint64_t texture_width, uint64_t texture_height)
{
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        std::cerr << "SDL could not initialize! SDL_Error: " << SDL_GetError() << std::endl;
       
    }
    window = SDL_CreateWindow(title.c_str(), 0,0, window_width,window_height, SDL_WINDOW_SHOWN);
    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    
}
void Platform::Input()
{

}
void Platform::Update()
{

}
Platform::~Platform()
{
    free(window);
    free(renderer);
    free(texture);
}