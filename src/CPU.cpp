#include "Chip8.h"
#include<iostream>
#include<fstream>
#include <sstream>

Chip8::Chip8()
{
    for(auto& reg : V)
    {
        reg = 0;
    }
    for(auto& d : video)
    {
        d = 0;
    }

    pc = PC_START_ADDR;
    sp = 0;
    I = 0;
    delay_timer = 0;
    sound_timer = 0;


}

Chip8::~Chip8()
{
}
int Chip8::LoadRom(std::string rom)
{
    std::ifstream file("chip8-roms/games/Airplane.ch8", std::ios_base::binary);
    std::string line;
    
    if(!file.is_open())
    {
        std::cerr << "Failed to open file" << std::endl;
        return 1;
    }
   
   file.close();
}
void Chip8::fetch()
{
    uint16_t opcode =(memory[pc] >> 8) | memory[pc + 1];
    pc += 2;
    


}
void Chip8::execute(uint16_t op)
{
    switch (op)
    {

        case 0x0000:
        {

        }
        case 0x1000:
            printf("JP addr");
        break;
        case 0x2000:
            printf("Call addr");
        break;
        case 0x3000:
            printf("SE Vx, byte");
        break;
        case 0x4000:
            
        break;



    }
}

//00E0
void Chip8::CLS()
{

}
//00EE
void Chip8::RET()
{
    pc = stack[sp];
    sp--;
}
//1NNN
void Chip8::JMP()
{

}
//2NNN
void Chip8::CALL()
{
    sp++;
    stack[sp] = pc;
}