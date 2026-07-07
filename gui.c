/*
    This source code file contains GUI functions
    Here we get game names from a folder and draw a selection menu
    of the games taken from the folder    
*/

#include "chip8.h"
#include <raylib.h>
#include <string.h>
#include <stdlib.h>

static const int SCALE = 12; 
const int screen_width = 64 * SCALE;
const int screen_height = 32 * SCALE;

// Get file names from the games folder
unsigned int GetCh8Games(char * games_array[], unsigned int sz) 
{
    unsigned int game_count = 0;
    FilePathList files = LoadDirectoryFiles("games");         // Get every file from the games folder

    for (unsigned int i = 0; i < files.count; i++) 
    {
        if (IsFileExtension(files.paths[i], ".ch8"))          // Save only .ch8 files to our game names array
        {
            
            games_array[game_count] = strdup(files.paths[i]); // strdup automatically calculates string size and allocates memory
           
            if (games_array[game_count] != NULL)           
                game_count++;
            
            if (game_count >= sz) 
                break; 
        }
    }
    UnloadDirectoryFiles(files);

    return game_count; 
}


// Draw the game selection menu
void DrawGamesMenu(char * games_array[], unsigned int total_games, unsigned int selected_game) 
{
    DrawRectangleLines(10, 10, screen_width - 20, screen_height - 20, DARKGREEN);

    DrawText("CHIP-8 GAMES", screen_width / 2 - 100, 30, 22, GREEN);
    DrawLine(20, 60, screen_width - 20, 60, GREEN);

    int line_height = 28;
    int start_y = 90;
    
    int max_visible_games = (screen_height - 60 - start_y) / line_height; 
    if (max_visible_games <= 0) 
        max_visible_games = 1;

    unsigned int start_index = 0;
    if (selected_game >= (unsigned int)max_visible_games) 
        start_index = selected_game - max_visible_games + 1;
    

    unsigned int end_index = start_index + max_visible_games;
    if (end_index > total_games) 
        end_index = total_games;
    
    for (unsigned int i = start_index; i < end_index; i++) 
    {
        int local_row = i - start_index;
        int y_pos = start_y + (local_row * line_height);
        const char * game_name = GetFileNameWithoutExt(games_array[i]);

        if (i == selected_game) 
        {
            DrawRectangle(30, y_pos - 4, screen_width - 60, 24, DARKGREEN);
            DrawText(game_name, 50, y_pos, 18, WHITE);
            DrawText(">", 35, y_pos, 18, GREEN);
        } 
        else 
            DrawText(game_name, 50, y_pos, 18, LIME);
        
    }

    if (end_index < total_games)
    {
        int dots_y = start_y + (max_visible_games * line_height);
        if (dots_y < screen_height - 50) 
            DrawText("... and more files ...", 50, dots_y, 18, GRAY);
    }

    DrawLine(20, screen_height - 45, screen_width - 20, screen_height - 45, DARKGREEN);
    DrawText("[UP/DOWN] Select   [ENTER] Start", 30, screen_height - 35, 14, GRAY);
}

// Work CHIP-8
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
        
        if (chip8->delay_timer > 0) 
            chip8->delay_timer--;
        if (chip8->sound_timer > 0) 
            chip8->sound_timer--;

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

        if (!total_games)                                                                  // Если игр нет выводим сообщение
            DrawText("No ROMs or no 'games/'", screen_width / 2 - 140, screen_height / 2, 20, RED);
        else 
            DrawGamesMenu(games_lists, total_games, selectgame);
            
        EndDrawing();
    }

    return enter_game ? (int)selectgame : -1;
}