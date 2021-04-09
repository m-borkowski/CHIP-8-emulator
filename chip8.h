#pragma once

namespace emulator
{
    class chip8
    {                
    public:
        chip8();
        ~chip8();

        unsigned char gfx[64 * 32]; //monochromatic screen with 2048 pixels
        unsigned char key[16];

    private:
        unsigned char memory[4096];
        unsigned short opcode; 
        unsigned char V[16]; // 8-bit CPU register
        unsigned short I; //addres register
        unsigned short PC; // program counter
        unsigned short stack;
        unsigned short ps; //stack pointer
        unsigned char key[16];
        unsigned char delay_timer;
        unsigned char sound_timer;
    
    };
}