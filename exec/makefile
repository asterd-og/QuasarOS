all:
	gcc -m64 -c app.c -o app.o -ffreestanding -std=gnu99
	gcc -m64 -c shell.c -o shell.o -ffreestanding -std=gnu99
	gcc -Tlinker.ld -o app -nostdlib app.o
	gcc -Tlinker.ld -o shell -nostdlib shell.o
	cp app ../initrd/app
	cp shell ../initrd/shell

clean:
	rm -fdr *.o shell app