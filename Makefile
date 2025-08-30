


all: main

main: main.o image.o ascii.o
	cc -Wall main.o image.o ascii.o -o main `MagickCore-config --ldflags --libs` -lncurses -lm

main.o: main.c
	cc -Wall -g `MagickCore-config --cflags --cppflags` -c main.c -o main.o

image: image.o ascii.o
	cc -Wall image.o ascii.o -o image `MagickCore-config --ldflags --libs` -lncurses -lm

image.o: image.c image.h
	cc -Wall -g `MagickCore-config --cflags --cppflags` -c image.c -o image.o

ascii.o: ascii.c ascii.h
	cc -Wall -g -c ascii.c -o ascii.o

clean:
	rm -f *.o main