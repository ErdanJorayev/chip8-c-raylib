#include <raylib.h>
#include "chip8.h"
#include <stdio.h>
#include <time.h>
#include <stdlib.h>

const int SCALE = 12; // Масштаб отображения одного пикселя
const int screen_width = 64 * SCALE;
const int screen_height = 32 * SCALE;

void WorkChip8(Chip8 * ch, char * games_lists[], unsigned int total_games);
int SelectGame(char * games_lists[], unsigned int total_games);

int main() 
{
    srand(time(NULL));

    Chip8 chip8;
    char * games_lists[100];
    unsigned int total = GetCh8Games(games_lists, 100);

    InitWindow(screen_width, screen_height, "C + Raylib CHIP-8 Emulator");
    SetTargetFPS(60); 
    SetExitKey(KEY_NULL); 

    int selected_game = SelectGame(games_lists, total);

    if (selected_game == -1) 
    {
        for (unsigned int i = 0; i < total; i++) 
            free(games_lists[i]);
        CloseWindow();
        return 0;
    }

    Chip8_Init(&chip8);
    Chip8_LoadROM(&chip8, games_lists[selected_game]);

    WorkChip8(&chip8, games_lists, total); 

    for (unsigned int i = 0; i < total; i++) 
        free(games_lists[i]);
        
    CloseWindow();
    return 0;
}


void WorkChip8(Chip8 * chip8, char * games_lists[], unsigned int total_games)
{
    while (!WindowShouldClose()) 
    {
        if (IsKeyPressed(KEY_ESCAPE))
        {
            int next_game = SelectGame(games_lists, total_games);
            if (WindowShouldClose()) 
                break; 

            if (next_game >= 0) 
            {
                Chip8_Init(chip8);                        
                Chip8_LoadROM(chip8, games_lists[next_game]); 
            }
            GetKeyPressed(); 
        }

        HandleInput(chip8);
        
        for (size_t i = 0; i < 8; i++) 
            Chip8_Cycle(chip8);
        
        if (chip8->delay_timer > 0) chip8->delay_timer--;
        if (chip8->sound_timer > 0) chip8->sound_timer--;

        BeginDrawing();       
        ClearBackground(BLACK);

        for (size_t y = 0; y < 32; y++) 
        {
            for (size_t x = 0; x < 64; x++) 
                if (chip8->video[y * 64 + x] != 0) 
                    DrawRectangle(x * SCALE, y * SCALE, SCALE - 1, SCALE - 1, WHITE);
        }
        EndDrawing();
    }
}

int SelectGame(char * games_lists[], unsigned int total_games)
{
    unsigned int selectgame = 0;
    bool enter_game = false;

    while (!WindowShouldClose() && !enter_game) 
    {
        if (total_games > 0) 
        {
            if (IsKeyPressed(KEY_DOWN)) 
                selectgame = (selectgame + 1) % total_games;
            if (IsKeyPressed(KEY_UP)) 
                selectgame = (selectgame - 1 + total_games) % total_games;
            if (IsKeyPressed(KEY_ENTER)) 
                enter_game = true; 
        }

        BeginDrawing();       
        ClearBackground(BLACK);

        if (total_games == 0) 
            DrawText("No ROMs found in 'games/' folder!", 30, screen_height / 2, 20, RED);
        else 
            DrawGamesMenu(games_lists, total_games, selectgame);
            
        EndDrawing();
    }

    return enter_game ? (int)selectgame : -1;
}

