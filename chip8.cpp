#include "chip8.h"
#include <stdio.h>

emulator::chip8::chip8()
{
    //pass
}

emulator::chip8::chip8()
{
    //pass
}

void emulator::chip8::initialize()
{
    PC = 0x200; // start program counter at 0x200
    opcode = 0; // reset current opcode
    I = 0;      // reset index register
    sp = 0;     // resert stack pointer

    // Clear display	
    // Clear stack
    // Clear registers V0-VF
    // Clear memory

    for (int i=0; i<80; ++i)
    {
        memory[i] = chip8_fontset[i];
    }

    // reset timers
    
}

void emulator::chip8::load()
{     
    FILE *f = fopen(".\\data\\pong2.c8", "rb+");
    if (f)
    {
        fseek(f, 0L, SEEK_END);
        long bufferSize = ftell(f); // get file size
        fseek(f, 0L ,SEEK_SET); //go back to the beginning
        char* buffer = new char[bufferSize]; // allocate the read buf
        fread(buffer, 1, bufferSize, f);
        fclose(f);

        for (int i=0; i<bufferSize; ++i)
        {
            memory[i+512] = buffer[i];
        }

        delete[] buffer;
    }
}

void emulator::chip8::emulateCycle()
{
    opcode = memory[PC] << 8 | memory[PC+1];
    
    switch (opcode && 0xF000)
    {
    case 0xA000:
      // Execute opcode
        break;
    
    default:
        printf ("Unknown opcode: 0x%X\n", opcode);
;
    }

}