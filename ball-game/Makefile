CXX      = g++
CXXFLAGS = -std=c++17 -Wall -Wextra -O2 -fdiagnostics-color=always -g -Iinclude $(shell pkg-config --cflags sdl2 SDL2_ttf)
LDFLAGS  = -lSDL2_mixer -lavcodec -lavformat -lavutil -lswscale $(shell pkg-config --libs sdl2 SDL2_ttf)

SRC = src/*.cpp
OUT = build/ball-game

all:
	$(CXX) $(CXXFLAGS) $(SRC) -o $(OUT) $(LDFLAGS) 

clean:
	rm -f $(OUT)