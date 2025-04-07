#include<iostream>
#include<fstream>
#include<cstdint>
#include<algorithm>
#include <ctime>
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

    last_timer_update = SDL_GetTicks64();

    std::srand(static_cast<unsigned int>(std::time(nullptr)));

    if( Mix_OpenAudio( 22050, MIX_DEFAULT_FORMAT, 2, 4096 ) == -1 )
    {
        throw std::runtime_error("Failed to initalize SDL_Mixer");
    }

    sound =Mix_LoadWAV("sound_effect.mp3");
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
    Mix_FreeChunk( sound);
    Mix_CloseAudio();
}

void Chip8::Reset()
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


    last_timer_update = SDL_GetTicks64();

    std::srand(static_cast<unsigned int>(std::time(nullptr)));

    if( Mix_OpenAudio( 22050, MIX_DEFAULT_FORMAT, 2, 4096 ) == -1 )
    {
        throw std::runtime_error("Failed to initalize SDL_Mixer");
    }

    sound =Mix_LoadWAV("sound_effect.mp3");
    pc = START_ADDR;
    sp = 0;
    I = 0;
    delay_timer = 0;
    sound_timer = 0;
    X = Y = N = NN = 0;
    NNN = 0;
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
void Chip8::handleKeyEvent(SDL_Event event) {
    if (event.type == SDL_KEYDOWN || event.type == SDL_KEYUP) {
        bool pressed = (event.type == SDL_KEYDOWN);
        
        switch (event.key.keysym.sym) {
            // Original CHIP-8 keypad layout:
            // 1 2 3 C       -> 1 2 3 4
            // 4 5 6 D       -> Q W E R
            // 7 8 9 E       -> A S D F
            // A 0 B F       -> Z X C V

            case SDLK_1: keypad[0x7] = pressed; break;
            case SDLK_2: keypad[0x2] = pressed; break;
            case SDLK_3: keypad[0x3] = pressed; break;
            case SDLK_q: keypad[0x1] = pressed; break;
            case SDLK_w: keypad[0x5] = pressed; break;  
            case SDLK_e: keypad[0x6] = pressed; break;
            case SDLK_a: keypad[0x4] = pressed; break;
            case SDLK_s: keypad[0x8] = pressed; break;  
            
            
            case SDLK_d: keypad[0x9] = pressed; break;
            case SDLK_f: keypad[0xE] = pressed; break;
            
            case SDLK_z: keypad[0xA] = pressed; break;
            case SDLK_x: keypad[0x0] = pressed; break;
            case SDLK_c: keypad[0xB] = pressed; break;
            case SDLK_v: keypad[0xF] = pressed; break;
            
            default: break;
        }
    }
  
}
bool Chip8::isKeyPressed(uint8_t chip8Key) {
    if (chip8Key > 0xF) {
        return false; // Invalid key
    }
    return keypad[chip8Key];
}
void Chip8::updateTimers()
{
    // Get current time
    uint64_t current_time = SDL_GetTicks64();
    
    // Calculate time since last update (in milliseconds)
    uint64_t elapsed = current_time - last_timer_update;
    
    // Timers should decrement at 60Hz (every 16.67ms)
    if (elapsed >= 16.67) // 1000ms / 60 ≈ 16.67ms
    {
        // Calculate how many timer decrements to perform
        int decrements = elapsed / 16.67;
        
        // Decrement delay timer if it's greater than 0
        if (delay_timer > 0)
        {
            if (decrements >= delay_timer)
                delay_timer = 0;
            else
                delay_timer -= decrements;
        }
        
        // Decrement sound timer if it's greater than 0
        if (sound_timer > 0)
        {
            Mix_PlayChannel(-1, sound, 0 );


            if (decrements >= sound_timer)
                sound_timer = 0;
            else
                sound_timer -= decrements;
        }
        
        // Update the last timer update time
        // We add the actual time used for the decrements to avoid drift
        last_timer_update += decrements * 16.67;
    }
}

void Chip8::fetch()
{
    uint16_t opcode = (memory[pc] << 8) | memory[pc + 1];

    //Avoid code duplication
    pc += 2;

    X = (opcode & 0x0f00) >> 8;

    Y = (opcode & 0x00F0) >> 4;

    N = opcode & 0x0F;

    NN = opcode & 0xff;

    NNN = opcode & 0x0FFF;

    execute(opcode, event);
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
                    throw std::runtime_error("stack overflow");
                }
                --sp;
                pc = stack[sp];
              
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
                {
                    uint8_t sum = 0;
                    sum = V[X] + V[Y];
                    if(sum > 255)
                    {
                        V[0xf] = 1;
                    }
                    else
                    {
                        V[0xf] = 0;
                    }
                    V[X] = sum & 0xff;
                }
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
                case 0x0006: //SHR Vx {, Vy}
                
                    if (modernShiftBehavior) {
                        V[0xF] = V[X] & 0x1;
                        V[X] >>= 1;
                    } else {
                        V[X] = V[Y];
                        V[0xF] = V[X] & 0x1;
                        V[X] >>= 1;
                    }
                break;
                case 0x0007: //SUBN Vx, Vy
                    if(V[Y] > V[X])
                    {
                        V[0xF] = 1;
                    }
                    else
                    {
                        V[0xf] = 0;
                    }
                    V[X] = V[Y] - V[X];

                break;
                case 0x000E://SHL Vx {, Vy}
                {
                   
                    V[0xF] = (V[X] & 0x80) >> 7; // Set VF to most significant bit of VX
                    V[X] <<= 1; 
                }
                break;
                default:
                    printf("Unknown opcode {%d} 0x8000 \n", op);
                break;
            }
        }
        break;
        case 0x9000: //SNE Vx, Vy
           if(V[X] != V[Y])
           {
             pc += 2;
           }
        break;
        case 0xA000: //LD I, addr
            I = NNN;
        break;
        case 0xB000: //JP V0, addr
           pc = NNN + V[0];
        break;
        case 0xC000: //RND Vx, byt
        V[X] = (std::rand() % 256) & NN;

        break;
        case 0xD000: //DRW Vx, Vy
        {
            if (N == 0) {  // Special case for ball
                memset(video.data(), 0, sizeof(video));
            }
            
            uint8_t x = V[X] % VIDEO_WIDTH;
            uint8_t y = V[Y] % VIDEO_HEIGHT;
            uint8_t height = N;
            
            V[0xF] = 0; // Reset collision flag
            
            for(int row = 0; row < height; row++) {
                if(y + row >= VIDEO_HEIGHT) break; // Stay within bounds
                
                uint8_t spriteByte = memory[I + row];
                for(int col = 0; col < 8; col++) {
                    if(x + col >= VIDEO_WIDTH) break; // Stay within bounds
                    
                    if((spriteByte & (0x80 >> col)) != 0) {
                        int pixelPos = (y + row) * VIDEO_WIDTH + (x + col);
                        if(video[pixelPos] == 0xFFFFFFFF) {
                            V[0xF] = 1; // Set collision flag
                        }
                        video[pixelPos] ^= 0xFFFFFFFF;
                    }
                }
            }
        }
        break;
        case 0xE000:
        {
            printf("E000 opcode: 0x%04X, checking key 0x%X\n", op, V[X]);
            switch (op &0xF0FF)
            {
            case 0xE09E: //SKP Vx
            {
                if (isKeyPressed(V[X])) {
                    pc += 2;
                }
            } 
            break;
            case 0xE0A1://SKNP Vx
            {
                if (!isKeyPressed(V[X])) {
                    pc += 2;
                }
            }
               
            break;
            default:
                printf("Unknown 0xF000 opcode: 0x%04X\n", op);
                break;
            }
        }
        break;
        case 0xF000:
        {
            switch (op & 0x00FF)
            {
            case 0x0007: // LD Vx, DT
                V[X] = delay_timer;
                break;
            case 0x000A: // LD Vx, K
            {
                bool key_pressed = false;
                for (uint8_t i = 0; i < 16; ++i) {
                    if (keypad[i]) {
                        V[X] = i;
                        key_pressed = true;
                        break;
                    }
                }
                if (!key_pressed) {
                    pc -= 2; // Retry this instruction next cycle
                }
            }
                break;
            case 0x0015: // LD DT, Vx
                delay_timer = V[X];
                break;
            case 0x0018: // LD ST, Vx
                sound_timer = V[X];
                break;
            case 0x001E: // ADD I, Vx
                I += V[X];
                break;
            case 0x0029: // LD F, Vx
                I = FONTSET_START_ADDR + (5 * V[X]);
                break;
            case 0x0033: // LD B, Vx
                memory[I] = (V[X] % 1000) / 100;
                memory[I + 1] = (V[X] % 100) / 10;
                memory[I + 2] = (V[X] % 10);
                break;
            case 0x0055: // LD [I], Vx
                for(uint8_t i = 0; i <= X; ++i)
                {
                    memory[I + i] = V[i];
                }
                break;
            case 0x0065: // LD Vx, [I]
                for(uint8_t i = 0; i <= X; ++i)
                {
                    V[i] = memory[I + i];
                }
                break;
            default:
                printf("Unknown 0xF000 opcode: 0x%04X\n", op);
                break;
            }
            break; 
        }
        default:
            printf("Unknown opcode: 0x%04X\n", op);
            break;
    }
}
