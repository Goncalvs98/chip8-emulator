#include "chip8.h"
#include <stdio.h>
#include <stdlib.h>
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

/*
0x000-0x1FF - Chip 8 interpreter (contains font set in emu)
0x050-0x0A0 - Used for the built in 4x5 pixel font set (0-F)
0x200-0xFFF - Program ROM and work RAM
*/

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

void chip8_emulate_cycle(Chip8State* state){
    // 1. Fetch
    uint8_t byte1 = state->memory[state->pc];
    uint8_t byte2 = state->memory[state->pc + 1];

    uint16_t opcode = (byte1 << 8) | byte2;

    state->pc += 2;

    // 2. Decode
    uint16_t NNN    = opcode & 0x0FFF;
    uint16_t NN     = opcode & 0x00FF;
    uint16_t N      = opcode & 0x000F;
    uint16_t X      = (opcode & 0x0F00) >> 8;
    uint16_t Y      = (opcode & 0x00F0) >> 4;

    // 3. Execute
    switch (opcode & 0xF000){
    case 0x0000:
        // Opcodes Especiais
        switch (NN){
            case 0xE0: // 0x00E0: CLS (Clear Screen)
                memset(state->video_buffer, 0, sizeof(state->video_buffer));
                state->draw_flag = 1;
                break;
            case 0xEE: // 0x00EE: RET (Return from subroutine)
                state->sp--;
                state->pc = state->stack[state->sp];
                break;
            default:
                printf("Opcode 0x0... desconhecido: 0x%X\n", opcode);
                break;
        }
        break;
    
    case 0x1000: // Opcode 0x1NNN: JMP (Jump)
        state->pc = NNN;
        break;
    
    case 0x2000: // Opcode 0x2NNN: CALL (Call subroutine)
        state->stack[state->sp] = state->pc;
        state->sp++;
        state->pc = NNN;
        break;
    
    case 0x3000: // Opcode 0x3XNN: if (skip if equal)
        if(state->V[X] == NN){
            state->pc += 2;
        }
        break;
    
    case 0x4000: // Opcode 0x4XNN: ifn (skip if not equal)
        if(state->V[X] != NN){
            state->pc += 2;
        }
        break;
    
    case 0x5000: // Opcode 0x5XY0: IFE (compare two values) 
        if(state->V[X] == state->V[Y]){
            state->pc += 2;
        }
        break;
    
    case 0x6000: // Opcode 0x6XNN: LDX (Load)
        state->V[X] = NN;
        break;
    
    case 0x7000: // Opcode 0x7XNN: ADD
        state->V[X] += NN;
        break;
    
    case 0x8000: // Opcode 0x8XYN (ALU)
        switch (N){
        case 0x0: // 8XY0: LD
            state->V[X] = state->V[Y];
            break;
        
        case 0x1: // 8XY1: OR
            state->V[X] |= state->V[Y];
            break;
        
        case 0x2: // 8XY2: AND
            state->V[X] &= state->V[Y];
            break;
        
        case 0x3: // 8XY3: XOR
            state->V[X] ^= state->V[Y];
            break;
        
        case 0x4: // 8XY4: ADD
            uint16_t sum = state->V[X] + state->V[Y];
            state->V[0xF] = (sum > 0xFF) ? 1 : 0;
            state->V[X] = (uint8_t)sum;
            break;
        
        case 0x5: // 8XY5: SUB
            state->V[0xF] = (state->V[X] > state->V[Y]) ? 1 : 0;
            state->V[X] -= state->V[Y];
            break;
        
        case 0x6: // 8XY6: SHR
            state->V[0xF] = state->V[X] & 0x1;
            state->V[X] >>= 1;
            break;
        
        case 0x7: // 8XY7: SUBN
            state->V[0xF] = (state->V[Y] > state->V[X]) ? 1 : 0;
            state->V[X] = state->V[Y] - state->V[X];
            break;
        
        case 0xE: // 8XYE: SHL
            state->V[0xF] = (state->V[X] & 0x80) >> 7;
            state->V[X] <<= 1;
            break;
        
        default:
            break;
        }
        break;
    
    case 0x9000: // Opcode 9XY0: SNE
        if (state->V[X] != state->V[Y]){
            state->pc += 2;
        }
        break;
    
    case 0xA000: // Opcode ANNN: LDI
        state->I = NNN;
        break;
    
    case 0xB000: // Opcode BNNN: JP
        state->pc = NNN + state->V[0];
        break;
    
    case 0xC000: // Opcode CXNN: RND
        state->V[X] = (rand() % 256) & NN;
        break;
    
    case 0xD000: // Opcode DXYN: DRW (Draw)
        uint8_t x_coord = state->V[X] % VIDEO_WIDTH;
        uint8_t y_coord = state->V[Y] % VIDEO_HEIGHT;
        uint8_t height = N;

        state->V[0xF] = 0;

        for (int row = 0; row < height; row++){
            uint8_t sprite_byte = state->memory[state->I + row];

            for (int col = 0; col < 8; col++){
                uint8_t sprinte_pixel = (sprite_byte >> (7 - col)) & 1;

                if (sprinte_pixel == 1){
                    int screen_x = (x_coord + col) % VIDEO_WIDTH;
                    int screen_y = (y_coord + row) % VIDEO_HEIGHT;

                    uint32_t* screen_pixel = &state->video_buffer[screen_y * VIDEO_WIDTH + screen_x];

                    if (*screen_pixel == 0xFFFFFFFF){
                        state->V[0xF] = 1;
                    }

                    *screen_pixel ^= 0xFFFFFFFF;
                }
            }
            
        }
        state->draw_flag = 1;
        break;
    
    case 0xE000: // Opcode EXNN
        switch (NN){
        case 0x9E: // Opcode EX9E
            if (state->keypad[state->V[X]]){
                state->pc += 2;
            }
            break;
        
        case 0xA1: // Opcode EXA1
            if (!state->keypad[state->V[X]]){
                state->pc += 2;
            }
            break;
        
        default:
            printf("Opcode 0xE... desconhecido: 0x%X\n", opcode);
            break;
        }
        break;
    
    case 0xF000: // Opcode FXNN
        switch (NN){
        case 0x07: // Opcode FX07
            state->V[X] = state->delay_timer;
            break;
        
        case 0x0A: // Opcode FX0A
            int key_pressed = -1;
            for (int i = 0; i < KEYPAD_SIZE; i++){
                if (state->keypad[i]){
                    key_pressed = i;
                    break;
                }
            }

            if (key_pressed != -1){
                state->V[X] = key_pressed;
            }else{
                state->pc -= 2;
            }
            break;
        
        case 0x15: // Opcode FX15
            state->delay_timer = state->V[X];
            break;
        
        case 0x18: // Opcode FX18
            state->sound_timer = state->V[X];
            break;
        
        case 0x1E: // Opcode FX1E
            state->I += state->V[X];
            break;
        
        case 0x29: // Opcode FX29
            state->I = FONTSET_START_ADDRESS + (state->V[X] * 5);
            break;
        
        case 0x33: // Opcode FX33
            uint8_t value = state->V[X];
            state->memory[state->I]     = value / 100;
            state->memory[state->I + 1] = (value / 10) % 10;
            state->memory[state->I + 2] = value % 10;
            break;
        
        case 0x55: // Opcode FX55
            for (int i = 0; i < X; i++){
                state->memory[state->I + i] = state->V[i];
            }
            
            break;
        
        case 0x65: // Opcode FX65
            for (int i = 0; i < X; i++){
                state->V[i] = state->memory[state->I + i];
            }
            
            break;
        
        default:
            break;
        }
        break;
    
    default:
        printf("Opcode desconhecido: 0x%X\n", opcode);
        break;
    }
}
