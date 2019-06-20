#CC = g++
CC = mingw32-g++
SOURCES = ./*.cpp

CFLAGS = -g -D_DEBUG -std=c++11
# LIBS = -lGLEW -lGLU -lGL -lglfw
LIBS = -lglew32 -lglfw3 -lopengl32 -lglu32 -lgdi32

TARGET = main

all: $(SOURCES)
	$(CC) $(CFLAGS) $(SOURCES) -o $(TARGET) $(LIBS)

# dri prime required for my graphics driver on ubuntu
run:
	DRI_PRIME=1 ./$(TARGET)
	
