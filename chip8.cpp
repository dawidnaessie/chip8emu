#include "chip8.h"
#include <cstring> // Potrzebne do memset (szybkie czyszczenie pamięci)
#include <cstdlib> // Do rand()
#include <ctime>   // Do time()
#include <fstream>
#include <iostream>
// To są "rysunki" znaków 0-F. Każdy znak ma 5 bajtów wysokości.
// Np. 0xF0 to linia pozioma **** (11110000)
unsigned char chip8_fontset[80] =
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

Chip8::Chip8() {
    initialize();
}

void Chip8::initialize() {
    // 1. Ustawienie PC na 0x200 (tam zaczynają się gry w CHIP-8)
    pc = 0x200;
    opcode = 0;
    I = 0;
    sp = 0;

    // 2. Wyczyszczenie pamięci, rejestrów i stosu (wypełnienie zerami)
    // memset to szybka komenda C: (wskaźnik, wartość, ile_bajtow)
    memset(memory, 0, 4096);
    memset(V, 0, 16);
    memset(stack, 0, 16 * sizeof(uint16_t));
    memset(gfx, 0, 64 * 32 * sizeof(uint32_t));

    // 3. Załadowanie czcionki do pamięci (od adresu 0 do 79)
    for(int i = 0; i < 80; ++i) {
        memory[i] = chip8_fontset[i];
    }

    // 4. Reset timerów
    delay_timer = 0;
    sound_timer = 0;
    
    // 5. Reset flagi rysowania
    drawFlag = true;
    
    // Inicjalizacja generatora liczb losowych (przydatne w grach)
    srand(time(NULL));
}

void Chip8::emulateCycle() {
    // --- 1. FETCH (Pobranie instrukcji) ---
    // Pobieramy 2 bajty z pamięci i sklejamy je w jeden opcode (Big Endian)
    opcode = (memory[pc] << 8) | memory[pc + 1];

    // Zwiększamy PC o 2, aby wskazywało na kolejną instrukcję
    // (Niektóre instrukcje skoku mogą to nadpisać później)
    pc += 2;

    // --- 2. DECODE (Rozkodowanie zmiennych pomocniczych) ---
    uint8_t X = (opcode & 0x0F00) >> 8;
    uint8_t Y = (opcode & 0x00F0) >> 4;
    uint8_t NN = opcode & 0x00FF;
    uint16_t NNN = opcode & 0x0FFF;

    // --- 3. EXECUTE (Wykonanie) ---
    switch (opcode & 0xF000) {

        case 0x0000:
            if (opcode == 0x00E0) {
                // 00E0: CLS (Wyczyść ekran)
                memset(gfx, 0, 64 * 32 * sizeof(uint32_t));
                drawFlag = true;
            } else if (opcode == 0x00EE) {
                // 00EE: RET (Powrót z podprogramu)
                pc = stack[--sp];
            }
            break;

        case 0x1000: // 1NNN: Skok bezwarunkowy (JMP)
            pc = NNN;
            break;

        case 0x2000: // 2NNN: Wywołanie podprogramu (CALL)
            stack[sp++] = pc;
            pc = NNN;
            break;

        case 0x3000: // 3XNN: Pomiń jeśli VX == NN
            if (V[X] == NN) pc += 2;
            break;

        case 0x4000: // 4XNN: Pomiń jeśli VX != NN
            if (V[X] != NN) pc += 2;
            break;

        case 0x5000: // 5XY0: Pomiń jeśli VX == VY
            if (V[X] == V[Y]) pc += 2;
            break;

        case 0x6000: // 6XNN: Ustaw VX = NN
            V[X] = NN;
            break;

        case 0x7000: // 7XNN: Dodaj NN do VX (bez flagi przeniesienia)
            V[X] += NN;
            break;

        case 0x8000: // Instrukcje matematyczne i bitowe
            switch (opcode & 0x000F) {
                case 0x0: V[X] = V[Y]; break;
                case 0x1: V[X] |= V[Y]; break;
                case 0x2: V[X] &= V[Y]; break;
                case 0x3: V[X] ^= V[Y]; break;
                case 0x4: { // ADD
                     uint16_t sum = V[X] + V[Y];
                     V[0xF] = (sum > 255) ? 1 : 0;
                     V[X] = sum & 0xFF;
                } break;
                case 0x5: // SUB (VX - VY)
                    V[0xF] = (V[X] >= V[Y]) ? 1 : 0;
                    V[X] -= V[Y];
                    break;
                case 0x6: // SHR
                    V[0xF] = V[X] & 0x1;
                    V[X] >>= 1;
                    break;
                case 0x7: // SUBN (VY - VX)
                    V[0xF] = (V[Y] >= V[X]) ? 1 : 0;
                    V[X] = V[Y] - V[X];
                    break;
                case 0xE: // SHL
                    V[0xF] = (V[X] >> 7) & 0x1;
                    V[X] <<= 1;
                    break;
            }
            break;

        case 0x9000: // 9XY0: Pomiń jeśli VX != VY
            if (V[X] != V[Y]) pc += 2;
            break;

        case 0xA000: // ANNN: Ustaw I = NNN
            I = NNN;
            break;

        case 0xB000: // BNNN: Skok do NNN + V0
            pc = NNN + V[0];
            break;

        case 0xC000: // CXNN: Losowa liczba (RAND & NN) - WAŻNE DLA GIER!
            V[X] = (rand() % 256) & NN;
            break;

        case 0xD000: // DXYN: Rysowanie (DRAW)
        {
            uint8_t x = V[X];
            uint8_t y = V[Y];
            uint8_t height = opcode & 0x000F;
            uint8_t pixel;

            V[0xF] = 0; // Reset flagi kolizji

            for (int yline = 0; yline < height; yline++) {
                pixel = memory[I + yline]; // Pobierz linię sprite'a z pamięci
                for (int xline = 0; xline < 8; xline++) {
                    if ((pixel & (0x80 >> xline)) != 0) {
                        // Sprawdzamy czy pixel na ekranie jest już zapalony (kolizja)
                        // Używamy modulo (x + xline) % 64 dla zawijania ekranu
                        int index = ((x + xline) % 64) + ((y + yline) % 32) * 64;
                        
                        if (gfx[index] == 1) {
                            V[0xF] = 1; // Kolizja wykryta!
                        }
                        gfx[index] ^= 1; // Rysowanie metodą XOR
                    }
                }
            }
            drawFlag = true;
        }
        break;

        case 0xE000: // Obsługa klawiatury
            if ((opcode & 0x00FF) == 0x9E) { // EX9E: Pomiń jeśli wciśnięty
                if (key[V[X]] != 0) pc += 2;
            } else if ((opcode & 0x00FF) == 0xA1) { // EXA1: Pomiń jeśli NIE wciśnięty
                if (key[V[X]] == 0) pc += 2;
            }
            break;

        case 0xF000: // Timery i pamięć
            switch (opcode & 0x00FF) {
                case 0x07: V[X] = delay_timer; break;
                case 0x15: delay_timer = V[X]; break;
                case 0x18: sound_timer = V[X]; break;
                case 0x1E: I += V[X]; break;
                case 0x29: I = V[X] * 5; break; // Ustaw I na znak czcionki
                case 0x33: // BCD
                    memory[I]     = V[X] / 100;
                    memory[I + 1] = (V[X] / 10) % 10;
                    memory[I + 2] = V[X] % 10;
                    break;
                case 0x55: // Zrzut rejestrów do pamięci
                    for (int i = 0; i <= X; ++i) memory[I + i] = V[i];
                    break;
                case 0x65: // Odczyt rejestrów z pamięci
                    for (int i = 0; i <= X; ++i) V[i] = memory[I + i];
                    break;
            }
            break;

        default:
            printf("Nieznany opcode: 0x%X\n", opcode);
    }

    // --- 4. Aktualizacja Timerów ---
    if (delay_timer > 0) --delay_timer;
    if (sound_timer > 0) --sound_timer;
}

bool Chip8::loadGame(const char* filename) {
    std::cout<< "Proba wczytania: " << filename << std::endl;
    
    //otwieramy plik binarny i ustawiamy kursor na koniec (ate)
    std::ifstream file(filename, std::ios::binary | std::ios::ate);
        
    if(file.is_open()){
        // Pobieramy rozmiar pliku
        std::streampos size = file.tellg();

        //tymaczasowe miejsce na dane
        char* buffer = new char[size];

        file.seekg(0, std::ios::beg);
        file.read(buffer, size);
        file.close();
        
        //kopia danych bufor do emulatora
        //PAMIETAJ!! START OD 0x200 = 512 start pamieci dla gry
        for (long i = 0; i < size ; ++i) {
            memory[0x200 + i] = buffer[i];
        }
        //sprzatanie
        delete [] buffer;


        std::cout << "ROM zaladowany pomyslnie! Rozmiar: " << size 
        << " bajtow." << std::endl;

        return true;
    }else{
        std::cerr << "BLAD: Nie mozna otworzyc pliku!" << std::endl;
        return false;
    }
}
