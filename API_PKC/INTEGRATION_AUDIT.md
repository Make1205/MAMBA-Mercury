# MAMBA-NIKE API_PKC Integration Audit (Final)

## Overall status
Final regression status is **PASS** for the ten-instance API_PKC submission package:

- Five Reference instances (`128/192/256/384/512`) build and run `./KAT_KEX` successfully.
- Five Optimized instances (`128/192/256/384/512`) build and run `./KAT_KEX` successfully.
- For all five security levels, Reference outputs match `API_PKC/Test_Vectors/KAT_KEX_MAMBA-NIKE-*.txt` byte-for-byte.
- For all five security levels, Optimized outputs also match the same official `Test_Vectors` byte-for-byte.

## Final package structure and scope
- Final API_PKC KEX instances are organized under:
  - `API_PKC/Implementations/Reference_Implementation/MAMBA-NIKE-{128,192,256,384,512}`
  - `API_PKC/Implementations/Optimized_Implementation/MAMBA-NIKE-{128,192,256,384,512}`
- Official KAT files are:
  - `API_PKC/Test_Vectors/KAT_KEX_MAMBA-NIKE-128.txt`
  - `API_PKC/Test_Vectors/KAT_KEX_MAMBA-NIKE-192.txt`
  - `API_PKC/Test_Vectors/KAT_KEX_MAMBA-NIKE-256.txt`
  - `API_PKC/Test_Vectors/KAT_KEX_MAMBA-NIKE-384.txt`
  - `API_PKC/Test_Vectors/KAT_KEX_MAMBA-NIKE-512.txt`

## Official-file baseline note
No external official baseline package was available in-tree for cryptographic hash-by-hash source verification.
Therefore:

- We **cannot** claim 100% identity against an external official template baseline.
- We can state that the final repair chain did **not** introduce new edits to `KAT_KEX.c`, `drng.c/.h`, or `auxfunc.c/.h` during final stabilization and validation steps.

## Randomness path audit
- Default build path uses the API_PKC DRNG bridge via `src/randombytes_api_pkc.c`:
  - `extern DRNG_ctx drng_algorithm;`
  - `get_random_number(&drng_algorithm, buf, len*8);`
- `src/non_api_pkc_randombytes.c` exists in instance trees but is filtered out of default Makefile source sets and is **not in default build path**.

## Hash / XOF / KDF classification (no code changes in this phase)
### Already adapted through API_PKC helpers
- Adapter files use:
  - `sm3hash` (hash adapter)
  - `pseudohash` (KDF adapter)
  - `pseudoXOF` (XOF adapter)

### Still present and compiled in default paths
- `fips202` / Keccak / SHAKE-related sources.
- `crypto_stream_chacha20` and (in some trees) AES-CTR/SHA-256 helper sources.
- In Optimized instances, `src/nike_refcompat/*` (including `fips202.c`) is explicitly compiled by default.

### Classification
- These are retained as **NEEDS REVIEW** items for future hardening/normalization because they are tied to ref-compatible core paths and legacy algorithm components.
- They are intentionally **not replaced** in this final packaging phase to avoid protocol/semantic drift after KAT closure.

## Ref/Opt boundary summary
- Reference instance Makefiles do not use AVX2/AES/SSE aggressive optimization flags.
- Optimized instance Makefiles do use AVX2/AES/SSE flags as expected.
- Optimized instance build paths do not pull source files from `../../Reference_Implementation`.

## Legacy template directories
- `API_PKC/Implementations/Reference_Implementation/AlgorithmInstance` still exists as a historical template/demo subtree.
- It is not part of the final ten-instance default KAT build path.
- Classified as a **cleanup candidate**; not removed in final validation phase.
