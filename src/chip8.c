#include "chip8.h"
#include <stdio.h>
#include <string.h>

const unsigned int FONTSET_START_ADDRESS = 0x50;

uint8_t fontset[80] = {
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

const unsigned int ROM_START_ADDRESS = 0x200;

void chip8_initialize(Chip8State* state){
    memset(state, 0, sizeof(Chip8State)); // Limpa a memória que poderia estar no local do state
    
    state->pc = ROM_START_ADDRESS; // Declara onde a memória vai começar, a documentação do CHIP-8 informa que por padrão começa no endereço 512 (0x200)

    memcpy(&state->memory[FONTSET_START_ADDRESS], fontset, sizeof(fontset)); // Copia o fontset para a memória
}

int chip8_load_rom(Chip8State* state, const char* filename){
    FILE* rom_file = fopen(filename, "rb");
    if (!rom_file){
        fprintf(stderr, "ERRO: Não foi possível abrir a ROM: %s\n", filename);
        return 0; // Falha
    }
     
    fseek(rom_file, 0, SEEK_END);
    long rom_size = ftell(rom_file);
    fseek(rom_file, 0, SEEK_SET);

    if (rom_size > (MEMORY_SIZE - ROM_START_ADDRESS)){
        fprintf(stderr, "ERRO: ROM muito grande para a memória.\n");
        fclose(rom_file);
        return 0; // Falha
    }

    size_t read_size = fread(&state->memory[ROM_START_ADDRESS], 1, rom_size, rom_file);

    if (read_size != rom_size){
        fprintf(stderr, "ERRO: Falha ao ler a ROM inteira.\n");
        fclose(rom_file);
        return 0; // Falha
    }

    fclose(rom_file);
    return 1; // Sucesso!
}
