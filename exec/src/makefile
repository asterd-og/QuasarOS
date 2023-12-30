# User controllable C flags.
CFLAGS = -m64 -I../lib -ffreestanding -std=gnu99
LD_FLAGS = -Tlinker.ld -nostdlib

CFILES = $(shell find -L . -type f -name '*.c')
OBJ = $(CFILES:.c=.o)
OUT = $(OBJ:.o=.q)

LIBS =

all: $(OBJ) $(OUT) clean
	@echo all done.

%.o: %.c
	gcc $(CFLAGS) -c $< -o $@

%.q: %.o
	gcc $(LIBS) $< ../syslib/ubsan.o $(LD_FLAGS) -o ../../initrd/$(basename $@)

clean:
	rm -fdr $(OBJ) $(OUT)