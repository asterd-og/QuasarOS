all:
	$(MAKE) -C ../exec
	gcc quasfs.c -o quasfs
	./quasfs hello.txt shell app kfont.psf ls cat

view:
	gcc quasfsview.c -o quasfsview
	./quasfsview quasfs.fs

clean:
	rm -fdr quasfsview quasfs quasfs.fs shell app ls cat
	$(MAKE) -C ../exec clean