# MAMBA-NIKE Reference Implementation

This repository now uses **MAMBA-NIKE** as the active reference path.

It is derived from the NewHope code structure, but the protocol identity and active path are NIKE-oriented.

## Active full-KE profiles
- NIKE-128 (n=1024)
- NIKE-192 (n=1024)
- NIKE-256 (n=1024)

## Future profiles
- NIKE-384 (n=2048)
- NIKE-512 (n=2048)

NIKE-384/512 full KE is currently disabled until NIKE-native n=2048 main-flow migration is completed.

## Arithmetic readiness
- `q = 12289` for all NIKE profiles.
- Clean coefficient-domain NTT arithmetic tests pass for `n=1024` and `n=2048`.
- Note: n=2048 arithmetic readiness does **not** mean NIKE-384/512 full KE is enabled.

## Protocol structure (retained)
- D4 reconciliation is retained.
- RLWQ-Z public dither quantization replaces explicit additive-noise injection at `b/u/v` layers.
- `v` is local-only and is not transmitted.

Messages:
- `M_A = (rho, b)`
- `M_B = (mu, u, h)`

## Message sizes
Using:
- `|M_A| = 32 + n*t_pk/8`
- `|M_B| = 32 + n*t_u/8 + kappa`

Current validated active totals:
- NIKE-128 total = 2752
- NIKE-192 total = 3072
- NIKE-256 total = 3136

## Shared-secret length
Variable output KDF is used (`ss_bytes = classic/8`):
- NIKE-128 = 16
- NIKE-192 = 24
- NIKE-256 = 32
- NIKE-384 = 48 (future profile)
- NIKE-512 = 64 (future profile)
