CC = g++
CFLAGS = -g -std=c++11
LIBS = -lglfw -lGLU -lGL -lassimp -ldl
INCDIRS = -I../ -I./
LIBDIRS = -L/usr/lib/x86_64-linux-gnu
TARGET = main
SOURCES = main.cpp ../glad.c

all:
	$(CC) $(CFLAGS) $(SOURCES) -o $(TARGET) $(INCDIRS) $(LIBDIRS) $(LIBS)

