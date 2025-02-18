CC=g++
CFLAGS=-I$(IDIR) 
DEGUB_FLAGS=-Wall -g

SDIR=src
ODIR=obj
IDIR =inc

LIBS=-lraylib

_DEPS = ecs.hpp dungeonGen.hpp 
DEPS = $(patsubst %,$(IDIR)/%,$(_DEPS))

_OBJ = main.o ecs.o dungeonGen.o
OBJ = $(patsubst %,$(ODIR)/%,$(_OBJ))

$(ODIR)/%.o: $(SDIR)/%.cpp $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

game.exe: $(OBJ)
	$(CC) -o $@ $^ $(CFLAGS) $(LIBS)

run:
	make
	./game.exe

debug: $(OBJ)
	$(CC) -o $@ $^ $(CFLAGS) $(LIBS) $(DEGUB_FLAGS)
	gdb ./game.exe

.PHONY: clean

clean:
	rm -f $(ODIR)/*.o *~ core $(INCDIR)/*~ 
