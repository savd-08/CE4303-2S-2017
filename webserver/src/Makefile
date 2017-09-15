CC      = gcc
FILES   = client.c
OUT_EXE = client
LDFLAGS = -pthread -lrt
build:
	$(CC) $(FILES) -o $(OUT_EXE) $(LDFLAGS)

clean:
	rm $(OUT_EXE) 

rebuild: clean build
