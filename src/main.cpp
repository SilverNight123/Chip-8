// src/main.cpp
#include <SDL.h>
#include <iostream>
#include "Platform.h"
#include"Chip8.h"

int main(int argc, char* argv[]) {

    std::string filepath = "chip8-roms/games/Cave.ch8";

    Chip8 chip8 = Chip8(filepath);

    for(int i = 0; i < 356; i++)
    {
        chip8.fetch();
    }
    
    return 0;
}