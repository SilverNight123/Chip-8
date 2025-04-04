// src/main.cpp
#include <SDL.h>
#include <iostream>
#include"Chip8.h"

int main(int argc, char* argv[]) {

    Chip8 chip8;
    // Initialize SDL
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        std::cerr << "SDL could not initialize! SDL_Error: " << SDL_GetError() << std::endl;
        return 1;
    }
    
    // Create a window
    SDL_Window* window = SDL_CreateWindow(
        "Chip8",
        SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
        800, 600,
        SDL_WINDOW_SHOWN
    );
    
    if (!window) {
        std::cerr << "Window could not be created! SDL_Error: " << SDL_GetError() << std::endl;
        SDL_Quit();
        return 1;
    }
    
    // Create a renderer
    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    if (!renderer) {
        std::cerr << "Renderer could not be created! SDL_Error: " << SDL_GetError() << std::endl;
        SDL_DestroyWindow(window);
        SDL_Quit();
        return 1;
    }
    
    // Main loop flag
    bool quit = false;
    
    // Event handler
    SDL_Event e;
    
    // Main loop
    while (!quit) {
        // Handle events on queue
        while (SDL_PollEvent(&e) != 0) {
            // User requests quit
            if (e.type == SDL_QUIT) {
                quit = true;
            }
        }
        
        // Clear screen
        SDL_SetRenderDrawColor(renderer, 100, 149, 237, 255); // Cornflower blue
        SDL_RenderClear(renderer);
        
        // Draw a white rectangle
        SDL_Rect rect = {300, 250, 200, 100};
        SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255); // White
        SDL_RenderFillRect(renderer, &rect);
        
        // Update screen
        SDL_RenderPresent(renderer);
    }
    
    // Cleanup
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
    
    return 0;
}