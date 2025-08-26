
all: main

main: main.o image
	cc -lncurses main.o -o main

main.o: main.c
	cc -c main.c -o main.o

image: image.c image.h
	cc `MagickCore-config --cflags --cppflags` -O2 -o image image.c `MagickCore-config --ldflags --libs`


clean:
	rm -f main.o main image