# Compilation file for PC to debug the frontend
PROG_NAME = build/fbacapex.exe
HEADERS = capex.h capex_ecriture.h capex_lecture.h capex_pixel.h capex_tri.h
OBJS = capex.o
#LDFLAGS = -static

CC = gcc
CXX = g++

# output from sdl-config --cflags and sdl-config --libs
SDL_FLAGS = -D_GNU_SOURCE=1 -D_REENTRANT
SDL_LIBS = -L/local/lib -lSDL -lmingw32 -lSDL_image

CFLAGS = -g $(SDL_FLAGS)
CXXFLAGS = $(SDL_FLAGS) -Wall
LIBS = $(SDL_LIBS)

TARGET = $(PROG_NAME)

all : $(TARGET)

$(TARGET) : $(OBJS) $(HEADERS)
	$(CC) -g $(LDFLAGS) -o $(TARGET) $(OBJS) $(LIBS)

clean:
	/bin/rm -f *.o  *~ $(TARGET)
