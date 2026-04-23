CC = gcc
CFLAGS = -Wall -I./include

all: lga.exe


lga.exe: src/*.c
	$(CC) $(CFLAGS) src/*.c -o lga.exe -lm


clean:
	rm -f lga.exe result.txt grid.bin output/*.ppm