CC=gcc
CFLAGS=-O2 -Wall -Wextra -Isrc
SRC=$(wildcard src/*.c)
OBJ=KAT_KEX.o KEX_AlgorithmInstance.o drng.o auxfunc.o $(SRC:.c=.o)

all: KAT_KEX
KAT_KEX: $(OBJ)
	$(CC) -o $@ $(OBJ)
clean:
	rm -f KAT_KEX *.o src/*.o
