CC ?= gcc
CFLAGS ?= -O2 -Wall -Wextra -std=gnu11
PROFILES := 128 192 256 384 512
REF_DIR := Implementations/Reference_Implementation
OPT_DIR := Implementations/Optimized_Implementation
KAT_DIR := KAT

REF_SRC := $(filter-out %/randombytes.c %/cpucycles.c,$(wildcard $(REF_DIR)/NIKE/*.c))
OPT_SRC := $(filter-out %/randombytes.c %/cpucycles.c %/crypto_stream_aes256ctr.c %/crypto_hash_sha256.c,$(wildcard $(OPT_DIR)/NIKE/*.c))
REF_ALG := $(REF_DIR)/AlgorithmInstance/KAT_KEX.c $(REF_DIR)/AlgorithmInstance/drng.c $(REF_DIR)/AlgorithmInstance/auxfunc.c $(REF_DIR)/AlgorithmInstance/KEX_AlgorithmInstance.c
OPT_ALG := $(OPT_DIR)/AlgorithmInstance/KAT_KEX.c $(OPT_DIR)/AlgorithmInstance/drng.c $(OPT_DIR)/AlgorithmInstance/auxfunc.c $(OPT_DIR)/AlgorithmInstance/KEX_AlgorithmInstance.c

ONE_PROFILE := $(if $(NIKE_PROFILE),$(NIKE_PROFILE),$(PROFILES))

.PHONY: all test kat-ref kat-opt clean clean-kat

all: test

test: kat-ref kat-opt

kat-ref: | $(KAT_DIR)
	@for p in $(ONE_PROFILE); do \
		echo "[kat-ref] profile $$p"; \
		rm -rf output; mkdir -p output $(KAT_DIR); \
		$(CC) $(CFLAGS) -DNIKE_PROFILE=$$p -I$(REF_DIR)/NIKE -I$(REF_DIR)/AlgorithmInstance $(REF_ALG) $(REF_SRC) -o build_ref_$$p; \
		./build_ref_$$p; \
		out_file=$$(find output -type f \( -name '*KEX*' -o -name '*.rsp' -o -name '*.txt' \) | head -n 1); \
		if [ -z "$$out_file" ]; then \
			echo "No KEX KAT output found for NIKE-$$p ref"; \
			find output -maxdepth 2 -type f -print; \
			exit 1; \
		fi; \
		echo "Using output: $$out_file"; \
		cp "$$out_file" "$(KAT_DIR)/NIKE-$$p-ref.rsp"; \
	done

kat-opt: | $(KAT_DIR)
	@for p in $(ONE_PROFILE); do \
		echo "[kat-opt] profile $$p"; \
		rm -rf output; mkdir -p output $(KAT_DIR); \
		$(CC) $(CFLAGS) -DNIKE_PROFILE=$$p -I$(OPT_DIR)/NIKE -I$(OPT_DIR)/AlgorithmInstance $(OPT_ALG) $(OPT_SRC) -o build_opt_$$p; \
		./build_opt_$$p; \
		out_file=$$(find output -type f \( -name '*KEX*' -o -name '*.rsp' -o -name '*.txt' \) | head -n 1); \
		if [ -z "$$out_file" ]; then \
			echo "No KEX KAT output found for NIKE-$$p opt"; \
			find output -maxdepth 2 -type f -print; \
			exit 1; \
		fi; \
		echo "Using output: $$out_file"; \
		cp "$$out_file" "$(KAT_DIR)/NIKE-$$p-opt.rsp"; \
	done

$(KAT_DIR):
	mkdir -p $(KAT_DIR)

clean:
	rm -rf output
	rm -f build_ref_* build_opt_* *.o

clean-kat:
	rm -f $(KAT_DIR)/NIKE-*-ref.rsp $(KAT_DIR)/NIKE-*-opt.rsp
