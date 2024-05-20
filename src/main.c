#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "raylib.h"
#include "config.h"
#include "core.h"
#include "embed.c"

int main()
{
    Chip8core *emulator_core = (Chip8core *)malloc(sizeof(Chip8core));

    initialize_chip8(emulator_core);
    int paused = 1;
    int rom_loaded = 0;

    InitWindow(SCREEN_WIDTH * SCALE_FACTOR, SCREEN_HEIGHT * SCALE_FACTOR, "Chippy");
    InitAudioDevice();
    SetTargetFPS(60);
    Wave beep_wave = LoadWaveFromMemory(".mp3", beep_mp3_data, beep_mp3_data_size);
    Sound sfx_beep = LoadSoundFromWave(beep_wave);

    int keypad[] = {KEY_X, KEY_ONE, KEY_TWO, KEY_THREE, KEY_Q, KEY_W, KEY_E, KEY_A, KEY_S, KEY_D, KEY_Z, KEY_C, KEY_FOUR, KEY_R, KEY_F, KEY_V};

    while (!WindowShouldClose())
    {
        if (IsFileDropped())
        {
            FilePathList droppedFiles = LoadDroppedFiles();

            if ((droppedFiles.count > 0) && (IsFileExtension(droppedFiles.paths[0], ".ch8") || IsFileExtension(droppedFiles.paths[0], ".rom")))
            {
                load_rom_chip8(droppedFiles.paths[0], emulator_core);
                rom_loaded = 1;
                paused = 0;
            }
            UnloadDroppedFiles(droppedFiles);
        }

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
        if (rom_loaded)
        {
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
        }
        else
        {
            DrawText("Drag and drop a ROM into the window.", 0, 0, 24, WHITE);
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