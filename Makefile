CC = g++
CFLAGS = -Wall -std=c++17
LIBS = -lSDL2

# Pliki zrodlowe
FILES = main.cpp chip8.cpp

# Nazwa programu
OUTPUT = chip8

all:
	$(CC) $(FILES) $(CFLAGS) $(LIBS) -o $(OUTPUT)

clean:
	rm -f $(OUTPUT)
