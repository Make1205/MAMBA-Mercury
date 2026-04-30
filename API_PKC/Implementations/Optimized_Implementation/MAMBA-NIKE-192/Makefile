CC=/usr/bin/gcc
CFLAGS=-Wall -Wextra -O3 -fomit-frame-pointer -msse2avx -march=corei7-avx -mavx2 -maes -msse2 -mssse3
HEADERS = src/crypto_stream_chacha20.h src/params.h src/poly.h src/randombytes.h src/ntt.h src/error_correction.h src/crypto_hash_sha256.h src/fips202.h src/crypto_stream_aes256ctr.h
CFILES = src/crypto_stream_chacha20.c src/poly.c src/precomp.c src/error_correction.c src/newhope.c src/nike.c src/crypto_hash_sha256.c src/chacha.S src/cbd.s src/consts.c src/fips202.c src/omegas.c src/ntt_double.s src/pwmul.s src/bitrev.s src/hr.s src/rec.s src/crypto_stream_aes256ctr.c src/randombytes_api_pkc.c 

all: KAT_KEX
KAT_KEX: KAT_KEX.c KEX_AlgorithmInstance.c drng.c auxfunc.c $(HEADERS) $(CFILES)
	$(CC) $(CFLAGS) -Isrc KAT_KEX.c KEX_AlgorithmInstance.c drng.c auxfunc.c $(CFILES) -o KAT_KEX -no-pie
clean:
	rm -f KAT_KEX *.o src/*.o
