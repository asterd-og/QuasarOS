all:
	gcc -m64 -c main.c -o main.o -ffreestanding -std=gnu99
	gcc -Tlinker.ld -o bin -nostdlib main.o

clean:
	rm -fdr bin main.o elf2.o bin2