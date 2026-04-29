CC ?= gcc
CFLAGS_COMMON = -std=c11 -Wall -Wextra -I. -Isrc
CFLAGS = -O2 $(CFLAGS_COMMON) -mavx2 -maes -msse2 -mssse3
ASFLAGS = $(CFLAGS)
OBJ = KAT_KEX.o KEX_AlgorithmInstance.o drng.o auxfunc.o \
 src/consts.o src/cpucycles.o src/crypto_hash_sha256.o src/crypto_stream_aes256ctr.o src/crypto_stream_chacha20.o src/error_correction.o src/fips202.o src/newhope.o src/omegas.o src/poly.o src/precomp.o src/randombytes.o src/bitrev.o src/cbd.o src/chacha.o src/hr.o src/ntt_double.o src/pwmul.o src/rec.o
KAT_KEX: $(OBJ)
	$(CC) $(CFLAGS) -no-pie -o $@ $(OBJ)
clean:
	rm -f $(OBJ) KAT_KEX

src/%.o: src/%.s
	$(CC) $(CFLAGS) -c -o $@ $<
