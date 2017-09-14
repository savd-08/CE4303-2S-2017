CC      = gcc
FILES   = SequencialServer.c 
OUT_EXE = SequencialServer
LDFLAGS = -pthread -lrt
build:
	$(CC) $(FILES) -o $(OUT_EXE) $(LDFLAGS)

clean:
	rm $(OUT_EXE) 

rebuild: clean build
