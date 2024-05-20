#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "raylib.h"
#include "config.h"
#include "core.h"

int main()
{
    FilePathList romlist = LoadDirectoryFiles("../roms");
    for (unsigned int i = 0; i < romlist.count; i++)
    {
        printf("%d: %s\n", i + 1, GetFileNameWithoutExt(romlist.paths[i]));
    }

    int choice;
    printf("Enter the rom number to load: \n");
    scanf("%d", &choice);

    Chip8core *emulator_core = (Chip8core *)malloc(sizeof(Chip8core));

    initialize_chip8(emulator_core);
    load_rom_chip8(romlist.paths[choice - 1], emulator_core);
    int paused = 0;

    UnloadDirectoryFiles(romlist);

    InitWindow(SCREEN_WIDTH * SCALE_FACTOR, SCREEN_HEIGHT * SCALE_FACTOR, "Chippy");
    InitAudioDevice();
    SetTargetFPS(60);
    Sound sfx_beep = LoadSound("../data/beep.wav");

    int keypad[] = {KEY_X, KEY_ONE, KEY_TWO, KEY_THREE, KEY_Q, KEY_W, KEY_E, KEY_A, KEY_S, KEY_D, KEY_Z, KEY_C, KEY_FOUR, KEY_R, KEY_F, KEY_V};

    while (!WindowShouldClose())
    {
        if (!paused)
        {
            for (int i = 0; i < 16; i++)
            {
                if (IsKeyPressed(keypad[i]))
                {
                    emulator_core->keys[i] = 1;
                }
                else if (IsKeyReleased(keypad[i]))
                {
                    emulator_core->keys[i] = 0;
                }
            }

            for (int i = 0; i < IPF; i++)
            {
                run_cycle_chip8(emulator_core);
            }

            if (emulator_core->delay_timer > 0)
            {
                emulator_core->delay_timer--;
            }

            if (emulator_core->sound_timer > 0)
            {
                if (!IsSoundPlaying(sfx_beep))
                {
                    PlaySound(sfx_beep);
                }
                emulator_core->sound_timer--;
            }
        }

        BeginDrawing();
        ClearBackground(BLACK);
        for (int y = 0; y < SCREEN_HEIGHT; y++)
        {
            for (int x = 0; x < SCREEN_WIDTH; x++)
            {
                if (emulator_core->gfx[y * SCREEN_WIDTH + x] == 1)
                {
                    DrawRectangle(x * SCALE_FACTOR, y * SCALE_FACTOR, SCALE_FACTOR, SCALE_FACTOR, WHITE);
                }
            }
        }
        EndDrawing();

        if (IsKeyPressed(KEY_SPACE))
        {
            paused = !paused;
        }
    }

    UnloadSound(sfx_beep);
    CloseWindow();
    CloseAudioDevice();
}