CC=g++
CFLAGS=-lmraa
INCLUDE=inc

all:
	@$(CC) $(CFLAGS) -I$(INCLUDE) sx1509.cpp src/libsx1509.cpp -o sx1509

clean:
	@rm -rf sx1509
