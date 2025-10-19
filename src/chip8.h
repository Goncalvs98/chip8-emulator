#ifndef CHIP8_H
#define CHIP8_H

#include <stdint.h> // Para usar uint8_t, uint16_t, etc.

#define MEMORY_SIZE 4096
#define NUM_REGISTERS 16
#define STACK_SIZE 16
#define KEYPAD_SIZE 16
#define VIDEO_WIDTH 64
#define VIDEO_HEIGHT 32

typedef struct {
    uint8_t memory[MEMORY_SIZE];
    uint8_t V[NUM_REGISTERS];         // Registradores de V0 a VF
    uint16_t I;                         // Registrador de índice
    uint16_t pc;                        // Program Counter
    uint16_t stack[STACK_SIZE];
    uint8_t sp;                         // Stack Pointer

    uint8_t delay_timer;
    uint8_t sound_timer;

    uint8_t keypad[KEYPAD_SIZE];
    uint32_t video_buffer[VIDEO_WIDTH * VIDEO_HEIGHT]; // Buffer gráfico (64x32)
    
    int draw_flag; // Flag para indicar que a tela precisa ser redesenhada

} Chip8State;

// Protótipos das funções que você implementará
void chip8_initialize(Chip8State* state);
int  chip8_load_rom(Chip8State* state, const char* filename);
void chip8_emulate_cycle(Chip8State* state);

#endif // CHIP8_H
