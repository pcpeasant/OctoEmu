#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "raylib.h"
#include "config.h"

#define SCREEN_WIDTH 64
#define SCREEN_HEIGHT 32

unsigned short opcode;
unsigned char memory[4096];
unsigned char V[16];
unsigned short pc;
unsigned short I;
unsigned char gfx[64*32];
unsigned char sound_timer;
unsigned char delay_timer;
unsigned short stack[16];
unsigned short sp;
unsigned char keys[16];
unsigned int drawflag = 0;

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

void initialize_chip8()
{
    pc = 0x200;
    opcode = 0;
    I = 0;
    sp = 0;

    memset(gfx, 0, sizeof(gfx));
    memset(V, 0, sizeof(V));
    memset(stack, 0, sizeof(stack));
    memset(keys, 0, sizeof(keys));
    memset(memory, 0, sizeof(memory));

    for(int i = 0; i < 80; i++)
    {
        memory[i] = fontset[i];
    }   

    sound_timer = 0;
    delay_timer = 0;
    drawflag = 1;
}

void load_rom(char filepath[])
{
    FILE* fptr = fopen(filepath, "rb");
    if(fptr == NULL)
    {
        printf("Could not open file.\n");
        return;
    }

    fseek(fptr, 0L, SEEK_END);
    int sz = ftell(fptr);
    rewind(fptr);

    char buf[sz];

    fread(buf, sizeof(char), sz, fptr);

    for(int i = 0; i < sz; i++)
    {
        memory[i + 0x200] = buf[i];
    }

    fclose(fptr);
}

void emulate_cycle()
{
    opcode = memory[pc] << 8 | memory[pc + 1];
    pc += 2;

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
                for(int i = 0; i < 64*32; i++)
                {
                gfx[i] = 0;
                }
                drawflag = 1;
                break;
            
            case 0x000E:
                pc = stack[sp];
                sp--;
                break;
        }
        break;
    
    case 0x1000:
        pc = NNN;
        break;
    
    case 0x2000:
        stack[++sp] = pc;
        pc = NNN;
        break;
    
    case 0x3000:
        if(V[X] == NN)
        {
            pc += 2;
        }
        break;
    
    case 0x4000:
        if(V[X] != NN)
        {
            pc += 2;
        }
        break;
    
    case 0x5000:
        if(V[X] == V[Y])
        {
            pc += 2;
        }
        break;
    
    case 0x6000:
        V[X] = NN;
        break;
    
    case 0x7000:
        V[X] += NN;
        break;
    
    case 0x8000:
        int flag;
        switch (N)
        {
            case 0x0000:
                V[X] = V[Y];
                break;
            
            case 0x0001:
                V[X] = V[X] | V[Y];
                V[0xF] = 0;
                break;
            
            case 0x0002:
                V[X] = V[X] & V[Y];
                V[0xF] = 0;
                break;
            
            case 0x0003:
                V[X] = V[X] ^ V[Y];
                V[0xF] = 0;
                break;
            
            case 0x0004:
                flag = V[Y] > 0xFF - V[X];
                V[X] = V[X] + V[Y];
                V[0xF] = flag;
                break;
            
            case 0x0005:
                flag = V[X] > V[Y];
                V[X] = V[X] - V[Y];
                V[0xF] = flag;
                break;
            
            case 0x0006:
                V[X] = V[Y];
                flag = V[X] & 0x1;
                V[X] >>= 1;
                V[0xF] = flag;
                break;
            
            case 0x0007:
                flag = V[Y] > V[X];
                V[X] = V[Y] - V[X];
                V[0xF] = flag;
                break;
            
            case 0x000E:
                V[X] = V[Y];
                flag = V[X] >> 7;
                V[X] <<= 1;
                V[0xF] = flag;
                break;
        }
        break;
    
    case 0x9000:
        if(V[X] != V[Y])
        {
            pc += 2;
        }
        break;
    
    case 0xA000:
        I = NNN;
        break;
    
    case 0xB000:
        pc = V[0] + NNN;
        break;
    
    case 0xC000:
        V[X] = (rand() % 256) & NN;
        break;
    
    case 0xD000:
        unsigned short xcoord = V[X] % SCREEN_WIDTH;
        unsigned short ycoord = V[Y] % SCREEN_HEIGHT;
        V[0xF] = 0;
        unsigned short pixel;

        for(int yline = 0; yline < N; yline++)
        {
            pixel = memory[I + yline];

            for(int xline = 0; xline < 8 ; xline++)
            {
                int xpos = xcoord + xline;
                int ypos = (ycoord + yline);

                if((xpos >= SCREEN_WIDTH) || (ypos >= SCREEN_HEIGHT))
                {
                    break;
                }

                if((pixel & (0x80 >> xline)) != 0)
                {
                    if(gfx[xpos + ypos*SCREEN_WIDTH] == 1)
                    {
                        V[0xF] = 1;
                    }
                    gfx[xpos + ypos*SCREEN_WIDTH] ^= 1;
                }
            }
        }

        drawflag = 1;
        break;
    
    case 0xE000:
        switch (opcode & 0x00F0)
        {
        case 0x0090:
            if(keys[V[X]] != 0)
            {
                pc += 2;
            }
            break;
        

        case 0x00A0:
            if(keys[V[X]] == 0)
            {
                pc += 2;
            }
            break;
        }
    
    case 0xF000:
        switch (opcode & 0x00FF)
        {
        case 0x0007:
            V[X] = delay_timer;
            break;
        
        case 0x000A:
            unsigned int keypressed = 0;

            for(int i = 0; i < 16; i++)
            {
                if(keys[i] != 0)
                {
                    V[X] = i;
                    keypressed = 1;
                    break;
                }
            }

            if(keypressed == 0)
            {
                pc -= 2;
            }
            break;
        
        case 0x0015:
            delay_timer = V[X];
            break;
        
        case 0x0018:
            sound_timer = V[X];
            break;
        
        case 0x001E:
            I = I + V[X];
            break;
        
        case 0x0029:
            I = V[X] * 5;
            break;
        
        case 0x0033:
            memory[I] = V[X] / 100;
            memory[I+1] = (V[X]/10) % 10;
            memory[I+2] = V[X] % 10;
            break;
        
        case 0x0055:
            for(int i = 0; i <= X; i++)
            {
                memory[I++] = V[i];
            }
            break;
        
        case 0x0065:
            for(int i = 0; i <= X; i++)
            {
                V[i] = memory[I++];
            }
            break;
        }
        break;

    default:
        printf("Unknown opcode: %x\n", opcode);
        break;
    }
}

int main(int argc, char** argv)
{
    FilePathList romlist = LoadDirectoryFiles("./roms");
    for(int i = 0; i<romlist.count; i++)
    {
        printf("%d: %s\n", i+1, GetFileNameWithoutExt(romlist.paths[i]));
    }

    int choice;
    printf("Enter the rom number to load: \n");
    scanf("%d", &choice);

    initialize_chip8();
    load_rom(romlist.paths[choice-1]);
    bool running = 1;
    bool paused = 0;


    UnloadDirectoryFiles(romlist);

    InitWindow(SCREEN_WIDTH*SCALE_FACTOR, SCREEN_HEIGHT*SCALE_FACTOR, "Chippy");
    InitAudioDevice();
    SetTargetFPS(60);
    Sound sfx_beep = LoadSound("./data/beep.wav");

    int keypad[] = {KEY_X, KEY_ONE, KEY_TWO, KEY_THREE, KEY_Q, KEY_W, KEY_E, KEY_A, KEY_S, KEY_D, KEY_Z, KEY_C, KEY_FOUR, KEY_R, KEY_F, KEY_V};

    while(running)
    {
        BeginDrawing();
        if(!paused)
        {
            for(int i = 0; i<IPF; i++)
            {
                emulate_cycle();
            }

            if(delay_timer > 0)
            {
                delay_timer--;
            }

            if(sound_timer > 0)
            {
                if(!IsSoundPlaying(sfx_beep))
                {
                    PlaySound(sfx_beep);
                }
                sound_timer--;
            }
        }

        if(drawflag)
        { 
            ClearBackground(BLACK);

            for(int y = 0; y < SCREEN_HEIGHT; y++)
            {
                for(int x = 0; x < SCREEN_WIDTH; x++)
                {
                    if(gfx[y*SCREEN_WIDTH + x] == 1)
                    {
                        DrawRectangle(x*SCALE_FACTOR, y*SCALE_FACTOR, SCALE_FACTOR, SCALE_FACTOR, WHITE);
                    }
                }
            }
            drawflag = 0;
        }
        
        for(int i = 0; i<16; i++)
        {
            if(IsKeyPressed(keypad[i]))
            {
                keys[i] = 1;
            }
            else if(IsKeyReleased(keypad[i]))
            {
                keys[i] = 0;
            }
        }

        if(IsKeyPressed(KEY_SPACE))
        {
            paused = !paused;
        }
        if(WindowShouldClose())
        {
            running = 0;
        }
        EndDrawing();
    }
    
    UnloadSound(sfx_beep);
    CloseWindow();
    CloseAudioDevice();
}