AS = ca65
CC = cc65
CL = cl65
LD = ld65
SP = sp65

all: main.c
	$(CL) -t c64 -Ois -o 64fetch.prg main.c detect.c drive.c detectasm.s

clean:
	rm *.o
