#pragma once

namespace emulator
{
    class chip8
    {
    public:
        chip8();
        ~chip8();
        void emulateCycle();
        bool load(const char *filepath);

        bool drawFlag;
        unsigned char chip8_fontset[80] =
            {
                0x20, 0x60, 0x20, 0x20, 0x70, // 1
                0xF0, 0x10, 0xF0, 0x80, 0xF0, // 2
                0xF0, 0x10, 0xF0, 0x10, 0xF0, // 3
                0x90, 0x90, 0xF0, 0x10, 0x10, // 4
                0xF0, 0x90, 0x90, 0xF0, 0x20, // Q
                0x90, 0x90, 0x90, 0xF0, 0x90, // W
                0xF0, 0x80, 0xF0, 0x80, 0xF0, // E
                0x70, 0x90, 0x70, 0xA0, 0x90, // R
                0xF0, 0x90, 0xF0, 0x90, 0x90, // A
                0xF0, 0x80, 0xF0, 0x10, 0xF0, // S
                0xE0, 0x90, 0x90, 0x90, 0xE0, // D
                0xF0, 0x80, 0xF0, 0x80, 0x80, // F
                0xF0, 0x20, 0x60, 0x80, 0xF0, // Z
                0x90, 0x90, 0x40, 0x90, 0x90, // X
                0x90, 0x90, 0x90, 0x90, 0x60  // V
        };
        unsigned char gfx[64 * 32]; //monochromatic screen with 2048 pixels
        unsigned char key[16];

    private:
        void initialize();

        unsigned char memory[4096];
        unsigned short opcode;
        unsigned char V[16]; // 8-bit CPU register
        unsigned short I;    //addres register
        unsigned short PC;   // program counter
        unsigned short stack[16];
        unsigned short sp; //stack pointer
        unsigned char delay_timer;
        unsigned char sound_timer;
    };
}