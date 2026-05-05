CC=/usr/bin/gcc
CFLAGS=-Wall -Wextra -O3 -fomit-frame-pointer -msse2avx -march=corei7-avx -mavx2 -maes -msse2 -mssse3 -Isrc -Isrc/nike_refcompat

NIKE_REFCOMPAT_CFILES = src/nike_refcompat/nike.c src/nike_refcompat/nike_poly.c src/nike_refcompat/nike_ntt.c src/nike_refcompat/quantize.c src/nike_refcompat/error_correction.c src/nike_refcompat/fips202.c src/nike_refcompat/reduce.c src/nike_refcompat/poly.c src/nike_refcompat/ntt.c src/nike_refcompat/precomp.c src/nike_refcompat/crypto_stream_chacha20.c

all: KAT_KEX
KAT_KEX: KAT_KEX.c KEX_AlgorithmInstance.c drng.c auxfunc.c $(NIKE_REFCOMPAT_CFILES) src/randombytes_api_pkc.c src/hash_api_pkc.c
	$(CC) $(CFLAGS) KAT_KEX.c KEX_AlgorithmInstance.c drng.c auxfunc.c $(NIKE_REFCOMPAT_CFILES) src/randombytes_api_pkc.c src/hash_api_pkc.c -o KAT_KEX -no-pie

clean:
	rm -f KAT_KEX *.o src/*.o src/nike_refcompat/*.o
