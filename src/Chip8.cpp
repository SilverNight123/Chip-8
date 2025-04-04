#include "Chip8.h"

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

    std::vector<uint8_t> rom = LoadRom("chip8-roms/games/Airplane.ch8");

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
void Chip8::fetch()
{
    uint16_t opcode = (memory[pc] << 8) | memory[pc + 1];
    pc += 2;

    execute(opcode);
    

}
void Chip8::execute(uint16_t op)
{

    switch (op & 0xf000)
    {

        case 0x0000: //0nnn
        {
            switch (op)
            {
            case 0x00E0:
               printf("CLS\n");
            break;
            case 0x00EE:
                printf("RET\n");
            break;
            
            default:
                printf("Unknow opcode {%d} 0x0000 \n", op);
            break;
            }
        }
        break;
        case 0x1000:
            printf("JP addr\n");
        break;
        case 0x2000:
            printf("Call addr\n");
        break;
        case 0x3000:
            printf("SE Vx, byte\n");
        break;
        case 0x4000:
            printf("SNE Vx, byte\n");
        break;
        case 0x5000:
            printf("SE Vx, Vy\n");
        break;
        case 0x6000:
            printf("LD Vx, byte\n");
        break;
        case 0x7000:
            printf("ADD Vx, byte\n");
        break;
        case 0x8000:
        {
            switch(op)
            {
                case 0x0000:
                    printf("LD Vx, Vy\n");
                break;
                case 0x0001:
                    printf("OR Vx, Vy\n");
                break;
                case 0x0002:
                    printf("AND Vx, Vy\n");
                break;
                case 0x0003:
                    printf("XOR Vx, Vy\n");
                break;
                case 0x0004:
                    printf("ADD Vx, Vy\n");
                break;
                case 0x0005:
                    printf("SUB Vx, Vy\n");
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
                    printf("Unknow opcode {%d} 0x8000 \n", op);
                 break;
            }
        }
        break;
        case 0x9000:
            printf("SNE Vx, Vy\n");
        break;
        case 0xA000:
            printf("LD I, addr\n");
        break;
        case 0xB000:
            printf("JP V0, addr\n");
        break;
        case 0xC000:
            printf("RND Vx, byte\n");
        break;
        case 0xD000:
            printf("DRW Vx, Vy, nibble\n");
        break;
        case 0xE000:
        {
            switch (op)
            {
            case 0x009E:
                printf("SKP Vx\n");
            break;
            case 0x00A1:
                printf("SKNP Vx\n");
            break;
            default:
                printf("Unknow opcode {%d} 0xE000 \n", op);
                break;
            }
        }
        case 0xF000:
        {
            switch (op)
            {
            case 0x000A:
                printf("LD Vx, DT\n");
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
                printf("Unknow opcode {%d} 0xF000 \n", op);
                break;
            }
        }
        default:
        printf("Unknow opcode {%d}\n", op);
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
    //pc = stack[];
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