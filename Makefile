CC=g++
CFLAGS=-I$(IDIR) 
DEGUB_FLAGS=-Wall -Wextra -g

SDIR=src
ODIR=obj
IDIR =inc

LIBS=-lraylib

_DEPS = ecs.hpp dungeonGen.hpp quadtree.hpp vecUtils.hpp
DEPS = $(patsubst %,$(IDIR)/%,$(_DEPS))

_OBJ = main.o ecs.o dungeonGen.o quadtree.o vecUtils.o
OBJ = $(patsubst %,$(ODIR)/%,$(_OBJ))

$(ODIR)/%.o: $(SDIR)/%.cpp $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

game.exe: $(OBJ)
	$(CC) -o $@ $^ $(CFLAGS) $(LIBS)

run: game.exe
	./game.exe

debug: game.exe
	gdb ./game.exe

.PHONY: clean

clean:
	rm -f $(ODIR)/*.o *~ core $(INCDIR)/*~ 
