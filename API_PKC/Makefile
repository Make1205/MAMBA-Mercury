CC?=gcc
CFLAGS?=-O2 -Wall -Wextra -std=gnu11
PROFILES=128 192 256 384 512
REF_DIR=Implementations/Reference_Implementation
OPT_DIR=Implementations/Optimized_Implementation
KAT_DIR=KAT
REF_SRC=$(filter-out %/randombytes.c %/cpucycles.c,$(wildcard $(REF_DIR)/NIKE/*.c))
OPT_SRC=$(filter-out %/randombytes.c %/cpucycles.c %/crypto_stream_aes256ctr.c %/crypto_hash_sha256.c,$(wildcard $(OPT_DIR)/NIKE/*.c))
REF_ALG=$(REF_DIR)/AlgorithmInstance/KAT_KEX.c $(REF_DIR)/AlgorithmInstance/drng.c $(REF_DIR)/AlgorithmInstance/auxfunc.c $(REF_DIR)/AlgorithmInstance/KEX_AlgorithmInstance.c
OPT_ALG=$(OPT_DIR)/AlgorithmInstance/KAT_KEX.c $(OPT_DIR)/AlgorithmInstance/drng.c $(OPT_DIR)/AlgorithmInstance/auxfunc.c $(OPT_DIR)/AlgorithmInstance/KEX_AlgorithmInstance.c
kat-ref: $(KAT_DIR)
	@for p in $(PROFILES); do $(CC) $(CFLAGS) -DNIKE_PROFILE=$$p -I$(REF_DIR)/NIKE -I$(REF_DIR)/AlgorithmInstance $(REF_ALG) $(REF_SRC) -o build_ref_$$p && ./build_ref_$$p && mv KAT_KEX_AlgorithmInstance.rsp $(KAT_DIR)/NIKE-$$p-ref.rsp; done
kat-opt: $(KAT_DIR)
	@for p in $(PROFILES); do $(CC) $(CFLAGS) -DNIKE_PROFILE=$$p -I$(OPT_DIR)/NIKE -I$(OPT_DIR)/AlgorithmInstance $(OPT_ALG) $(OPT_SRC) -o build_opt_$$p && ./build_opt_$$p && mv KAT_KEX_AlgorithmInstance.rsp $(KAT_DIR)/NIKE-$$p-opt.rsp; done
test: kat-ref kat-opt
all: test
$(KAT_DIR):
	mkdir -p $(KAT_DIR)
clean:
	rm -f build_ref_* build_opt_* KAT_KEX_AlgorithmInstance.rsp
