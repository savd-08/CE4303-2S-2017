IDIR = ./include
SDIR = ./src
ODIR = ./obj
LDIR = ./lib

CC = gcc
CXX = g++

CFLAGS = -I$(IDIR)
CPPFLAGS = -I$(IDIR)

EXEC = webserver

_DEPS = HttpStructure.h 
DEPS = $(patsubst %,$(IDIR)/%,$(_DEPS))

_OBJ = $(EXEC).o 
OBJ = $(patsubst %,$(ODIR)/%,$(_OBJ))

LIBS = -lpthread

$(ODIR)/%.o: $(SDIR)/%.c $(DEPS)
	$(CC) -c -o $@ $< $(CPPFLAGS)

$(EXEC): $(OBJ)
	gcc -o $@ $^ $(CPPFLAGS) $(LIBS)

.PHONY: clean

clean:
	rm -f $(ODIR)/*.o $(EXEC)
