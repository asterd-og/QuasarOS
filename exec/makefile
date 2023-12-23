# User controllable C flags.
CFLAGS = -m64 -Ilib -ffreestanding -std=gnu99
LD_FLAGS = -Tlinker.ld -nostdlib

override CFILES := $(shell find -L . -type f -name '*.c')
override OBJ := $(CFILES:.c=.o)
override OUT := $(OBJ:.o=.q)

all: $(OBJ) $(OUT) clean
	@echo all done.

%.o: %.c
	gcc $(CFLAGS) -c $< -o $@

%.q: %.o
	gcc $< syslib/ubsan.o $(LD_FLAGS) -o ../initrd/$(basename $@)

clean:
	rm -fdr $(OBJ) $(OUT)