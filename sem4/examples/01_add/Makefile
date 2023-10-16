all: main.o add2.o
	gcc -ggdb main.o add2.o

main.o: main.c
	gcc -c -ggdb main.c

add2.o:
	gcc -c -ggdb add2.S

clean:
	rm -rf *.o a.out

