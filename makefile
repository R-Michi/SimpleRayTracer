CFLAGS = -O3 -Wall -std=c++17 -I./lib/glm -I./lib/stb_master -I./lib/glc-1.0.0 -fopenmp
LFLAGS = -fopenmp

CC	 = g++
EXE = ./ray_tracing.exe

ODIR = obj
IDIR =.

_DEPS = light.hpp
DEPS = $(patsubst %,$(IDIR)/%,$(_DEPS))

_OBJ = main.cpp.o
OBJ = $(patsubst %,$(ODIR)/%,$(_OBJ))

$(ODIR)/%.o: % $(DEPS)
	mkdir obj || cd .
	$(CC) -c $< -o $@ $(CFLAGS)

full:
	make link

build: $(OBJ)
	$(CC) -o $(EXE) $(OBJ) $(LFLAGS)

run:
	$(EXE)