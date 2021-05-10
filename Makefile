BIN=fafterm
SRC_DIR=src
SRC_FILES=$(SRC_DIR)/main.cpp $(SRC_DIR)/terminal.cpp $(SRC_DIR)/render.cpp $(SRC_DIR)/glBoilerplate.cpp $(SRC_DIR)/shader.cpp
CXX=clang++
LDFLAGS=-lglfw -lGLEW -lGLU -lGL -lrt -lXrandr -lXxf86vm -lXi -lXinerama -lX11 -lutil -lfreetype
INCFLAGS=-Iinclude/ -I/usr/include/freetype2
CFLAGS=-Wextra -Wall -std=c++11

all:
	$(CXX) $(LDFLAGS) $(INCFLAGS) $(CFLAGS) -o $(BIN) $(SRC_FILES)