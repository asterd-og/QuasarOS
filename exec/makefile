all:
	gcc -m64 -c app.c -o app.o -ffreestanding -std=gnu99
	gcc -m64 -c shell.c -o shell.o -ffreestanding -std=gnu99
	gcc -m64 -c ls.c -o ls.o -ffreestanding -std=gnu99
	gcc -m64 -c cat.c -o cat.o -ffreestanding -std=gnu99
	gcc -Tlinker.ld -o app -nostdlib app.o
	gcc -Tlinker.ld -o shell -nostdlib shell.o
	gcc -Tlinker.ld -o ls -nostdlib ls.o
	gcc -Tlinker.ld -o cat -nostdlib cat.o
	cp app ../initrd/app
	cp shell ../initrd/shell
	cp ls ../initrd/ls
	cp cat ../initrd/cat

clean:
	rm -fdr *.o shell app ls cat