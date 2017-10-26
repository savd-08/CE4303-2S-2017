all: calc

roboticFinger.tab.c roboticFinger.tab.h:	roboticFinger.y
	bison -d roboticFinger.y

lex.yy.c: roboticFinger.l roboticFinger.tab.h
	flex roboticFinger.l

calc: lex.yy.c roboticFinger.tab.c roboticFinger.tab.h
	gcc -o roboticFinger roboticFinger.tab.c lex.yy.c RoboticFinger.c manageStreamProcess.c -w

clean:
	rm roboticFinger roboticFinger.tab.c lex.yy.c roboticFinger.tab.h
