#ifndef NIKE_H
#define NIKE_H
#include <stddef.h>
#include <stdint.h>
#include "poly.h"

typedef struct {
  const char *name;
  unsigned t_pk, t_u, t_v;
  unsigned kappa;
} nike_params;

typedef struct { poly s; nike_params p; } nike_state;

extern const nike_params NIKE_128;
extern const nike_params NIKE_192;
extern const nike_params NIKE_256;
extern const nike_params NIKE_384;
extern const nike_params NIKE_512;

size_t nike_ma_bytes(const nike_params *p);
size_t nike_mb_bytes(const nike_params *p);

void nike_init(nike_state *st, unsigned char *M_A, const nike_params *p);
void nike_resp(unsigned char *M_B, unsigned char *K_B, const unsigned char *M_A, const nike_params *p);
void nike_derive(unsigned char *K_A, const nike_state *st, const unsigned char *M_A, const unsigned char *M_B);

void nike_pack_bits(unsigned char *out, const poly *a, unsigned t);
void nike_unpack_bits(poly *a, const unsigned char *in, unsigned t);
void nike_gen_public(poly *a, poly *dpk, const unsigned char rho[32]);
void nike_gen_dither(poly *du, poly *dv, const unsigned char mu[32]);
void nike_mul_coeff(poly *out, const poly *a, const poly *b);

#endif
