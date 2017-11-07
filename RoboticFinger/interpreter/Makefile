CC = gcc
LIB = 
CFLAGS = -I ../include
LFLAGS = 

roboticFinger: lex.yy.c sim.tab.c
	$(CC) $(CFLAGS) -g -w lex.yy.c sim.tab.c -o roboticFinger $(LFLAGS)

sim.tab.c: roboticFinger.y
	yacc -b sim -d roboticFinger.y

lex.yy.c: sim.tab.c roboticFinger.l
	lex roboticFinger.l

clean: 
	rm -f lex.yy.c sim.tab.c sim.tab.h roboticFinger error_log.txt
