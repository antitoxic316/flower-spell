


all: main

main: main.o ascii.o image.o
	cc -Wall main.o image.o ascii.o -o main `MagickCore-config --ldflags --libs` -lncurses -lm

main.o: main.c
	cc -Wall -g `MagickCore-config --cflags --cppflags` -c main.c -o main.o

image.o: image.c image.h
	cc -Wall -g `MagickCore-config --cflags --cppflags` -c image.c -o image.o

ascii.o: ascii.c ascii.h
	cc -Wall -g `MagickCore-config --cflags --cppflags` -c ascii.c -o ascii.o

clean:
	rm -f *.o main