#include <raylib.h>
#include "chip8.h"
#include <stdio.h>
#include <time.h>
#include <stdlib.h>

extern const int screen_width;
extern const int screen_height;

int main() 
{
    srand(time(NULL));

    Chip8 chip8;
    char * games_lists[100];
    unsigned int total = GetCh8Games(games_lists, 100);                    // Берем общее количество игр

    InitWindow(screen_width, screen_height, "C + Raylib CHIP-8 Emulator"); // Создаем окно 
    SetTargetFPS(60);                                                      // Установка количество кадров в секунду
    SetExitKey(KEY_NULL);                                                  // ESC возваращет в меню а не закрывает окно

    int selected_game = SelectGame(games_lists, total);                    // Выбираем игру

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
