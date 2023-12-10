all:
	$(MAKE) -C ../exec
	gcc quasfs.c -o quafs
	./quafs hello.txt shell app

view:
	gcc quasfsview.c -o quasfsview
	./quasfsview quasfs.fs