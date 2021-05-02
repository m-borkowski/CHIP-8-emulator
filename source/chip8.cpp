#include "chip8.h"
#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <time.h>

emulator::chip8::chip8()
{
    //pass
}

emulator::chip8::~chip8()
{
    //pass
}

void emulator::chip8::initialize()
{
    PC = 0x200; // start program counter at 0x200
    opcode = 0; // reset current opcode
    I = 0;      // reset index register
    sp = 0;     // resert stack pointer
    delay_timer = 0;
    sound_timer = 0;
    drawFlag = true;

    for (int i = 0; i < 2048; ++i) // clear display
        gfx[i] = 0;

    for (int i = 0; i < 16; ++i) // clear stack
        stack[i] = 0;

    for (int i = 0; i < 16; ++i) // clear keys
        key[i] = 0;

    for (int i = 0; i < 16; ++i) // clear registers V0-VF
        V[i] = 0;

    for (int i = 0; i < 4096; ++i) // clear memory
        memory[i] = 0;

    for (int i = 0; i < 80; ++i) // load fontset
        memory[i] = chip8_fontset[i];

    srand(time(NULL));
}

bool emulator::chip8::load(const char *filepath)
{
    initialize();
    FILE *pFile = fopen(filepath, "rb");

    if (pFile == NULL)
    {
        fputs("File error", stderr);
        return false;
    }

    fseek(pFile, 0L, SEEK_END);
    long bufferSize = ftell(pFile); // get file size
    fseek(pFile, 0L, SEEK_SET);     // go back to the beginning

    char *buffer = (char *)malloc(sizeof(char) * bufferSize); // allocate the read buf
    if (buffer == NULL)
    {
        fputs("Memory error", stderr);
        return false;
    }

    int result = fread(buffer, 1, bufferSize, pFile);
    if (result != bufferSize)
    {
        fputs("Reading error", stderr);
        return false;
    }

    for (int i = 0; i < bufferSize; ++i)
    {
        memory[i + 512] = buffer[i];
    }
    fclose(pFile);
    free(buffer);
    return true;
}

void emulator::chip8::emulateCycle()
{
    opcode = memory[PC] << 8 | memory[PC + 1];

    switch (opcode & 0xF000)
    {
    case 0x0000:
        switch (opcode & 0x000F)
        {
        case 0x0000: // 0x00E0: Clear the screen
            for (int i = 0; i < 2048; ++i)
                gfx[i] = 0x0;
            drawFlag = true;
            PC += 2;
            break;

        case 0x000E: // 0x00EE: Return from subroutine
            --sp;
            PC = stack[sp];
            PC += 2;
            break;

        default:
            printf("Unknown opcode [0x0000]: 0x%X\n", opcode);
        }
        break;

    case 0x1000: // 1NNN:: Jump to address NNN
        PC = opcode & 0x0FFF;
        break;

    case 0x2000: // 2NNN:: Call subroutine at NNN
        stack[sp] = PC;
        ++sp;
        PC = opcode & 0x0FFF;
        break;

    case 0x3000: // 3XNN:: Skip the next instruction if VX equals NN
        if (V[(opcode & 0x0F00) >> 8] == (opcode & 0x00FF))
            PC += 4;
        else
            PC += 2;
        break;

    case 0x4000: // 4XNN:: Skip the next instruction if VX does not equal NN
        if (V[(opcode & 0x0F00) >> 8] != (opcode & 0x00FF))
            PC += 4;
        else
            PC += 2;
        break;

    case 0x5000: // 5XY0:: Skip the next instruction if VX equals VY
        if (V[(opcode & 0x0F00) >> 8] == V[(opcode & 0x00F0) >> 4])
            PC += 4;
        else
            PC += 2;
        break;

    case 0x6000: // 6XNN:: Set VX to NN
        V[(opcode & 0x0F00) >> 8] = opcode & 0x00FF;
        PC += 2;
        break;

    case 0x7000: // 7XNN:: 	Add NN to VX
        V[(opcode & 0x0F00) >> 8] += opcode & 0x00FF;
        PC += 2;
        break;

    case 0x8000:
        switch (opcode & 0x000F)
        {
        case 0x0000: // 8XY0:: Set VX to the value of VY
            V[(opcode & 0x0F00) >> 8] = V[(opcode & 0x00F0) >> 4];
            PC += 2;
            break;

        case 0x0001: // 8XY1:: Set VX = VX OR VY
            V[(opcode & 0x0F00) >> 8] = V[(opcode & 0x0F00) >> 8] | V[(opcode & 0x00F0) >> 4];
            PC += 2;
            break;

        case 0x0002: // 8XY2: Set VX = VX AND VY
            V[(opcode & 0x0F00) >> 8] = V[(opcode & 0x0F00) >> 8] & V[(opcode & 0x00F0) >> 4];
            PC += 2;
            break;

        case 0x0003: // 8XY3: Set VX = VX XOR VY
            V[(opcode & 0x0F00) >> 8] = V[(opcode & 0x0F00) >> 8] ^ V[(opcode & 0x00F0) >> 4];
            PC += 2;
            break;

        case 0x0004: // 8XY4: Set VX = VX + VY, set VF = carry
            if (V[(opcode & 0x00F0) >> 4] > (0xFF - V[(opcode & 0x0F00) >> 8]))
                V[0xF] = 1;
            else
                V[0xF] = 0;
            V[(opcode & 0x0F00) >> 8] += V[(opcode & 0x00F0) >> 4];
            PC += 2;
            break;

        case 0x0005: // 8XY5: Set VX = VX - VY, set VF = NOT borrow.
            if (V[(opcode & 0x00F0) >> 4] > V[(opcode & 0x0F00) >> 8])
                V[0xF] = 0;
            else
                V[0xF] = 1;
            V[(opcode & 0x0F00) >> 8] -= V[(opcode & 0x00F0) >> 4];
            PC += 2;
            break;

        case 0x0006: // 8XY5: Store the least significant bit of VX in VF and then shift VX to the right by 1
            V[0xF] = V[(opcode & 0x0F00) >> 8] & 0x1;
            V[(opcode & 0x0F00) >> 8] >>= 1;
            PC += 2;
            break;

        case 0x0007: // 8XY6: Set VX = VY - VX, set VF = NOT borrow
            if (V[(opcode & 0x0F00) >> 8] > V[(opcode & 0x00F0) >> 4])
                V[0xF] = 0;
            else
                V[0xF] = 1;
            V[(opcode & 0x0F00) >> 8] = V[(opcode & 0x00F0) >> 4] - V[(opcode & 0x0F00) >> 8];
            PC += 2;
            break;

        case 0x000E: //  8XYE: Store the most significant bit of VX in VF and then shift VX to the left by 1
            V[0xF] = V[(opcode & 0x0F00) >> 8] >> 7;
            V[(opcode & 0x0F00) >> 8] <<= 1;
            PC += 2;
            break;

        default:
            printf("Unknown opcode [0x8000]: 0x%X\n", opcode);
        }
        break;

    case 0x9000: // 9XY0: Skip next instruction if VX != VY
        if (V[(opcode & 0x0F00) >> 8] != V[(opcode & 0x00F0) >> 4])
            PC += 4;
        else
            PC += 2;
        break;

    case 0xA000: // ANNN: Set I = NNN
        I = opcode & 0x0FFF;
        PC += 2;
        break;

    case 0xB000: // BNNN: Jump to location NNN + V0
        PC = V[0] + (opcode & 0x0FFF);
        break;

    case 0xC000: // CXNN: Set VX to the result of a bitwise and operation on a random number and NN
        V[(opcode & 0x0F00) >> 8] = (rand() % 0xFF) & (opcode & 0x00FF);
        PC += 2;
        break;

    case 0xD000: // DXYN: Draw a sprite at coordinate (VX, VY) that has a width of 8 pixels and a height of N+1 pixels
    {
        unsigned short x = V[(opcode & 0x0F00) >> 8];
        unsigned short y = V[(opcode & 0x00F0) >> 4];
        unsigned short height = opcode & 0x000F;
        unsigned short pixels;

        V[0xF] = 0;
        for (int row = 0; row < height; ++row)
        {
            pixels = memory[I + row];
            for (int col = 0; col < 8; ++col)
            {
                if ((pixels & (0x80 >> col)) != 0)
                {
                    if (gfx[x + row + ((y + col) * 64)] == 1)
                        V[0xF] = 1;
                    gfx[x + row + ((y + col) * 64)] ^= 1;
                }
            }
        }
        drawFlag = true;
        PC += 2;
    }
    break;

    case 0xE000:
        switch (opcode & 0x00FF)
        {
        case 0x009E: // EX9E: Skip next instruction if key with the value of VX is pressed

            if (key[V[(opcode & 0x0F00) >> 8]] != 0)
                PC += 4;
            else
                PC += 2;
            break;

        case 0x00A1: // EXA1: Skip next instruction if key with the value of VX is not pressed
            if (key[V[(opcode & 0x0F00) >> 8]] == 0)
                PC += 4;
            else
                PC += 2;
            break;

        default:
            printf("Unknown opcode [0xE000]: 0x%X\n", opcode);
        }
        break;

    case 0xF000:
        switch (opcode & 0x00FF)
        {
        case 0x0007: // FX07 : Set VX = delay timer value
            V[(opcode & 0x0F00) >> 8] = delay_timer;
            PC += 2;
            break;

        case 0x000A: // FX0A : Wait for a key press, store the value of the key in VX
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

        case 0x0015: // FX15: Set delay timer = VX
            delay_timer = V[(opcode & 0x0F00) >> 8];
            PC += 2;
            break;

        case 0x0018: // FX18: Set sound timer = VX
            sound_timer = V[(opcode & 0x0F00) >> 8];
            PC += 2;
            break;

        case 0x001E: // FX1E: Set I = I + VX
            if ((V[(opcode & 0x0F00) >> 8] + I) > 0xFFF)
                V[0xF] = 1;
            else
                V[0xF] = 0;
            I += V[(opcode & 0x0F00) >> 8];
            PC += 2;
            break;

        case 0x0029: // FX29: Set I = location of sprite for digit VX
            I = V[(opcode & 0x0F00) >> 8] * 0x5;
            PC += 2;
            break;

        case 0x0033: // FX33: Store BCD representation of VX in memory locations I, I+1, and I+2
            memory[I] = V[(opcode & 0x0F00) >> 8] % 100;
            memory[I + 1] = (V[(opcode & 0x0F00) >> 8] - memory[I] * 100) % 10;
            memory[I + 2] = V[(opcode & 0x0F00) >> 8] - memory[I + 1] * 10;
            PC += 2;
            break;

        case 0x0055: // FX55: Store registers V0 through VX in memory starting at location I
            for (int i = 0; i <= ((opcode & 0x0F00) >> 8); ++i)
                memory[I + i] = V[i];
            // On the original interpreter, when the operation is done, I = I + X + 1.
            I += ((opcode & 0x0F00) >> 8) + 1;
            PC += 2;
            break;

        case 0x0065: // FX65: Read registers V0 through VX from memory starting at location I
            for (int i = 0; i <= ((opcode & 0x0F00) >> 8); ++i)
                V[i] = memory[I + i];

            // On the original interpreter, when the operation is done, I = I + X + 1.
            I += ((opcode & 0x0F00) >> 8) + 1;
            PC += 2;
            break;

        default:
            printf("Unknown opcode [0xF000]: 0x%X\n", opcode);
        }
        break;

    default:
        printf("Unknown opcode: 0x%X\n", opcode);
    }
    if (delay_timer > 0)
        --delay_timer;

    if (sound_timer > 0)
    {
        if (sound_timer == 1)
            printf("BEEP!\n");
        --sound_timer;
    }
}
