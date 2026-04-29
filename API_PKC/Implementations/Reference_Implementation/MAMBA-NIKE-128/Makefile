CC ?= gcc
CFLAGS ?= -O2 -std=c11 -Wall -Wextra -I. -Isrc
LDFLAGS ?=
SRC = KAT_KEX.c KEX_AlgorithmInstance.c drng.c auxfunc.c \
      src/crypto_stream_chacha20.c src/error_correction.c src/fips202.c src/newhope.c src/ntt.c src/poly.c src/precomp.c src/reduce.c src/nike.c src/nike_poly.c src/nike_ntt.c src/quantize.c src/randombytes.c
OBJ = $(SRC:.c=.o)
KAT_KEX: $(OBJ)
	$(CC) $(CFLAGS) -o $@ $(OBJ) $(LDFLAGS)
clean:
	rm -f $(OBJ) KAT_KEX
