#include "chip8.h"
#include <raylib.h>
#include <string.h>
#include <stdlib.h>

extern const int screen_width;
extern const int screen_height;

unsigned int GetCh8Games(char * games_array[], unsigned int sz) 
{
    unsigned int game_count = 0;
    FilePathList files = LoadDirectoryFiles("games");

    for (unsigned int i = 0; i < files.count; i++) 
    {
        if (IsFileExtension(files.paths[i], ".ch8")) 
        {
            
            games_array[game_count] = strdup(files.paths[i]);
           
            if (games_array[game_count] != NULL) 
                game_count++;
            
            if (game_count >= sz) 
                break; 
        }
    }
    UnloadDirectoryFiles(files);

    return game_count; 
}

void DrawGamesMenu(char * games_array[], unsigned int total_games, unsigned int selected_game) 
{
    DrawRectangleLines(10, 10, screen_width - 20, screen_height - 20, DARKGREEN);

    DrawText("CHIP-8 BOOT MENU", screen_width / 2 - 100, 30, 22, GREEN);
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

