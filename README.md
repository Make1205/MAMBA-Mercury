# MAMBA-NIKE Reference Implementation

This repository now uses **MAMBA-NIKE** as the active reference path.

It is derived from the NewHope code structure, but the protocol identity and active path are NIKE-oriented.

## Supported full-KE profiles
- NIKE-128 (n=1024)
- NIKE-192 (n=1024)
- NIKE-256 (n=1024)
- NIKE-384 (n=2048)
- NIKE-512 (n=2048)

## Arithmetic and protocol notes
- `q = 12289` for all NIKE profiles.
- Clean coefficient-domain NTT arithmetic supports `n=1024` and `n=2048`.
- D4 reconciliation is retained.
- `b/u/v` are quantized in coefficient domain using RLWQ-Z public dither.
- `v` is local-only and is not transmitted.

Messages:
- `M_A = (rho, b)`
- `M_B = (mu, u, h)`

## Message sizes
Using:
- `|M_A| = 32 + n*t_pk/8`
- `|M_B| = 32 + n*t_u/8 + kappa`

Validated totals:
- NIKE-128 total = 2752
- NIKE-192 total = 3072
- NIKE-256 total = 3136
- NIKE-384 total = 6080
- NIKE-512 total = 6208

## Shared-secret length
Variable output KDF is used (`ss_bytes = classic/8`):
- NIKE-128 = 16
- NIKE-192 = 24
- NIKE-256 = 32
- NIKE-384 = 48
- NIKE-512 = 64
