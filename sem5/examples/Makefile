GCC=/home/danila/caos_all/2023_caos/sem5/examples/gcc-linaro/bin/arm-linux-gnueabi-gcc
SYSROOT=/home/danila/caos_all/2023_caos/sem5/examples/sysroot

all: arm.out
	echo "arm.out..."

arm.out: main.o
	$(GCC) *.o -o arm.out

main.o: main.c
	$(GCC) -c main.c

run: arm.out 
	qemu-arm -L $(SYSROOT) ./arm.out

clean:
	rm *.o *.out

