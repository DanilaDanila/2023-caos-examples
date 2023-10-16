all: main.o sum.o
	gcc -ggdb main.o sum.o

main.o: main.c
	gcc -ggdb -c main.c

sum.o: sum.S
	gcc -ggdb -c sum.S

clean:
	rm main.o sum.o a.out
