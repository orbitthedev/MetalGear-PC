#include <SDL.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include "memory.h"
#include "vdp.h"
#include "game.h"

#define MSX2_WIDTH 256
#define MSX2_HEIGHT 192
#define SCALE 2

int main(int argc, char* argv[]) {
    // Suppress unused parameter warnings for argc/argv
    (void)argc;
    (void)argv;

    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO | SDL_INIT_GAMECONTROLLER) != 0) {
        printf("SDL_Init Error: %s\n", SDL_GetError());
        return 1;
    }

    SDL_Window* window = SDL_CreateWindow(
        "Metal Gear - MSX PC Port",
        SDL_WINDOWPOS_CENTERED,
        SDL_WINDOWPOS_CENTERED,
        MSX2_WIDTH * SCALE,
        MSX2_HEIGHT * SCALE,
        SDL_WINDOW_SHOWN
    );

    if (window == NULL) {
        printf("SDL_CreateWindow Error: %s\n", SDL_GetError());
        SDL_Quit();
        return 1;
    }

    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    if (renderer == NULL) {
        SDL_DestroyWindow(window);
        printf("SDL_CreateRenderer Error: %s\n", SDL_GetError());
        SDL_Quit();
        return 1;
    }

    // Initialize Memory System (using Absolute Path to avoid Visual Studio build folder issues)
    if (!mem_init("C:/Users/Surya/metal-gear-pc/assets/banks")) {
        printf("Failed to initialize memory system!\n");
        return 1;
    }

    // Initialize Video Display Processor
    if (!vdp_init(renderer)) {
        printf("Failed to initialize VDP!\n");
        return 1;
    }

    // Initialize Game Engine
    game_init();

    // Render initial game state
    game_render();

    bool is_running = true;
    SDL_Event event;

    // Main Game Loop
    while (is_running) {
        // Event processing
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) {
                is_running = false;
            }
            if (event.type == SDL_KEYDOWN) {
                if (event.key.keysym.sym == SDLK_ESCAPE) {
                    is_running = false;
                }
                if (event.key.keysym.sym == SDLK_LEFT) {
                    game_input_left();
                }
                if (event.key.keysym.sym == SDLK_RIGHT) {
                    game_input_right();
                }
                if (event.key.keysym.sym == SDLK_UP) {
                    game_input_up();
                }
                if (event.key.keysym.sym == SDLK_DOWN) {
                    game_input_down();
                }
            }
        }

        // Logic processing
        game_update();

        // Rendering
        // Clear screen to an olive green color before drawing VDP
        SDL_SetRenderDrawColor(renderer, 40, 60, 20, 255);
        SDL_RenderClear(renderer);

        // Render game to VRAM
        game_render();

        // Render the MSX2 VDP Screen
        vdp_render(renderer);

        // Swap buffers
        SDL_RenderPresent(renderer);
    }

    // Cleanup
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}