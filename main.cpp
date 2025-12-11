#include <SDL2/SDL.h>
#include <iostream>
#include "chip8.h"

// Ustawienia ekranu
const int SCREEN_WIDTH = 640;
const int SCREEN_HEIGHT = 320;

Chip8 myChip8;

int main(int argc, char* argv[]) {
    // 1. Sprawdź argumenty
    if (argc < 2) {
        std::cout << "Uzycie: ./chip8 <plik_z_gra>" << std::endl;
        return 1;
    }

    // 2. Wczytaj grę
    if (!myChip8.loadGame(argv[1])) {
        return 1;
    }

    // 3. Inicjalizacja SDL (Grafika)
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        std::cout << "Blad SDL: " << SDL_GetError() << std::endl;
        return 1;
    }

    SDL_Window* window = SDL_CreateWindow("CHIP-8 Emulator",
                                          SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
                                          SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);

    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, 0);
    SDL_Texture* sdlTexture = SDL_CreateTexture(renderer,
                                                SDL_PIXELFORMAT_ARGB8888,
                                                SDL_TEXTUREACCESS_STREAMING,
                                                64, 32);

    uint32_t pixels[2048];
    bool quit = false;
    SDL_Event e;

    // --- PĘTLA GŁÓWNA ---
    while (!quit) {

        // A. OBSŁUGA KLAWIATURY (TEGO BRAKOWAŁO!)
        while (SDL_PollEvent(&e) != 0) {
            if (e.type == SDL_QUIT) {
                quit = true;
            }
            
            // Wciśnięcie klawisza
            if (e.type == SDL_KEYDOWN) {
                switch (e.key.keysym.sym) {
                    case SDLK_1: myChip8.key[0x1] = 1; break;
                    case SDLK_2: myChip8.key[0x2] = 1; break;
                    case SDLK_3: myChip8.key[0x3] = 1; break;
                    case SDLK_4: myChip8.key[0xC] = 1; break; // 4 to C

                    case SDLK_q: myChip8.key[0x4] = 1; break;
                    case SDLK_w: myChip8.key[0x5] = 1; break;
                    case SDLK_e: myChip8.key[0x6] = 1; break;
                    case SDLK_r: myChip8.key[0xD] = 1; break; // R to D

                    case SDLK_a: myChip8.key[0x7] = 1; break;
                    case SDLK_s: myChip8.key[0x8] = 1; break;
                    case SDLK_d: myChip8.key[0x9] = 1; break;
                    case SDLK_f: myChip8.key[0xE] = 1; break; // F to E

                    case SDLK_z: myChip8.key[0xA] = 1; break;
                    case SDLK_x: myChip8.key[0x0] = 1; break;
                    case SDLK_c: myChip8.key[0xB] = 1; break;
                    case SDLK_v: myChip8.key[0xF] = 1; break;
                }
            }
            // Puszczenie klawisza
            if (e.type == SDL_KEYUP) {
                switch (e.key.keysym.sym) {
                    case SDLK_1: myChip8.key[0x1] = 0; break;
                    case SDLK_2: myChip8.key[0x2] = 0; break;
                    case SDLK_3: myChip8.key[0x3] = 0; break;
                    case SDLK_4: myChip8.key[0xC] = 0; break;

                    case SDLK_q: myChip8.key[0x4] = 0; break;
                    case SDLK_w: myChip8.key[0x5] = 0; break;
                    case SDLK_e: myChip8.key[0x6] = 0; break;
                    case SDLK_r: myChip8.key[0xD] = 0; break;

                    case SDLK_a: myChip8.key[0x7] = 0; break;
                    case SDLK_s: myChip8.key[0x8] = 0; break;
                    case SDLK_d: myChip8.key[0x9] = 0; break;
                    case SDLK_f: myChip8.key[0xE] = 0; break;

                    case SDLK_z: myChip8.key[0xA] = 0; break;
                    case SDLK_x: myChip8.key[0x0] = 0; break;
                    case SDLK_c: myChip8.key[0xB] = 0; break;
                    case SDLK_v: myChip8.key[0xF] = 0; break;
                }
            }
        }

        // B. EMULACJA CPU (Przyśpieszenie!)
        // Robimy 10 cykli procesora na jedną klatkę ekranu (60FPS x 10 = 600Hz)
        // Bez tego Tetris byłby niegrywalnie wolny.
        for (int i = 0; i < 4; ++i) {
            myChip8.emulateCycle();
        }

        // C. RYSOWANIE
        if (myChip8.drawFlag) {
            myChip8.drawFlag = false;
            for (int i = 0; i < 2048; ++i) {
                uint8_t pixel = myChip8.gfx[i];
                pixels[i] = (pixel == 1) ? 0xFF00FF00 : 0xFF000000;
            }
            SDL_UpdateTexture(sdlTexture, NULL, pixels, 64 * sizeof(uint32_t));
            SDL_RenderClear(renderer);
            SDL_RenderCopy(renderer, sdlTexture, NULL, NULL);
            SDL_RenderPresent(renderer);
        }

        // D. UTRZYMANIE 60 FPS DLA TIMERÓW
        SDL_Delay(16);
    }

    // Sprzątanie
    SDL_DestroyTexture(sdlTexture);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}
