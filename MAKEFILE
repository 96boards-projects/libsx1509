CC=gcc
CFLAGS=-lmraa

all:
    $(CC) -o example example.cpp libsx1509.cpp $(CFLAGS)
     
clean:
    rm example
