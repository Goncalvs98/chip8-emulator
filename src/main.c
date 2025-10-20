#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define SDL_MAIN_HANDLED
#include <SDL2/SDL.h>
#include "chip8.h"

const int CHIP8_WIDTH = 64;
const int CHIP8_HEIGHT = 32;
const int SCALE_FACTOR = 10;
const int WINDOW_WIDTH = CHIP8_WIDTH * SCALE_FACTOR;
const int WINDOW_HEIGHT = CHIP8_HEIGHT * SCALE_FACTOR;

const int CYCLES_PER_FRAME = 10;

int main(int argc, char* argv[])
{
    srand(time(NULL)); // Semear o gerador aleatório

    // Start setup ROM
    if (argc < 2){
        fprintf(stderr, "Uso: %s <caminho_para_a_rom>\n", argv[0]);
        return 1;
    }

    Chip8State chip8_state;
    chip8_initialize(&chip8_state);

    if (!chip8_load_rom(&chip8_state, argv[1])){
        return 1;
    }

    printf("ROM '%s' carregada com sucesso!\n", argv[1]);
    // End setup ROM
 
    // Start setup Graphics
    if (SDL_Init(SDL_INIT_VIDEO) < 0){
        fprintf(stderr, "Erro ao inicializar SDL: %s\n", SDL_GetError());
        return 1;
    }

    SDL_Window* window = SDL_CreateWindow(
        "Emulador CHIP-8",
        SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
        WINDOW_WIDTH, WINDOW_HEIGHT,
        SDL_WINDOW_SHOWN
    );

    if (!window){
        fprintf(stderr, "Erro ao criar janela: %s\n", SDL_GetError());
        SDL_Quit();
        return 1;
    }

    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    if (!renderer) {
        fprintf(stderr, "Erro ao criar renderer: %s\n", SDL_GetError());
        SDL_DestroyWindow(window);
        SDL_Quit();
        return 1;
    }

    SDL_Texture* texture = SDL_CreateTexture(
        renderer,
        SDL_PIXELFORMAT_ARGB8888,
        SDL_TEXTUREACCESS_STREAMING,
        CHIP8_WIDTH,
        CHIP8_HEIGHT
    );

    if (!texture){
        fprintf(stderr, "Erro ao criar textura: %s\n", SDL_GetError());
        return 1;
    }

    printf("Setup SDL2 bem sucedido!\n");
    // End setup Graphics

    int running = 1;
    while (running) {
        SDL_Event event;
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT){
                running = 0;
            }
        }

        for (int i = 0; i < CYCLES_PER_FRAME; i++){
            chip8_emulate_cycle(&chip8_state);
        }
        
        if (chip8_state.delay_timer > 0){
            chip8_state.delay_timer--;
        }

        if (chip8_state.sound_timer > 0){
            if (chip8_state.sound_timer == 1){
                printf("BEEP!\n");
                // TODO: Tocar Beep
            }
            chip8_state.sound_timer--;
        }

        SDL_SetRenderDrawColor(renderer, 5, 5, 30, 255);
        SDL_RenderClear(renderer);

        if (chip8_state.draw_flag){
            SDL_UpdateTexture(
                texture,
                NULL,
                chip8_state.video_buffer,
                CHIP8_WIDTH * sizeof(uint32_t)
            );
            chip8_state.draw_flag = 0;
        }

        SDL_RenderCopy(renderer, texture, NULL, NULL);

        SDL_RenderPresent(renderer);
        SDL_Delay(16);
    }

    SDL_DestroyTexture(texture);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
    
}
