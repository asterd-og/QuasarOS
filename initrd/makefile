all:
	$(MAKE) -C ../exec
	gcc quasfs.c -o quafs
	./quafs hello.txt shell app kfont.psf

view:
	gcc quasfsview.c -o quasfsview
	./quasfsview quasfs.fs