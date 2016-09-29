CFLAGS = -Wall
CC = gcc

all:
	$(CC) -L/usr/include/libusb-1.0 ./MeadeDSIColor.c -lusb-1.0 -lcfitsio -lm -lrt -o MeadeDSIColor

clean:
	rm -f dsiColor

