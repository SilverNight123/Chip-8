#pragma once
#include<iostream>
#include<cstdint>
#include<array>
#include <fstream>
#include<array>

#define PC_START_ADDR 0x200
#define FONT_SIZE 0x80
#define FONTSET_START_ADDR 0x50
#define MEMORY_SIZE 4096

class Chip8
{
public:
    Chip8(/* args */);
    ~Chip8();

    void fetch();
    void execute(uint16_t op);

    int LoadRom(std::string rom);

    void CLS();
    void RET();
    void JMP();
    void CALL();


 


private:
    std::array<uint8_t, 16> V; // VF is the flag register.

    uint16_t I; //16-bit index register I
    uint16_t pc;
    uint8_t sp;
    uint64_t stack[16];
    uint16_t clock_cycle;

    uint8_t delay_timer;
    uint8_t sound_timer;
    uint16_t video[64 * 32];
    std::array<uint16_t, MEMORY_SIZE> memory;


};
