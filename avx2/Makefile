CC = /usr/bin/gcc
CFLAGS = -Wall -Wextra -O3 -fomit-frame-pointer -msse2avx -march=corei7-avx
LDFLAGS += -no-pie

HEADERS = crypto_stream_chacha20.h params.h poly.h randombytes.h ntt.h error_correction.h crypto_hash_sha256.h  fips202.h crypto_stream_aes256ctr.h
CFILES  = crypto_stream_chacha20.c poly.c precomp.c error_correction.c newhope.c crypto_hash_sha256.c chacha.S cbd.s consts.c fips202.c omegas.c ntt_double.s pwmul.s bitrev.s crypto_stream_aes256ctr.o hr.s rec.s

all:	test/test_newhope \
			test/speed  \
			test/testvectors \
			test/test_statistical \
			test/test_nike_avx2 \
			test-nike-equiv-128


test/test_newhope: $(HEADERS) $(CFILES) test/test_newhope.c randombytes.c
	$(CC) $(CFLAGS) $(CFILES) test/test_newhope.c randombytes.c -o $@ $(LDFLAGS)

test/test_statistical: $(HEADERS) $(CFILES) test/test_statistical.c randombytes.c
	$(CC) $(CFLAGS) $(CFILES) -DSTATISTICAL_TEST randombytes.c test/test_statistical.c -o $@ $(LDFLAGS)

test/speed: $(HEADERS) $(CFILES) test/speed.c cpucycles.c randombytes.c
	$(CC) $(CFLAGS) $(CFILES) test/speed.c cpucycles.c randombytes.c -o $@ $(LDFLAGS)

test/testvectors: $(HEADERS) $(CFILES) test/testvectors.c
	$(CC) $(CFLAGS) $(CFILES) -DTESTVECTORS test/testvectors.c cpucycles.c -o $@ $(LDFLAGS)

crypto_stream_aes256ctr.o: crypto_stream_aes256ctr.c
	$(CC) -c -m64 -march=native -mtune=native -O3 -fomit-frame-pointer $^ -o $@

pwmul.q: ntt_double.py
	./ntt_double.py pwmul > $@

ntt_double.q: ntt_double.py
	./ntt_double.py > $@

.PHONY: clean

clean:
	-rm *.o
	-rm test/test_newhope	
	-rm test/test_statistical
	-rm test/speed
	-rm test/testvectors
	-rm test/gen_nike_ref_128 test/gen_nike_avx2_128 test/ref_128.out test/avx2_128.out



NIKE_REFCOMPAT_CFILES = nike_refcompat/nike.c nike_refcompat/nike_poly.c nike_refcompat/nike_ntt.c nike_refcompat/quantize.c nike_refcompat/error_correction.c nike_refcompat/fips202.c nike_refcompat/reduce.c nike_refcompat/poly.c nike_refcompat/ntt.c nike_refcompat/precomp.c nike_refcompat/crypto_stream_chacha20.c nike_refcompat/randombytes.c

test/test_nike_avx2: nike_refcompat/nike.h nike_refcompat/nike_poly.h nike_refcompat/nike_ntt.h nike_refcompat/nike_params.h nike_refcompat/nike_compat.h test/test_nike_avx2.c cpucycles.c
	$(CC) $(CFLAGS) -I./nike_refcompat $(NIKE_REFCOMPAT_CFILES) cpucycles.c test/test_nike_avx2.c -o $@ $(LDFLAGS)

test-nike-equiv-128: test/gen_nike_ref_128 test/gen_nike_avx2_128
	./test/gen_nike_ref_128 > test/ref_128.out
	./test/gen_nike_avx2_128 > test/avx2_128.out
	python3 test/compare_nike_128.py test/ref_128.out test/avx2_128.out

test/gen_nike_ref_128: test/gen_nike_ref_128.c test/randombytes_deterministic.c
	$(CC) -O2 -I../ref ../ref/nike.c ../ref/nike_poly.c ../ref/nike_ntt.c ../ref/quantize.c ../ref/error_correction.c ../ref/fips202.c ../ref/reduce.c ../ref/poly.c ../ref/ntt.c ../ref/precomp.c ../ref/crypto_stream_chacha20.c test/randombytes_deterministic.c test/gen_nike_ref_128.c -o $@

test/gen_nike_avx2_128: test/gen_nike_avx2_128.c
	$(CC) -O2 -I./nike_refcompat $(NIKE_REFCOMPAT_CFILES) test/gen_nike_avx2_128.c -o $@
