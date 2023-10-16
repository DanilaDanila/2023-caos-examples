all: main.o foo.o
	gcc -ggdb foo.o main.o

main.o: main.c
	gcc -ggdb -c main.c

foo.o: foo.S
	gcc -ggdb -c foo.S

clean:
	rm foo.o main.o
