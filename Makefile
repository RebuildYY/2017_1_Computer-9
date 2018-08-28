all	: tetris.o
	#gcc -lncurses tetris.c
	gcc tetris.c -lncurses

clean:
	rm a.out *.o
