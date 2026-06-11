#include "chip8.h"
#include <raylib.h>
#include <string.h>
#include <stdlib.h>

// Грузим игры
unsigned int GetCh8Games(char * games_array[], unsigned int sz) 
{
    unsigned int game_count = 0;

    // Считываем всё содержимое папки "games" через Raylib
    FilePathList files = LoadDirectoryFiles("games");

    for (unsigned int i = 0; i < files.count; i++) 
    {
        if (IsFileExtension(files.paths[i], ".ch8")) 
        {
            games_array[game_count] = malloc(strlen(files.paths[i]) + 1);
           
            if (games_array[game_count] != NULL) 
            {
                strcpy(games_array[game_count], files.paths[i]);
                game_count++;
            }
            
            if (game_count >= sz) 
                break; // Защита от выхода за границы массива указателей
        }
    }
    UnloadDirectoryFiles(files);

    return game_count; 
}

// Выводим список игр
void DrawGamesMenu(char *games_array[], unsigned int total_games, unsigned int selected_game, int screen_width, int screen_height) 
{
    // 1. Рисуем рамку в стиле старых терминалов
    DrawRectangleLines(10, 10, screen_width - 20, screen_height - 20, DARKGREEN);

    // 2. Выводим заголовок меню
    DrawText("CHIP-8 BOOT MENU", screen_width / 2 - 100, 30, 22, GREEN);
    DrawLine(20, 60, screen_width - 20, 60, GREEN);

    // 3. Выводим список игр
    for (unsigned int i = 0; i < total_games; i++) 
    {
        // Каждая строка смещается вниз на 28 пикселей
        int y_pos = 90 + (i * 28);
        
        // Защита: если список слишком длинный и упирается в подсказку внизу
        if (y_pos > screen_height - 60) 
        {
            DrawText("... and files ...", 40, y_pos, 18, GRAY);
            break;
        }

        // 4. Логика подсветки выбранного пункта
        if (i == selected_game) 
        {
            // Рисуем контрастную плашку под текстом
            DrawRectangle(30, y_pos - 4, screen_width - 60, 24, DARKGREEN);
            // Белый текст на зеленом фоне читается отлично
            DrawText(games_array[i], 50, y_pos, 18, WHITE);
            // Рисуем маркер-указатель слева
            DrawText(">", 35, y_pos, 18, GREEN);
        } 
        else 
            // Обычные неактивные пункты меню горят ретро-зеленым
            DrawText(games_array[i], 50, y_pos, 18, LIME);
    }

    // 4. Подсказка для управления в самом низу экрана
    DrawLine(20, screen_height - 45, screen_width - 20, screen_height - 45, DARKGREEN);
    DrawText("[UP/DOWN] Select   [ENTER] Start", 30, screen_height - 35, 14, GRAY);
}