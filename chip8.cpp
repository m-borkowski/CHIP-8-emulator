#include "chip8.h"
#include <stdio.h>
#include <iostream>

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

    for (int i = 0; i < 80; ++i)
    {
        memory[i] = chip8_fontset[i];
    }

    delay_timer = 0;
    sound_timer = 0;
}

void emulator::chip8::load()
{
    FILE *f = fopen(".\\data\\pong2.c8", "rb+");
    if (f)
    {
        fseek(f, 0L, SEEK_END);
        long bufferSize = ftell(f);          // get file size
        fseek(f, 0L, SEEK_SET);              // go back to the beginning
        char *buffer = new char[bufferSize]; // allocate the read buf
        fread(buffer, 1, bufferSize, f);
        fclose(f);

        for (int i = 0; i < bufferSize; ++i)
        {
            memory[i + 512] = buffer[i];
        }

        delete[] buffer;
    }
}

void emulator::chip8::emulateCycle()
{
    opcode = memory[PC] << 8 | memory[PC + 1];

    switch (opcode & 0xF000)
    {
    case 0x0000:
        switch (opcode & 0x000F)
        {
        case 0x0000: // 0x00E0: Clears the screen
            /* code */
            break;

        case 0x000E: // 0x00EE: Returns from subroutine
            --sp;
            stack[sp] = PC;
            PC += 2;
            break;

        default:
            printf("Unknown opcode: 0x%X\n", opcode);
        }

    case 0x1000:
        PC = opcode & 0x0FFF;
        break;

    case 0x2000:
        stack[sp] = PC;
        ++sp;
        PC = opcode & 0x0FFF;
        break;

    case 0x3000:
        if (V[(opcode & 0x0F00) >> 8] == (opcode & 0x00FF))
            PC += 4;
        else
            PC += 2;
        break;

    case 0x4000:
        if (V[(opcode & 0x0F00) >> 8] != (opcode & 0x00FF))
            PC += 4;
        else
            PC += 2;
        break;

    case 0x5000:
        if (V[(opcode & 0x0F00) >> 8] == V[(opcode & 0x00F0) >> 4])
            PC += 4;
        else
            PC += 2;
        break;

    case 0x6000:
        V[(opcode & 0x0F00) >> 8] = opcode & 0x00FF;
        PC += 2;
        break;

    case 0x7000:
        V[(opcode & 0x0F00) >> 8] += opcode & 0x00FF;
        PC += 2;
        break;

    case 0x8000:
        switch (opcode & 0x000F)
        {
        case 0x0000:
            V[(opcode & 0x0F00) >> 8] = V[(opcode & 0x00F0) >> 4];
            PC += 2;
            break;

        case 0x0001:
            V[(opcode & 0x0F00) >> 8] = V[(opcode & 0x0F00) >> 8] | V[(opcode & 0x00F0) >> 4];
            PC += 2;
            break;

        case 0x0002:
            V[(opcode & 0x0F00) >> 8] = V[(opcode & 0x0F00) >> 8] & V[(opcode & 0x00F0) >> 4];
            PC += 2;
            break;

        case 0x0003:
            V[(opcode & 0x0F00) >> 8] = V[(opcode & 0x0F00) >> 8] ^ V[(opcode & 0x00F0) >> 4];
            PC += 2;
            break;

        case 0x0004:
            if (V[(opcode & 0x00F0) >> 4] > (0xFF - V[(opcode & 0x0F00) >> 8]))
                V[0xF] = 1;
            else
                V[0xF] = 0;
            V[(opcode & 0x0F00) >> 8] += V[(opcode & 0x00F0) >> 4];
            PC += 2;
            break;

        case 0x0005:
            if (V[(opcode & 0x00F0) >> 4] > V[(opcode & 0x0F00) >> 8])
                V[0xF] = 1;
            else
                V[0xF] = 0;
            V[(opcode & 0x0F00) >> 8] -= V[(opcode & 0x00F0) >> 4];
            PC += 2;
            break;

        case 0x0006:
            V[0xF] = V[(opcode & 0x0F00) >> 8] & 0x1;
            V[(opcode & 0x0F00) >> 8] >>= 1;
            PC += 2;
            break;

        case 0x0007:
            if (V[(opcode & 0x0F00) >> 8] > V[(opcode & 0x00F0) >> 4])
                V[0xF] = 1;
            else
                V[0xF] = 0;
            V[(opcode & 0x0F00) >> 8] = V[(opcode & 0x00F0) >> 4] - V[(opcode & 0x0F00) >> 8];
            PC += 2;
            break;

        case 0x000E:
            V[0xF] = V[(opcode & 0x0F00) >> 8] >> 7;
            V[(opcode & 0x0F00) >> 8] <<= 1;
            PC += 2;
            break;

        default:
            printf("Unknown opcode [0x8000]: 0x%X\n", opcode);
        }

    case 0x9000:
        if (V[(opcode & 0x0F00) >> 8] != V[(opcode & 0x00F0) >> 4])
            PC += 4;
        else
            PC += 2;

    case 0xA000:
        I = opcode & 0x0FFF;
        PC += 2;
        break;

    case 0xB000:
        PC = V[0] + (opcode & 0x0FFF);
        break;

    case 0xC000:
        V[(opcode & 0x0F00) >> 8] = (std::rand() % 0xFF) & (opcode & 0x00FF);
        PC += 2;
        break;

    case 0xD000:
        //	Draws a sprite at coordinate
        break;

    case 0xE000:
        switch (opcode & 0x000F)
        {
        case 0x000E:
            if (key[V[(opcode & 0x0F00) >> 8]] != 0)
                PC += 4;
            else
                PC += 2;
            break;

        case 0x0001:
            if (key[V[(opcode & 0x0F00) >> 8]] == 0)
                PC += 4;
            else
                PC += 2;
            break;

        default:
            printf("Unknown opcode: 0x%X\n", opcode);
        }

    case 0xF000:
        switch (opcode & 0x00FF)
        {
        case 0x0007:
            V[(opcode & 0x0F00) >> 8] = delay_timer;
            PC += 2;
            break;

        case 0x000A:
        {
            bool keyPress = false;
            for (int i = 0; i < 16; ++i)
            {
                if (key[i] != 0)
                {
                    V[(opcode & 0x0F00) >> 8] = i;
                    keyPress = true;
                }
            }
            if (!keyPress)
                return;
            PC += 2;
        }
        break;

        case 0x0015:
            delay_timer = V[(opcode & 0x0F00) >> 8];
            PC += 2;
            break;

        case 0x0018:
            sound_timer = V[(opcode & 0x0F00) >> 8];
            PC += 2;
            break;

        case 0x001E:
            I += V[(opcode & 0x0F00) >> 8];
            PC += 2;
            break;

        case 0x0029:
            I = V[(opcode & 0x0F00) >> 8] * 0x5;
            PC += 2;
            break;

        case 0x0033:
            memory[I] = V[(opcode & 0x0F00) >> 8] % 100;
            memory[I + 1] = (V[(opcode & 0x0F00) >> 8] - memory[I] * 100) % 10;
            memory[I + 2] = V[(opcode & 0x0F00) >> 8] - memory[I + 1] * 10;
            break;

        case 0x0055:
            break;

        case 0x0065:
            break;

        default:
            printf("Unknown opcode: 0x%X\n", opcode);
        }

    default:
        printf("Unknown opcode: 0x%X\n", opcode);

        if (delay_timer > 0)
            --delay_timer;

        if (sound_timer > 0)
        {
            if (sound_timer == 1)
                printf("BEEP!\n");
            --sound_timer;
        }
    }
}