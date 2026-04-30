# MAMBA-NIKE API_PKC Integration Audit (Phase 1)

## Overall
Phase-1 scan found multiple blocking issues that prevent safe Phase-2 integration without additional refactoring and packaging work.

## Blocking findings
1. `API_PKC` currently uses template `AlgorithmInstance` layout and blank-vector config (`OUTPUT_BLANK_TEST_VECTORS 1`) instead of five concrete `MAMBA-NIKE-{128,192,256,384,512}` instances.
2. `ref` build is not portable reference profile yet: current `ref/Makefile` uses `-msse2avx -march=corei7-avx`.
3. `avx2` build directly depends on `../ref` sources for `test/test_nike_avx2` target.
4. Both `ref` and `avx2` currently use local `fips202/shake128` and `/dev/urandom`-based `randombytes.c`, not API_PKC DRNG+auxfunc adaptation path.
5. API_PKC tree does not currently match required final directory structure and naming (`Test_Vector` vs `Test_Vectors`, missing five instances under both Reference/Optimized).

## Independent compile check
- `make -C ref clean && make -C ref`: success (with warnings).
- `make -C avx2 clean && make -C avx2`: success (with warnings), but includes cross-tree dependency for one test target.

## Recommended minimal next actions
1. Create ten self-contained instance directories under `API_PKC/Implementations/{Reference_Implementation,Optimized_Implementation}/MAMBA-NIKE-*`.
2. Copy official `KAT_KEX.c`, `drng.c/h`, `auxfunc.c/h` unchanged into each instance.
3. Implement per-instance `KEX_AlgorithmInstance.c/h` bridge only.
4. Add API_PKC-specific RNG/hash/XOF adapter files and ensure default KAT path uses DRNG + auxfunc helpers.
5. Generate reference KATs and diff optimized temporary KATs byte-by-byte before promoting outputs.
