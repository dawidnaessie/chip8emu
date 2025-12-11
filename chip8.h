#ifndef CHIP8_H
#define CHIP8_H

#include <cstdint> // Potrzebne do uint8_t, uint16_t (konkretne rozmiary zmiennych)
#include <fstream>
#include <vector>

class Chip8 {
public:
    // Konstruktor: tu zresetujemy maszynę przy starcie
    Chip8(); 

    // Funkcja wykonująca jeden cykl procesora (serce emulatora)
    void emulateCycle();

    // Funkcja ładująca grę do pamięci
    bool loadGame(const char* filename);

    // Klawiatura: 16 klawiszy (HEX: 0-F)
    uint8_t key[16];

    // Ekran: 64x32 piksele (czarno-biały)
    // Używamy uint32_t dla łatwiejszej współpracy z SDL (każdy piksel to 32 bity RGBA)
    // Ale logicznie to tylko 0 lub 1.
    uint32_t gfx[64 * 32];

    // Flaga: czy trzeba odświeżyć ekran w tym cyklu?
    bool drawFlag;

private:
    // --- KOMPONENTY PROCESORA ---

    // Pamięć: 4096 bajtów (4KB)
    uint8_t memory[4096];

    // Rejestry: 16 sztuk, 8-bitowe (V0, V1... VF)
    // Służą do przechowywania danych tymczasowych
    uint8_t V[16];

    // Rejestr indeksowy (Index Register)
    // Wskazuje na konkretny adres w pamięci
    uint16_t I;

    // Licznik programu (Program Counter)
    // Mówi procesorowi: "którą instrukcję teraz wykonać?"
    uint16_t pc;

    // Stos (Stack) i wskaźnik stosu (Stack Pointer)
    // Potrzebne, gdy wywołujemy funkcje (skoki do podprogramów)
    uint16_t stack[16];
    uint16_t sp;

    // Opcode: Aktualna instrukcja (2 bajty)
    uint16_t opcode;

    // Timery: Systemowy (delay) i Dźwiękowy (sound)
    // Odliczają do zera z częstotliwością 60Hz
    uint8_t delay_timer;
    uint8_t sound_timer;

    // Funkcja czyszcząca pamięć
    void initialize();
};

#endif
