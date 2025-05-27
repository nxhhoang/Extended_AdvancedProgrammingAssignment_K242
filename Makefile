# Makefile for compiling Tetris

CXX = g++
CXXFLAGS = -Iinclude
SRC = main.cpp $(wildcard src/*.cpp)
OUT = tetris.out

all: $(OUT)

$(OUT): $(SRC)
	$(CXX) $(CXXFLAGS) $(SRC) -o $(OUT)

run: all
	./$(OUT)

clean:
	rm -f $(OUT)
