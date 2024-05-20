#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "core.h"

void initialize_chip8(Chip8core *core)
{

    unsigned char fontset[80] =
        {
            0xF0, 0x90, 0x90, 0x90, 0xF0, // 0
            0x20, 0x60, 0x20, 0x20, 0x70, // 1
            0xF0, 0x10, 0xF0, 0x80, 0xF0, // 2
            0xF0, 0x10, 0xF0, 0x10, 0xF0, // 3
            0x90, 0x90, 0xF0, 0x10, 0x10, // 4
            0xF0, 0x80, 0xF0, 0x10, 0xF0, // 5
            0xF0, 0x80, 0xF0, 0x90, 0xF0, // 6
            0xF0, 0x10, 0x20, 0x40, 0x40, // 7
            0xF0, 0x90, 0xF0, 0x90, 0xF0, // 8
            0xF0, 0x90, 0xF0, 0x10, 0xF0, // 9
            0xF0, 0x90, 0xF0, 0x90, 0x90, // A
            0xE0, 0x90, 0xE0, 0x90, 0xE0, // B
            0xF0, 0x80, 0x80, 0x80, 0xF0, // C
            0xE0, 0x90, 0x90, 0x90, 0xE0, // D
            0xF0, 0x80, 0xF0, 0x80, 0xF0, // E
            0xF0, 0x80, 0xF0, 0x80, 0x80  // F
        };

    core->pc = 0x200;
    core->opcode = 0;
    core->I = 0;
    core->sp = 0;

    memset(core->gfx, 0, sizeof(core->gfx));
    memset(core->V, 0, sizeof(core->V));
    memset(core->stack, 0, sizeof(core->stack));
    memset(core->keys, 0, sizeof(core->keys));
    memset(core->memory, 0, sizeof(core->memory));

    for (int i = 0; i < 80; i++)
    {
        core->memory[i] = fontset[i];
    }

    core->sound_timer = 0;
    core->delay_timer = 0;
    core->drawflag = 1;
}

void load_rom_chip8(char filepath[], Chip8core *core)
{
    FILE *fptr = fopen(filepath, "rb");
    if (fptr == NULL)
    {
        printf("Could not open file.\n");
        return;
    }

    fseek(fptr, 0L, SEEK_END);
    int sz = ftell(fptr);
    rewind(fptr);

    char buf[sz];

    fread(buf, sizeof(char), sz, fptr);

    for (int i = 0; i < sz; i++)
    {
        core->memory[i + 0x200] = buf[i];
    }

    fclose(fptr);
}

void run_cycle_chip8(Chip8core *core)
{
    unsigned short opcode = core->memory[core->pc] << 8 | core->memory[core->pc + 1];
    core->pc += 2;

    unsigned char X = (opcode & 0x0F00) >> 8;
    unsigned char Y = (opcode & 0x00F0) >> 4;
    unsigned char N = opcode & 0x000F;
    unsigned char NN = opcode & 0x00FF;
    unsigned short NNN = opcode & 0x0FFF;

    switch (opcode & 0xF000)
    {
    case 0x0000:
        switch (N)
        {
        case 0x0000:
            for (int i = 0; i < 64 * 32; i++)
            {
                core->gfx[i] = 0;
            }
            core->drawflag = 1;
            break;

        case 0x000E:
            core->pc = core->stack[core->sp];
            core->sp--;
            break;
        }
        break;

    case 0x1000:
        core->pc = NNN;
        break;

    case 0x2000:
        core->stack[++core->sp] = core->pc;
        core->pc = NNN;
        break;

    case 0x3000:
        if (core->V[X] == NN)
        {
            core->pc += 2;
        }
        break;

    case 0x4000:
        if (core->V[X] != NN)
        {
            core->pc += 2;
        }
        break;

    case 0x5000:
        if (core->V[X] == core->V[Y])
        {
            core->pc += 2;
        }
        break;

    case 0x6000:
        core->V[X] = NN;
        break;

    case 0x7000:
        core->V[X] += NN;
        break;

    case 0x8000:
        int flag;
        switch (N)
        {
        case 0x0000:
            core->V[X] = core->V[Y];
            break;

        case 0x0001:
            core->V[X] = core->V[X] | core->V[Y];
            core->V[0xF] = 0;
            break;

        case 0x0002:
            core->V[X] = core->V[X] & core->V[Y];
            core->V[0xF] = 0;
            break;

        case 0x0003:
            core->V[X] = core->V[X] ^ core->V[Y];
            core->V[0xF] = 0;
            break;

        case 0x0004:
            flag = core->V[Y] > 0xFF - core->V[X];
            core->V[X] = core->V[X] + core->V[Y];
            core->V[0xF] = flag;
            break;

        case 0x0005:
            flag = core->V[X] > core->V[Y];
            core->V[X] = core->V[X] - core->V[Y];
            core->V[0xF] = flag;
            break;

        case 0x0006:
            core->V[X] = core->V[Y];
            flag = core->V[X] & 0x1;
            core->V[X] >>= 1;
            core->V[0xF] = flag;
            break;

        case 0x0007:
            flag = core->V[Y] > core->V[X];
            core->V[X] = core->V[Y] - core->V[X];
            core->V[0xF] = flag;
            break;

        case 0x000E:
            core->V[X] = core->V[Y];
            flag = core->V[X] >> 7;
            core->V[X] <<= 1;
            core->V[0xF] = flag;
            break;
        }
        break;

    case 0x9000:
        if (core->V[X] != core->V[Y])
        {
            core->pc += 2;
        }
        break;

    case 0xA000:
        core->I = NNN;
        break;

    case 0xB000:
        core->pc = core->V[0] + NNN;
        break;

    case 0xC000:
        core->V[X] = (rand() % 256) & NN;
        break;

    case 0xD000:
        unsigned short xcoord = core->V[X] % SCREEN_WIDTH;
        unsigned short ycoord = core->V[Y] % SCREEN_HEIGHT;
        core->V[0xF] = 0;
        unsigned short pixel;

        for (int yline = 0; yline < N; yline++)
        {
            pixel = core->memory[core->I + yline];

            for (int xline = 0; xline < 8; xline++)
            {
                int xpos = xcoord + xline;
                int ypos = (ycoord + yline);

                if ((xpos >= SCREEN_WIDTH) || (ypos >= SCREEN_HEIGHT))
                {
                    break;
                }

                if ((pixel & (0x80 >> xline)) != 0)
                {
                    if (core->gfx[xpos + ypos * SCREEN_WIDTH] == 1)
                    {
                        core->V[0xF] = 1;
                    }
                    core->gfx[xpos + ypos * SCREEN_WIDTH] ^= 1;
                }
            }
        }

        core->drawflag = 1;
        break;

    case 0xE000:
        switch (opcode & 0x00F0)
        {
        case 0x0090:
            if (core->keys[core->V[X]] != 0)
            {
                core->pc += 2;
            }
            break;

        case 0x00A0:
            if (core->keys[core->V[X]] == 0)
            {
                core->pc += 2;
            }
            break;
        }
        break;

    case 0xF000:
        switch (opcode & 0x00FF)
        {
        case 0x0007:
            core->V[X] = core->delay_timer;
            break;

        case 0x000A:
            unsigned int keypressed = 0;

            for (int i = 0; i < 16; i++)
            {
                if (core->keys[i] != 0)
                {
                    core->V[X] = i;
                    keypressed = 1;
                    break;
                }
            }

            if (keypressed == 0)
            {
                core->pc -= 2;
            }
            break;

        case 0x0015:
            core->delay_timer = core->V[X];
            break;

        case 0x0018:
            core->sound_timer = core->V[X];
            break;

        case 0x001E:
            core->I = core->I + core->V[X];
            break;

        case 0x0029:
            core->I = core->V[X] * 5;
            break;

        case 0x0033:
            core->memory[core->I] = core->V[X] / 100;
            core->memory[core->I + 1] = (core->V[X] / 10) % 10;
            core->memory[core->I + 2] = core->V[X] % 10;
            break;

        case 0x0055:
            for (int i = 0; i <= X; i++)
            {
                core->memory[core->I++] = core->V[i];
            }
            break;

        case 0x0065:
            for (int i = 0; i <= X; i++)
            {
                core->V[i] = core->memory[core->I++];
            }
            break;
        }
        break;

    default:
        printf("Unknown opcode: %x\n", opcode);
        break;
    }
}