#ifndef CHIP8_H
#define CHIP8_H

#include <stdint.h>

typedef struct 
{
    uint8_t registers[16];   // Registers V0 - VF
    uint8_t memory[4096];    // 4 KB of RAM
    uint16_t index;          // Index register I
    uint16_t pc;             // Program Counter
    uint16_t stack[16];      // Subroutine stack
    uint8_t sp;              // Stack Pointer
    uint8_t delay_timer;     // Delay timer
    uint8_t sound_timer;     // Sound timer
    uint8_t keypad[16];      // Keypad state (0-F)
    uint32_t video[64 * 32]; // Screen buffer (64x32 pixels)
    uint16_t opcode;         // Current operation code
} Chip8;

void Chip8_Init(Chip8 * chip8);
bool Chip8_LoadROM(Chip8 * chip8, const char * filename);
void Chip8_Cycle(Chip8 * chip8);
void HandleInput(Chip8 * chip8);

unsigned int GetCh8Games(char * games_array[], unsigned int sz);
void DrawGamesMenu(char * games_array[], unsigned int total_games, unsigned int selected_game);
void WorkChip8(Chip8 * chip8, char * games_lists[], unsigned int total_games);
int SelectGame(char * games_lists[], unsigned int total_games);


#endif
