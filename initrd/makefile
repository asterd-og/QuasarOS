ELFFILES = $(shell find * -type f -executable)

all: compile
	./quasfs hello.txt kfont.psf file1.txt $(filter-out quasfs quasfsview,$(ELFFILES))

compile:
	$(MAKE) -C ../exec
	gcc quasfs.c -o quasfs

view:
	gcc quasfsview.c -o quasfsview
	./quasfsview quasfs.fs

clean:
	rm -fdr quasfsview quasfs quasfs.fs $(ELFFILES)
	$(MAKE) -C ../exec clean