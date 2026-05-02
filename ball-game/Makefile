CXX      = g++
CXXFLAGS = -std=c++17 -Wall -Wextra -O2 -fdiagnostics-color=always -g
LDFLAGS  = -lX11

SRC = src/*.cpp
OUT = build/ball-game

all:
	$(CXX) $(CXXFLAGS) $(SRC) -o $(OUT) $(LDFLAGS)

clean:
	rm -f $(OUT)