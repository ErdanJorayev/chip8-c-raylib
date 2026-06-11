#include <raylib.h>
#include "chip8.h"
#include <stdio.h>
#include <time.h>
#include <stdlib.h>

#define SCALE 12 // Масштаб отображения одного пикселя
const int screen_width = 64 * SCALE;
const int screen_height = 32 * SCALE;

// Карта оригинальных клавиш CHIP-8 на раскладку клавиатуры ПК
void HandleInput(Chip8 * chip8) 
{
    chip8->keypad[0x1] = IsKeyDown(KEY_ONE);   chip8->keypad[0x2] = IsKeyDown(KEY_TWO);
    chip8->keypad[0x3] = IsKeyDown(KEY_THREE); chip8->keypad[0xC] = IsKeyDown(KEY_FOUR);
    
    chip8->keypad[0x4] = IsKeyDown(KEY_Q);     chip8->keypad[0x5] = IsKeyDown(KEY_W);
    chip8->keypad[0x6] = IsKeyDown(KEY_E);     chip8->keypad[0xD] = IsKeyDown(KEY_R); // Исправлен индекс: 0x4 заменен на 0xD
    
    chip8->keypad[0x7] = IsKeyDown(KEY_A);     chip8->keypad[0x8] = IsKeyDown(KEY_S);
    chip8->keypad[0x9] = IsKeyDown(KEY_D);     chip8->keypad[0xF] = IsKeyDown(KEY_F);
    
    chip8->keypad[0xA] = IsKeyDown(KEY_Z);     chip8->keypad[0x0] = IsKeyDown(KEY_X);
    chip8->keypad[0xB] = IsKeyDown(KEY_C);     chip8->keypad[0xE] = IsKeyDown(KEY_V);
}

void WorkChip8(Chip8 * ch);

int main() 
{
    srand(time(NULL));

    Chip8 chip8;
    char * games_lists[100];
    unsigned int total_games = GetCh8Games(games_lists, 100);

    unsigned int selected_game = 0;
    bool game_selected = false;

    // Инициализация окна Raylib 64*12 x 32*12
    InitWindow(screen_width, screen_height, "C + Raylib CHIP-8 Emulator");
    SetTargetFPS(60); // Частота обновления экрана и таймеров

    while (!WindowShouldClose() && !game_selected) 
    {
        // Обработка кнопок в меню
        if (IsKeyPressed(KEY_DOWN) && total_games > 0) 
            selected_game = (selected_game + 1) % total_games;
        if (IsKeyPressed(KEY_UP) && total_games > 0) 
            selected_game = (selected_game - 1 + total_games) % total_games;
        if (IsKeyPressed(KEY_ENTER) && total_games > 0) 
            game_selected = true; // Выходим из цикла меню

        // Отрисовка меню
        BeginDrawing();       
        ClearBackground(BLACK);

        if (total_games == 0) 
            DrawText("No ROMs found in 'games/' folder!", 30, screen_height / 2, 20, RED);
        else 
            DrawGamesMenu(games_lists, total_games, selected_game, screen_width, screen_height);
        EndDrawing();
    }

    // Если пользователь закрыл окно крестиком во время меню — завершаем программу
    if (WindowShouldClose()) 
    {
        // Освобождаем память перед выходом
        for (unsigned int i = 0; i < total_games; i++) 
            free(games_lists[i]);
        CloseWindow();
        return 0;
    }

    Chip8_Init(&chip8);
    Chip8_LoadROM(&chip8, games_lists[selected_game]);

    WorkChip8(&chip8); // работа чипа

    CloseWindow();
    return 0;
}

void WorkChip8(Chip8 * chip8)
{
    while (!WindowShouldClose()) 
    {
        HandleInput(chip8);

        // Стандартные ROM выполняют ~8-10 процессорных циклов за 1 кадр графики (60Гц)
        for (size_t i = 0; i < 8; i++) 
            Chip8_Cycle(chip8);
        
        // Обновление таймеров системы (срабатывают строго на частоте 60 Гц за кадр)
        if (chip8->delay_timer > 0) chip8->delay_timer--;
        if (chip8->sound_timer > 0) chip8->sound_timer--;

        BeginDrawing();       
        ClearBackground(BLACK);

        // Рендеринг буфера экрана
        for (size_t y = 0; y < 32; y++) 
        {
            for (size_t x = 0; x < 64; x++) 
                if (chip8->video[y * 64 + x] != 0) 
                    DrawRectangle(x * SCALE, y * SCALE, SCALE - 1, SCALE - 1, WHITE);
        }

        EndDrawing();
    }
}