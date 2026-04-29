#ifndef NIKE_PARAMS_H
#define NIKE_PARAMS_H
#include <stddef.h>
#include <stdint.h>
#define NIKE_MAX_N 2048
#define NIKE_Q 12289
#define NIKE_SEEDBYTES 32

typedef struct nike_params {
  const char *name; unsigned classic, quantum, n, q, eta_s, eta_r, t_pk, t_u, t_v, kappa, ss_bytes, ma_bytes, mb_bytes;
} nike_params;

extern const nike_params NIKE_128_PARAMS;
extern const nike_params NIKE_192_PARAMS;
extern const nike_params NIKE_256_PARAMS;
extern const nike_params NIKE_384_PARAMS;
extern const nike_params NIKE_512_PARAMS;

#endif
