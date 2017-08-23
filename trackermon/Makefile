IDIR = ./include
SDIR = ./src
ODIR = ./obj
LDIR = ./lib

CC = gcc
CXX = g++

CFLAGS = -I$(IDIR)
CPPFLAGS = -I$(IDIR)

EXEC = trackermon

_DEPS = mon_utils.h config_utils.h
DEPS = $(patsubst %,$(IDIR)/%,$(_DEPS))

_OBJ = $(EXEC).o mon_utils.o config_utils.o
OBJ = $(patsubst %,$(ODIR)/%,$(_OBJ))

LIBS = 

$(ODIR)/%.o: $(SDIR)/%.c $(DEPS)
	$(CC) -c -o $@ $< $(CPPFLAGS)

$(EXEC): $(OBJ)
	gcc -o $@ $^ $(CPPFLAGS) $(LIBS)

.PHONY: clean

clean:
	rm -f $(ODIR)/*.o $(EXEC)
