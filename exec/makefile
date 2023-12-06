all:
	gcc -m64 -nostdlib -fPIC -fPIE -C main.c -o bin
	objcopy -O binary -j .text bin bin

clean:
	rm -fdr bin main.o