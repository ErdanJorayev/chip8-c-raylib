#ifndef CHIP8_H
#define CHIP8_H

#include <stdint.h>

#define START_ADDRESS 0x200
#define FONTSET_START_ADDRESS 0x50

typedef struct 
{
    uint8_t registers[16];   // Регистры V0 - VF
    uint8_t memory[4096];    // 4 КБ оперативной памяти
    uint16_t index;          // Индексный регистр I
    uint16_t pc;             // Счетчик команд (Program Counter)
    uint16_t stack[16];      // Стек для подпрограмм
    uint8_t sp;              // Указатель стека (Stack Pointer)
    uint8_t delay_timer;     // Таймер задержки
    uint8_t sound_timer;     // Таймер звука
    uint8_t keypad[16];      // Состояние клавиатуры (0-F)
    uint32_t video[64 * 32]; // Буфер экрана (64х32 пикселя)
    uint16_t opcode;         // Текущий код операции
} Chip8;

void Chip8_Init(Chip8 * chip8);
bool Chip8_LoadROM(Chip8 * chip8, const char *filename);
void Chip8_Cycle(Chip8 * chip8);

unsigned int GetCh8Games(char * games_array[], unsigned int sz);
void DrawGamesMenu(char * games_array[], unsigned int total_games, unsigned int selected_game, int screen_width, int screen_height);

#endif
