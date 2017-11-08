SOURCE = library.c
HEADER = library.h
CC = gcc
RANLIB = ranlib
LD = ld
AR = ar r
LIB_A = library.a 
OBJECT = library.o

all: estatica

estatica: $(SOURCE) $(HEADER)
		@$(CC) -c $(SOURCE)
		@$(AR) $(LIB_A) $(OBJECT)
		@$(RANLIB) $(LIB_A)
		@echo "Creando la librería"

clean:
		@rm *.a $(OBJECT)
		@echo "Limpiando" 
