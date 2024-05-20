#ifndef CORE_H
#define CORE_H

#define SCREEN_WIDTH 64
#define SCREEN_HEIGHT 32

struct Chip8core
{
    unsigned short opcode;
    unsigned char memory[4096];
    unsigned char V[16];
    unsigned short pc;
    unsigned short I;
    unsigned char gfx[64 * 32];
    unsigned char sound_timer;
    unsigned char delay_timer;
    unsigned short stack[16];
    unsigned short sp;
    unsigned char keys[16];
    unsigned int drawflag;
};

typedef struct Chip8core Chip8core;

void initialize_chip8(Chip8core *core);
void load_rom_chip8(char filepath[], Chip8core *core);
void run_cycle_chip8(Chip8core *core);

#endif