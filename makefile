CFLAGS = -O3 -Wall -std=c++17 -I./lib/glm -I./lib/stb_master -I./lib/glc-1.0.0 -fopenmp
LFLAGS = -fopenmp

CC	 = g++
EXE = ./ray_tracing.exe

ODIR = obj
IDIR = .

_DEPS = rt/light.h \
		rt/primitive.h \
		rt/rt_types.h
DEPS = $(patsubst %,$(IDIR)/%,$(_DEPS))

_OBJ = 	main.cpp.o \
		rt/src/primitive.cpp.o \
		rt/src/sphere.cpp.o \
		rt/src/distancesphere.cpp.o \
		rt/src/infplane.cpp.o \
		rt/src/light.cpp.o
OBJ = $(patsubst %,$(ODIR)/%,$(_OBJ))

$(ODIR)/%.o: % $(DEPS)
	py ./makedir.py $(OBJ)
	$(CC) -c $< -o $@ $(CFLAGS)

full:
	make build

build: $(OBJ)
	$(CC) -o $(EXE) $(OBJ) $(LFLAGS)

run:
	$(EXE)