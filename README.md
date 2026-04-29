# MAMBA-NIKE (based on NewHope reference code)

This repository is being migrated from the original NewHope reference structure to **MAMBA-NIKE**.

## Design status

The current implementation keeps the NewHope code layout and core arithmetic stack (`q = 12289`, NTT-based polynomial multiplication, and NewHope-style D4 reconciliation), while introducing the MAMBA-NIKE protocol naming and parameter presets.

## MAMBA-NIKE protocol direction

- Keep NewHope-style two-pass key exchange flow.
- Keep D4 reconciliation (`HelpRec`, `Rec`) as the raw-key reconciliation core.
- Replace explicit additive noise layers at `b`, `u`, `v` with RLWQ-Z public dither quantization/dequantization.
- Do **not** send `v`; only send:
  - `M_A = (rho, b)`
  - `M_B = (mu, u, h)`
- Main compression points are `b` and `u`.

## Parameter instances (temporary engineering presets)

| Instance | n | q | t_pk | t_u | t_v | eta_s | eta_r | kappa |
|---|---:|---:|---:|---:|---:|---:|---:|---:|
| NIKE-128 | 1024 | 12289 | 10 | 10 | 6 | 5 | 5 | 128 |
| NIKE-192 | 1024 | 12289 | 11 | 11 | 6 | 5 | 5 | 192 |
| NIKE-256 | 1024 | 12289 | 11 | 11 | 6 | 5 | 5 | 256 |
| NIKE-384 | 2048 | 12289 | 11 | 11 | 6 | 5 | 5 | 384 |
| NIKE-512 | 2048 | 12289 | 11 | 11 | 6 | 5 | 5 | 512 |

Definitions:

- `p_pk = 1 << t_pk`
- `p_u  = 1 << t_u`
- `p_v  = 1 << t_v`

## NTT split requirement

- `NIKE-128/192/256` use the existing `n=1024` path.
- `NIKE-384/512` require a separate `n=2048` negacyclic NTT path under `q=12289`.
- `n=1024` and `n=2048` root tables/schedules/constants must be separated.

## Message sizes (target)

- NIKE-128: `|M_A|=1312`, `|M_B|=1440`, total `2752`
- NIKE-192: `|M_A|=1440`, `|M_B|=1600`, total `3040`
- NIKE-256: `|M_A|=1440`, `|M_B|=1664`, total `3104`
- NIKE-384: `|M_A|=2848`, `|M_B|=3232`, total `6080`
- NIKE-512: `|M_A|=2848`, `|M_B|=3360`, total `6208`

## Security-note

These presets are currently temporary engineering parameters and are expected to be revised after DFR measurement and lattice-estimator based security re-evaluation.
