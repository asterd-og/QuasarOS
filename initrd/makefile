all:
	$(MAKE) -C ../exec
	gcc quasfs.c -o quasfs
	./quasfs hello.txt shell app kfont.psf

view:
	gcc quasfsview.c -o quasfsview
	./quasfsview quasfs.fs

clean:
	rm -fdr quasfsview quasfs quasfs.fs shell app
	$(MAKE) -C ../exec clean