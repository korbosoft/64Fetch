
all: main.c
	oscar64 -O3 -o=64fetch.prg *.c

clean:
	rm -f *.o *.prg *.asm *.int *.lbl *.map
