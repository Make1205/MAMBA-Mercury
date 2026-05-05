# MAMBA-NIKE API_PKC Integration Audit (Final)

## Overall conclusion
Overall final audit conclusion is **PASS**.

## Final full regression summary (128/192/256/384/512)
- Five Reference instances: `make clean && make && ./KAT_KEX` all PASS.
- Five Optimized instances: `make clean && make && ./KAT_KEX` all PASS.
- Five Reference vs Test_Vectors comparisons all `cmp=0`.
- Five Optimized vs Test_Vectors comparisons all `cmp=0`.
- Five Reference vs Optimized comparisons all `cmp=0`.

Detailed cmp results:
- `REF128_TV_CMP=0`, `REF192_TV_CMP=0`, `REF256_TV_CMP=0`, `REF384_TV_CMP=0`, `REF512_TV_CMP=0`
- `OPT128_TV_CMP=0`, `OPT192_TV_CMP=0`, `OPT256_TV_CMP=0`, `OPT384_TV_CMP=0`, `OPT512_TV_CMP=0`
- `REF_OPT_128_CMP=0`, `REF_OPT_192_CMP=0`, `REF_OPT_256_CMP=0`, `REF_OPT_384_CMP=0`, `REF_OPT_512_CMP=0`

## Test_Vectors integrity check
For each of the five KAT files under `API_PKC/Test_Vectors/`:
- `Count = 10`
- `nul=False` (no NUL byte)

## Symbol audit (legacy primitive symbols)
For all ten built `KAT_KEX` executables (5 Reference + 5 Optimized), running:
- `nm KAT_KEX | rg "shake|Shake|SHAKE|Keccak|keccak|sha3|sha3256|sha256|crypto_stream|chacha|ChaCha|aes256ctr|AES"`

Result: no matches in all ten cases (`rg` return code `1` each), i.e. no legacy primitive symbols detected in these final binaries.

## Default build-path audit (Makefile-level)
The ten instances use API_PKC adapter path in default KAT build:
- randomness: `randombytes_api_pkc.c` (ultimately `get_random_number(&drng_algorithm, ...)`)
- hash/XOF/KDF/PRG adapters: `hash_api_pkc.c`, `xof_api_pkc.c`, `kdf_api_pkc.c`, `prg_api_pkc.c`
- plus `drng.c`, `auxfunc.c`

Legacy primitive source files may still exist in repository trees, but are classified as **UNUSED_OR_NONDEFAULT** for the final default KAT regression target. They are not treated as active migration blockers in this final audit.


## Reference legacy filename clarification
- `src/crypto_hash_sha256.c` is absent in all five Reference instance `src/` directories (128/192/256/384/512).
- `src/crypto_stream_aes256ctr.c` is absent in all five Reference instance `src/` directories (128/192/256/384/512).
- Because those two files are absent, Reference-128/192/256/384 do not need to add filter-out entries for those names.
- In Reference-512, if filter-out contains those names, it is defensive exclusion only; it does not imply file presence or compilation.
- Default build-path and `nm` results confirm legacy primitives do not enter final `KAT_KEX` binaries.

## auxfunc backend note
Current default adapter backend path is:
- hash: `sm3hash`
- kdf: `pseudohash`
- xof: `pseudoXOF`


## 512 determinism fix status
- State determinism fix commit: `7a20fec`
- Verification retained in final regression context: `OPT_STABLE=0`, `REF_OPT_CMP=0`, `TV_CMP=0`.

## MUST_FIX / blocking issues
- `MUST_FIX`: **empty**.
- Blocking issue: **none**.

## Non-blocking review item
- Recommendation: script the ten-instance regression and/or integrate it into CI for routine reproducibility checks.

## Official-file statement (conservative)
- This audit did not perform external hash-baseline comparison against an upstream official API_PKC template package; therefore it does **not** claim historical never-modified status for official files.
- During the final auxfunc migration regression phase, no further edits were made to `KAT_KEX.c`, `drng.c/.h`, or `auxfunc.c/.h`.
- Current ten instances complete official `KAT_KEX` regression using these files.
