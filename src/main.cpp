// src/main.cpp
#include <SDL.h>
#include <iostream>
#include"Chip8.h"

int main(int argc, char* argv[]) {

    Chip8 chip8 = Chip8();

    for(int i = 0; i < 356; i++)
    {
        chip8.fetch();
    }
    
    return 0;
}