# MAMBA-NIKE API_PKC package

This package contains the MAMBA-NIKE KEX submission implementation and is self-contained under `API_PKC/`.

- `Implementations/Reference_Implementation` maps to the repository `ref/` NIKE implementation (packaged locally in API_PKC).
- `Implementations/Optimized_Implementation` maps to the repository `avx2/` NIKE implementation (packaged locally in API_PKC).
- Optimized status: NIKE-128/192/256 are avx2-native; NIKE-384/512 are avx2-mixed fallback.
- Protocol is 2-pass KEX: `M_A=rho||b`, `M_B=mu||u||h`, `v` is local and not transmitted.
- RLWQ-Z coefficient-domain quantization and D4 reconciliation are retained.
- helper size = kappa bytes.

## Parameters
See `Implementations/*/NIKE/nike.c` for NIKE-128/192/256/384/512 table and message sizes.

## Build and test
```bash
make clean
make test
make kat-ref
make kat-opt
```
Outputs are written to `KAT/NIKE-*-ref.rsp` and `KAT/NIKE-*-opt.rsp`.

Forbidden template files left unmodified: `drng.c/h`, `auxfunc.c/h`, `KAT_SIG.c`, `KAT_KEM.c`, `KAT_KEX.c`.
