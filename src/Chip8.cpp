#include<iostream>
#include<sstream>
#include<fstream>
#include<cstdint>
#include<algorithm>
#include "SDL.h"
#include "Chip8.h"


const unsigned char fontsets[FONTSET_SIZE] =
{
    0xF0, 0x90, 0x90, 0x90, 0xF0,		// 0
	0x20, 0x60, 0x20, 0x20, 0x70,		// 1
	0xF0, 0x10, 0xF0, 0x80, 0xF0,		// 2
	0xF0, 0x10, 0xF0, 0x10, 0xF0,		// 3
	0x90, 0x90, 0xF0, 0x10, 0x10,		// 4
	0xF0, 0x80, 0xF0, 0x10, 0xF0,		// 5
	0xF0, 0x80, 0xF0, 0x90, 0xF0,		// 6
	0xF0, 0x10, 0x20, 0x40, 0x40,		// 7
	0xF0, 0x90, 0xF0, 0x90, 0xF0,		// 8
	0xF0, 0x90, 0xF0, 0x10, 0xF0,		// 9
	0xF0, 0x90, 0xF0, 0x90, 0x90,		// A
	0xE0, 0x90, 0xE0, 0x90, 0xE0,		// B
	0xF0, 0x80, 0x80, 0x80, 0xF0,		// C
	0xE0, 0x90, 0x90, 0x90, 0xE0,		// D
	0xF0, 0x80, 0xF0, 0x80, 0xF0,		// E
	0xF0, 0x80, 0xF0, 0x80, 0x80		// F

};

Chip8::Chip8(std::string file)
{

    for(auto& reg : V)
    {
        reg = 0;
    }
    for(auto& d : video)
    {
        d = 0x00000000;
    }
    for(int i = 0; i < FONTSET_SIZE; i++)
    {
        memory[FONTSET_START_ADDR  + i] = fontsets[i];
    }
    std::vector<uint8_t> rom = LoadRom(file);

    if(rom.size() > (MEMORY_SIZE - START_ADDR))
    {
        throw std::runtime_error("ROM exceeds Chip-8 memory capacity.");
    }

    std::copy_n(rom.begin(), rom.size(),memory.begin() + START_ADDR);

    std::cout << rom.size();

    pc = START_ADDR;
    sp = 0;
    I = 0;
    delay_timer = 0;
    sound_timer = 0;
    X = Y = N = NN = 0;
    NNN = 0;

}



Chip8::~Chip8()
{
}

std::vector<uint8_t> Chip8::LoadRom(std::string rom)
{
    std::ifstream file(rom, std::ios::binary | std::ios::ate);

    if(!file)
    {
        throw std::runtime_error("Failed to open ROM file.");
    }

    std::streamsize sz = file.tellg();
   
    if(sz <= 0)
    {
        throw std::runtime_error("ROM is empty.");
    }

    file.seekg(0, std::ios::beg);

    std::vector<uint8_t> buffer(sz);

    if(!file.read(reinterpret_cast<char*>(buffer.data()), sz))
    {
        throw std::runtime_error("Failed to read ROM file.");
    }

   file.close();

   return buffer;
}
std::array<uint32_t, VIDEO_WIDTH * VIDEO_HEIGHT> Chip8::GetVideo()
{
    return video;
}
void Chip8::fetch()
{

    uint16_t opcode = (memory[pc] << 8) | memory[pc + 1];

    pc += 2;
    //Avoid code duplication
    
    X = (opcode & 0x0f00) >> 8;

    Y = (opcode & 0x00F0) >> 4;

    N = opcode & 0x0F;

    NN = opcode & 0xff;

    NNN = opcode & 0x0FFF;


    execute(opcode, event);

    if(delay_timer >= 0)
    {
        delay_timer--;
    }
    if(sound_timer >= 0)
    {
        sound_timer--;
    }
    

}
void Chip8::execute(uint16_t op, SDL_Event event)
{
    switch (op & 0xf000)
    {

        case 0x0000:
        {
            switch (op)
            {
            case 0x00E0: //clear screen
               memset(video.data(), 0, sizeof video);
            break;
            case 0x00EE: //RET
                if(sp == 0)
                {
                    throw std::runtime_error("stack overthrow");
                }
                pc = stack[sp];
               --sp;
            break;
            
            default:
             printf("Unknown 0x0000 opcode: 0x%04X\n", op);
            break;
            }
        }
        break;
        case 0x1000: //jump NNN
            pc = NNN;
            
        break;
        case 0x2000: //Call addr
            if(sp >= 16)
            {
                throw std::runtime_error("stack overflow");
            }
            stack[sp] = pc;
            ++sp;
            pc = NNN;
        break;
        case 0x3000: //SE Vx, byte
            if(V[X] == NN)
            {
                pc += 2;
            }

        break;
        case 0x4000: //SNE Vx, byte
            if(V[X] != NN)
            {
                pc += 2;
            }
        break;
        case 0x5000: //SE Vx, Vy
           if(V[X] == V[Y])
           {
                pc += 2;
           }
        break;
        case 0x6000: //LD Vx, byte
            V[X] = NN;
        break;
        case 0x7000: //DD Vx, byte
            V[X] += NN;

        break;
        case 0x8000:
        {
            switch(op & 0x000F)
            {
                case 0x0000: //LD Vx, Vy
                   V[X] = V[Y];
                break;
                case 0x0001: //OR Vx, Vy
                    V[X] |=  V[Y]; 
                break;
                case 0x0002: //AND Vx, Vy
                    V[X] &= V[Y];
                break;
                case 0x0003: //XOR Vx, Vy
                    V[X] ^= V[Y];
                break;
                case 0x0004: //ADD Vx, Vy
                    uint8_t sum = V[X] + V[Y];
                    if(sum > 255)
                    {
                        V[0xf] = 1;
                    }
                    else
                    {
                        V[0xf] = 0;
                    }
                    V[X] = sum & 0xff;

                break;
                case 0x0005://SUB Vx, Vy
                    if(V[X] > V[Y])
                    {
                        V[0xF] = 1;
                    }
                    else
                    {
                        V[0xF] = 0;
                    }
                    V[X] -= V[Y];

                break;
                case 0x0006:
                    printf("SHR Vx {, Vy}\n");
                break;
                case 0x0007:
                    printf("SUBN Vx, Vy\n");
                break;
                case 0x000E:
                    printf("SHL Vx {, Vy}\n");
                break;
                default:
                    printf("Unknown opcode {%d} 0x8000 \n", op);
                break;
            }
        }
        break;
        case 0x9000: //SNE Vx, Vy
           if(X != Y)
           {
             pc += 2;
           }
        break;
        case 0xA000: //LD I, addr
            I = NNN;
        break;
        case 0xB000:
            printf("JP V0, addr\n");
        break;
        case 0xC000:
            printf("RND Vx, byte\n");
        break;
        case 0xD000: //DRW Vx, Vy
        {
            uint8_t x = V[X] % VIDEO_WIDTH;
            uint8_t y = V[Y] % VIDEO_HEIGHT;
            uint8_t height = N;
        
            V[0xF] = 0;
            
            for(int row = 0; row < height; row++) {
                uint8_t spriteByte = memory[I + row];
                for(int col = 0; col < 8; col++) {
                    if((spriteByte & (0x80 >> col)) != 0) {
                        int pixelPos = (y + row) * VIDEO_WIDTH + (x + col);
                        if(pixelPos < video.size()) {
                            if(video[pixelPos] == 0xFFFFFFFF) {
                                V[0xF] = 1;
                            }
                            video[pixelPos] ^= 0xFFFFFFFF;
                        }
                    }
                }
            }
            
        }
        break;
        case 0xE000:
        {
            switch (op &0xF0FF)
            {
            case 0x009E:
                printf("SKP Vx\n");
            break;
            case 0x00A1:
                printf("SKNP Vx\n");
            break;
            default:
                printf("Unknown 0xF000 opcode: 0x%04X\n", op);
                break;
            }
        }
        break;
        case 0xF000:
        {
            switch (op & 0xf0ff)
            {
            case 0x0007:
                printf("LD Vx, DT\n");
                break;
            case 0x000A:
                printf("LD Vx, K\n");
                break;
            case 0x0015:
                printf("LD Vx, K\n");
            break;
            case 0x0018:
                printf("LD ST, Vx\n");
            break;
            case 0x001E:
                printf("ADD I, Vx\n");
            break;
            case 0x0029:
                printf("LD F, Vx\n");
            break;
            case 0x0033:
                printf("LD B, Vx\n");
            break;
            case 0x0055:
                printf("LD [I], Vx\n");
            break;
            case 0x0065:
                printf("LD Vx, [I]\n");
            break;
            default:
                printf("Unknown 0xF000 opcode: 0x%04X\n", op);
                break;
            }
        }
        default:
             printf("Unknown opcode: 0x%04X\n", op);
        break;

    }
    
}
