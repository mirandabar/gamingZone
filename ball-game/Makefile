CXX      = g++
CXXFLAGS = -std=c++17 -Wall -Wextra -O2 -fdiagnostics-color=always -g -Iinclude
LDFLAGS  = -lX11 -lSDL2 -lSDL2_mixer -lavcodec -lavformat -lavutil -lswscale

SRC = src/*.cpp
OUT = build/ball-game

all:
	$(CXX) $(CXXFLAGS) $(SRC) -o $(OUT) $(LDFLAGS) 

clean:
	rm -f $(OUT)