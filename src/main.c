#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define SDL_MAIN_HANDLED
#include <SDL2/SDL.h>
#include "chip8.h"

const int CHIP8_WIDTH = 64;
const int CHIP8_HEIGHT = 32;
const int SCALE_FACTOR = 10;

int main(int argc, char* argv[])
{
    sran(time(NULL)); // Semear o gerador aleatório

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
        SDL_WINDOWPOS_UNDEFINED,
        SDL_WINDOWPOS_UNDEFINED,
        CHIP8_WIDTH * SCALE_FACTOR,
        CHIP8_HEIGHT * SCALE_FACTOR,
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

        chip8_emulate_cycle(&chip8_state);

        SDL_SetRenderDrawColor(renderer, 5, 5, 30, 255);
        SDL_RenderClear(renderer);

        SDL_RenderPresent(renderer);
        SDL_Delay(16);
        
    }

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
    
}
