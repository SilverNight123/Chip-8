#pragma once
#include<vector>
#include<array>
#include "SDL_mixer.h"


#define START_ADDR 0x200
#define FONTSET_SIZE 0x80
#define FONTSET_START_ADDR 0x50
#define MEMORY_SIZE 4096
#define VIDEO_HEIGHT 32
#define VIDEO_WIDTH 64

class Chip8
{
public:
    Chip8(std::string file);
    ~Chip8();

    void Reset();

    void fetch();


    std::vector<uint8_t> LoadRom(std::string rom);

    std::array<uint32_t,VIDEO_WIDTH * VIDEO_HEIGHT> GetVideo();
    void handleKeyEvent(SDL_Event event);
    void updateTimers();

private:
    std::array<uint8_t, 16> V; // VF is the flag register.

    uint16_t I; //16-bit index register I
    uint16_t pc;
    uint8_t sp;
    
    uint8_t X;
    uint8_t Y;
    uint8_t N;
    uint8_t NN;
    uint16_t NNN;
    uint64_t last_timer_update; 
    std::array<uint16_t, 16> stack;
    uint16_t clock_cycle;

    bool modernShiftBehavior = true;

    uint8_t delay_timer;
    uint8_t sound_timer;
    std::array<uint32_t, VIDEO_WIDTH * VIDEO_HEIGHT> video;
    std::array<uint8_t, MEMORY_SIZE> memory;

    SDL_Event event;
    bool isKeyPressed(uint8_t chip8Key);
    std::array<bool, 16> keypad = {false};

    void execute(uint16_t op, SDL_Event event);

    std::string soundfilepath;

    Mix_Chunk* sound = nullptr;

  
};
