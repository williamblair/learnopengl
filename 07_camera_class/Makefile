CC = g++
SOURCES = ./*.cpp

CFLAGS = -g -D_DEBUG
LIBS = -lGLEW -lGLU -lGL -lglfw

TARGET = main

all: $(SOURCES)
	$(CC) $(CFLAGS) $(SOURCES) -o $(TARGET) $(LIBS)

# dri prime required for my graphics driver on ubuntu
run:
	DRI_PRIME=1 ./$(TARGET)
	
