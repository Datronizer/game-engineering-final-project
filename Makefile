CXX      = g++
CXXFLAGS = -std=c++17 -Wall -I include -I include/objects -I include/core

SRCS = $(wildcard src/*.cpp src/objects/*.cpp src/core/*.cpp)

ifeq ($(OS),Windows_NT)
	TARGET = builds/game.exe
	LIBS   = lib/libraylibdll.a -lopengl32 -lgdi32 -lwinmm
else
	TARGET = builds/game
	LIBS   = -lraylib -lGL -lm -lpthread -ldl -lrt -lX11
endif

.PHONY: all clean run

all: $(TARGET)

$(TARGET): $(SRCS)
	$(CXX) $(CXXFLAGS) $^ $(LIBS) -o $@

run: $(TARGET)
	./$(TARGET)

clean:
	rm -f $(TARGET)
