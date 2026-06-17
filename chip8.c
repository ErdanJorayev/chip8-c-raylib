#include "chip8.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <raylib.h>

static const int START_ADDRESS = 0x200;
static const int FONTSET_START_ADDRESS = 0x50;

// Стандартный набор встроенных шрифтов CHIP-8
const uint8_t fontset[80] = 
{
    0xF0, 0x90, 0x90, 0x90, 0xF0, // 0
    0x20, 0x60, 0x20, 0x20, 0x70, // 1
    0xF0, 0x10, 0xF0, 0x80, 0xF0, // 2
    0xF0, 0x10, 0xF0, 0x10, 0xF0, // 3
    0x90, 0x90, 0xF0, 0x10, 0x10, // 4
    0xF0, 0x80, 0xF0, 0x10, 0xF0, // 5
    0xF0, 0x80, 0xF0, 0x90, 0xF0, // 6
    0xF0, 0x10, 0x20, 0x40, 0x40, // 7
    0xF0, 0x90, 0xF0, 0x90, 0xF0, // 8
    0xF0, 0x90, 0xF0, 0x10, 0xF0, // 9
    0xF0, 0x90, 0xF0, 0x90, 0x90, // A
    0xE0, 0x90, 0xE0, 0x90, 0xE0, // B
    0xF0, 0x80, 0x80, 0x80, 0xF0, // C
    0xE0, 0x90, 0x90, 0x90, 0xE0, // D
    0xF0, 0x80, 0xF0, 0x80, 0xF0, // E
    0xF0, 0x80, 0xF0, 0x80, 0x80  // F
};

void HandleInput(Chip8 * chip8) 
{
    chip8->keypad[0x1] = IsKeyDown(KEY_ONE);   chip8->keypad[0x2] = IsKeyDown(KEY_TWO);
    chip8->keypad[0x3] = IsKeyDown(KEY_THREE); chip8->keypad[0xC] = IsKeyDown(KEY_FOUR);
    
    chip8->keypad[0x4] = IsKeyDown(KEY_Q);     chip8->keypad[0x5] = IsKeyDown(KEY_W);
    chip8->keypad[0x6] = IsKeyDown(KEY_E);     chip8->keypad[0xD] = IsKeyDown(KEY_R); 
    
    chip8->keypad[0x7] = IsKeyDown(KEY_A);     chip8->keypad[0x8] = IsKeyDown(KEY_S);
    chip8->keypad[0x9] = IsKeyDown(KEY_D);     chip8->keypad[0xF] = IsKeyDown(KEY_F);
    
    chip8->keypad[0xA] = IsKeyDown(KEY_Z);     chip8->keypad[0x0] = IsKeyDown(KEY_X);
    chip8->keypad[0xB] = IsKeyDown(KEY_C);     chip8->keypad[0xE] = IsKeyDown(KEY_V);
}

void Chip8_Init(Chip8 *chip8) 
{
    memset(chip8, 0, sizeof(*chip8)); 
    chip8->pc = START_ADDRESS;        
    
    for (unsigned int i = 0; i < 80; ++i) 
        chip8->memory[FONTSET_START_ADDRESS + i] = fontset[i]; 
}

bool Chip8_LoadROM(Chip8 *chip8, const char *filename) 
{
    FILE *file = fopen(filename, "rb");
    if (!file) return false;

    fread(&chip8->memory[START_ADDRESS], 1, 4096 - START_ADDRESS, file);
    fclose(file);
    return true;
}


void Chip8_Cycle(Chip8 *chip8) 
{
    chip8->opcode = (chip8->memory[chip8->pc] << 8) | chip8->memory[chip8->pc + 1];
    chip8->pc += 2;

    uint16_t opcode = chip8->opcode;
    uint8_t X    = (opcode & 0x0F00) >> 8; 
    uint8_t Y    = (opcode & 0x00F0) >> 4;
    uint8_t N    = (opcode & 0x000F);
    uint8_t NN   = (opcode & 0x00FF);
    uint16_t NNN = (opcode & 0x0FFF);

    switch (opcode & 0xF000) 
    {
        case 0x0000:
        {
            if (opcode == 0x00E0) // Стираем экран
                memset(chip8->video, 0, sizeof(chip8->video));
            
            else if (opcode == 0x00EE) // работаем со стеком. После функции возвращаемся 
            {
                chip8->sp--;
                chip8->pc = chip8->stack[chip8->sp];
            }
            break;
        }
        case 0x1000: // Инструкция 1NNN: Безусловный переход (Jump)
        {
            chip8->pc = NNN; // Просто перезаписываем счетчик команд на новый адрес
            break;
        }
        case 0x2000: // Инструкция 2NNN: CALL addr (Вызов функции)
        {
            chip8->stack[chip8->sp] = chip8->pc; // Шаг 1: Запоминаем текущий адрес в массив стека
            chip8->sp++;                         // Шаг 2: Сдвигаем указатель стека вверх
            chip8->pc = NNN;                     // Шаг 3: Перезаписываем PC и прыгаем на адрес функции
            break;
        }
        case 0x3000: // Инструкция 3XNN: SE Vx, byte (Пропустить, если равно)
        {
            if (chip8->registers[X] == NN) 
                chip8->pc += 2; // Перешагиваем через следующий двухбайтовый опкод
            break;
        }
        case 0x4000: // Инструкция 4XNN 
        {
            if (chip8->registers[X] != NN) 
                chip8->pc += 2;
            break;
        }
        case 0x5000: // Инструкция 5XY0 Тоже пропуск следующего опкода
        {
        if (chip8->registers[X] == chip8->registers[Y])
                chip8->pc += 2;
            break;
        }
        case 0x6000: // Интсрукция 6XNN Запись числа NN в регистр VX
        {
            chip8->registers[X] = NN;
            break;
        }
        case 0x7000: // Инструкция 7XNN 
        {
            // Прибавляем число NN к текущему значению регистра VX
            chip8->registers[X] += NN;
            break;
        }
        case 0x8000:
        {
            switch (N) // Смотрим на последнюю цифру опкода (переменную N) для выбора операции
            {
                case 0x0: // Инструкция 8XY0: LD Vx, Vy (Копирование)
                {
                    chip8->registers[X] = chip8->registers[Y]; // Копируем значение из регистра VY в регистр VX
                    break;
                }

                case 0x1: // Инструкция 8XY1: OR Vx, Vy (Побитовое ИЛИ)
                {
                    chip8->registers[X] |= chip8->registers[Y]; // Выполняем побитовое ИЛИ между VX и VY
                    break;
                }

                case 0x2: // Инструкция 8XY2: AND Vx, Vy (Побитовое И)
                {
                    chip8->registers[X] &= chip8->registers[Y]; // Выполняем побитовое И между VX и VY
                    break;
                }

                case 0x3: // Инструкция 8XY3: XOR Vx, Vy (Побитовый XOR)
                {
                    chip8->registers[X] ^= chip8->registers[Y]; // Выполняем исключающее ИЛИ между VX и VY
                    break;
                }

                case 0x4: // Инструкция 8XY4: ADD Vx, Vy (Сложение с переносом)
                {
                    uint16_t sum = chip8->registers[X] + chip8->registers[Y]; // Считаем сумму в 16 битах, чтобы поймать переполнение
                    chip8->registers[X] = sum & 0xFF; // Записываем результат сложения в регистр VX
                    chip8->registers[0xF] = (sum > 255) ? 1 : 0; // Если сумма больше 255, выставляем флаг переноса VF = 1, иначе 0
                    break;
                }

                case 0x5: // Инструкция 8XY5: SUB Vx, Vy (Вычитание с заёмом)
                {
                    uint8_t borrow = (chip8->registers[X] >= chip8->registers[Y]) ? 1 : 0; // Запоминаем флаг: 1 если заёма нет, 0 если заём нужен
                    chip8->registers[X] -= chip8->registers[Y]; // Вычитаем значение VY из регистра VX
                    chip8->registers[0xF] = borrow; // Записываем флаг заёма в регистр VF
                    break;
                }

                case 0x6: // Инструкция 8XY6: SHR Vx {, Vy} (Сдвиг вправо)
                {
                    uint8_t lsb = chip8->registers[X] & 0x01; // Вытаскиваем самый младший (правый) бит регистра VX
                    chip8->registers[X] >>= 1; // Сдвигаем биты регистра VX вправо на 1 шаг (деление на 2)
                    chip8->registers[0xF] = lsb; // Сохраняем вылетевший бит в системный регистр VF
                    break;
                }

                case 0x7: // Инструкция 8XY7: SUBN Vx, Vy (Обратное вычитание)
                {
                    uint8_t borrow = (chip8->registers[Y] >= chip8->registers[X]) ? 1 : 0; // Запоминаем флаг: 1 если заёма нет, 0 если заём нужен
                    chip8->registers[X] = chip8->registers[Y] - chip8->registers[X]; // Вычитаем VX из VY и пишем результат в VX
                    chip8->registers[0xF] = borrow; // Записываем флаг заёма в регистр VF
                    break;
                }

                case 0xE: // Инструкция 8XYE: SHL Vx {, Vy} (Сдвиг влево)
                {
                    uint8_t msb = (chip8->registers[X] & 0x80) >> 7; // Вытаскиваем самый старший (левый) бит регистра VX
                    chip8->registers[X] <<= 1; // Сдвигаем биты регистра VX влево на 1 шаг (умножение на 2)
                    chip8->registers[0xF] = msb; // Сохраняем вылетевший бит в системный регистр VF
                    break;
                }
            }
            break;
        }
        case 0x9000: // Интсрукция 9XYE
        {
            if (chip8->registers[X] != chip8->registers[Y]) 
                chip8->pc += 2; // Пропускаем следующий опкод, если регистры не равны
            break;
        }
        case 0xA000: // Инструкция ANNN: LD I, addr (Загрузка индекса)
        {
            chip8->index = NNN; // Записываем адрес NNN в индексный регистр I
            break;
        }
        case 0xB000: // Инструкция BNNN: JP V0, addr (Прыжок со смещением V0)
        {
            chip8->pc = NNN + chip8->registers[0]; // Перезаписываем счетчик команд на адрес NNN плюс значение V0
            break;
        }
        case 0xC000: // Инструкция CXNN: RND Vx, byte (Случайное число с маской NN)
        {
            chip8->registers[X] = (rand() & 0xFF) & NN; // Генерируем случайный байт, накладываем маску NN и пишем в VX
            break;
        }
        case 0xD000: // Инструкция DXYN: DRW Vx, Vy, nibble (Отрисовка спрайта)
        {
            uint8_t x_pos = chip8->registers[X] % 64; // Получаем координату X с защитой от вылета за экран
            uint8_t y_pos = chip8->registers[Y] % 32; // Получаем координату Y с защитой от вылета за экран
            chip8->registers[0xF] = 0; // Сбрасываем флаг столкновения в VF перед отрисовкой

            for (uint8_t row = 0; row < N; row++) // Цикл по строкам спрайта высотой N
            {
                uint8_t sprite_byte = chip8->memory[chip8->index + row]; // Читаем байт строки спрайта из памяти
                if (y_pos + row >= 32) break; // Прекращаем рисовать, если вышли за нижнюю границу экрана

                for (int col = 0; col < 8; col++) // Цикл по 8 пикселям (битам) текущей строки
                {
                    if (x_pos + col >= 64) break; // Прекращаем рисовать строку, если вышли за правый край

                    if ((sprite_byte & (0x80 >> col)) != 0) // Проверяем, горит ли текущий бит в байте спрайта
                    {
                        size_t screen_index = (x_pos + col) + ((y_pos + row) * 64); // Вычисляем индекс пикселя в массиве video
                        if (chip8->video[screen_index] == 0xFFFFFFFF) // Если пиксель на экране уже белый
                        {
                            chip8->video[screen_index] = 0x00000000; // Гасим его (XOR-эффект)
                            chip8->registers[0xF] = 1; // Фиксируем столкновение в регистре VF
                        }
                        else
                        {
                            chip8->video[screen_index] = 0xFFFFFFFF; // Зажигаем пиксель белым цветом
                        }
                    }
                }
            }
            break;
        }
        case 0xE000:
        {
            switch (NN) // Смотрим на последние две цифры (байт NN) для выбора операции
            {
                case 0x9E: // Инструкция EX9E: SKP Vx (Пропустить, если нажата)
                {
                    uint8_t key = chip8->registers[X]; // Получаем код клавиши из регистра VX
                    if (chip8->keypad[key] == 1) chip8->pc += 2; // Если клавиша нажата, перешагиваем следующий опкод
                    break;
                }

                case 0xA1: // Инструкция EXA1: SKNP Vx (Пропустить, если НЕ нажата)
                {
                    uint8_t key = chip8->registers[X]; // Получаем код клавиши из регистра VX
                    if (chip8->keypad[key] == 0) chip8->pc += 2; // Если клавиша отжата, перешагиваем следующий опкод
                    break;
                }
            }
            break;
        }
        case 0xF000:
        {
            switch (NN) // Смотрим на последние две цифры (байт NN) для выбора операции
            {
                case 0x07: // Инструкция FX07: LD Vx, DT (Прочитать таймер задержки)
                {
                    chip8->registers[X] = chip8->delay_timer; // Записываем текущее значение таймера в регистр VX
                    break;
                }

                case 0x0A: // Инструкция FX0A: LD Vx, K (Ожидание нажатия клавиши)
                {
                    bool key_pressed = false; // Флаг, была ли нажата хоть одна кнопка
                    for (int i = 0; i < 16; i++) // Сканируем весь массив клавиатуры
                    {
                        if (chip8->keypad[i] == 1) 
                        {
                            chip8->registers[X] = i; // Записываем номер нажатой клавиши в VX
                            key_pressed = true; // Фиксируем нажатие
                            break;
                        }
                    }
                    if (!key_pressed) chip8->pc -= 2; // Если ничего не нажато, откатываем PC назад, чтобы повторить команду на следующем шаге
                    break;
                }

                case 0x15: // Инструкция FX15: LD DT, Vx (Установить таймер задержки)
                {
                    chip8->delay_timer = chip8->registers[X]; // Загружаем значение из регистра VX в таймер задержки
                    break;
                }

                case 0x18: // Инструкция FX18: LD ST, Vx (Установить звуковой таймер)
                {
                    chip8->sound_timer = chip8->registers[X]; // Загружаем значение из регистра VX в звуковой таймер
                    break;
                }

                case 0x1E: // Инструкция FX1E: ADD I, Vx (Сложить индекс и VX)
                {
                    chip8->index += chip8->registers[X]; // Прибавляем значение регистра VX к индексному регистру I
                    break;
                }

                case 0x29: // Инструкция FX29: LD F, Vx (Установить адрес шрифта)
                {
                    chip8->index = FONTSET_START_ADDRESS + (chip8->registers[X] * 5); // Считаем адрес встроенного спрайта-символа (каждый весит 5 байт)
                    break;
                }

                case 0x33: // Инструкция FX33: LD B, Vx (BCD представление числа)
                {
                    uint8_t value = chip8->registers[X]; // Берем число из VX (0-255)
                    chip8->memory[chip8->index]     = value / 100; // Раскладываем сотни по адресу I
                    chip8->memory[chip8->index + 1] = (value / 10) % 10; // Раскладываем десятки по адресу I+1
                    chip8->memory[chip8->index + 2] = value % 10; // Раскладываем единицы по адресу I+2
                    break;
                }

                case 0x55: // Инструкция FX55: LD [I], Vx (Сохранить регистры в память)
                {
                    for (uint8_t i = 0; i <= X; i++) chip8->memory[chip8->index + i] = chip8->registers[i]; // Пишем регистры от V0 до VX в память начиная с I
                    break;
                }

                case 0x65: // Инструкция FX65: LD Vx, [I] (Загрузить регистры из памяти)
                {
                    for (uint8_t i = 0; i <= X; i++) chip8->registers[i] = chip8->memory[chip8->index + i]; // Заполняем регистры от V0 до VX данными из памяти начиная с I
                    break;
                }
            }
            break;
        }
    }

}
