# CHIP-8 Emulator (C++)

A fully functional CHIP-8 interpreter written in C++. This project emulates the CPU, memory, timers, and graphics of the classic COSMAC VIP system.

![CHIP-8 Demo](<img width="647" height="349" alt="image" src="https://github.com/user-attachments/assets/7ac23eab-1dae-4e74-b73d-963543fa02cb" />
)

## 🚀 Features

* **Full Opcode Support:** Implements standard Chip-8 instruction set.
* **Graphics:** 64x32 monochrome display rendering.
* **Input:** Hex keypad mapped to modern keyboard layout.
* **Timers:** Functional Delay Timer and Sound Timer.
* **Portable:** Written in C++ using SDL2 for rendering and input handling.

## 🎮 Controls (Key Mapping)

The original Chip-8 used a 16-key hexadecimal keypad. This emulator maps them to the left side of your keyboard:

| Original (Hex) | Emulated (Keyboard) |
| :---: | :---: |
| **1 2 3 C** | **1 2 3 4** |
| **4 5 6 D** | **Q W E R** |
| **7 8 9 E** | **A S D F** |
| **A 0 B F** | **Z X C V** |

* **ESC**: Exit emulator.

## 🛠️ Build Instructions

### Prerequisites
You need a C++ compiler and the **SDL2** library installed.

**Linux (Debian/Ubuntu):**
```bash
sudo apt-get update
sudo apt-get install build-essential libsdl2-dev
```
**Windows:**

You can use Visual Studio with the SDL2 NuGet package.

Or MinGW with SDL2 development libraries linked.

Compilation
Clone the repository and compile using g++:

```bash
git clone [https://github.com/dawidnaessie/chip8emu.git](https://github.com/dawidnaessie/chip8emu.git)
cd chip8emu
```

# Example compilation command (adjust according to your file names)
g++ main.cpp chip8.cpp -lSDL2 -o chip8emu
# Usage
Run the emulator by passing the path to a ROM file as an argument:

```bash
./chip8emu roms/PONG
```
(Make sure you have a folder named roms containing valid Chip-8 rom files).

# Project Structure
main.cpp: Entry point, handles the main loop and SDL2 window.

chip8.cpp / .h: The core CPU logic (Memory, Registers, Opcode implementation).

roms/: Directory containing game files.
