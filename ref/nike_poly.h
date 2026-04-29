#ifndef NIKE_POLY_H
#define NIKE_POLY_H
#include <stdint.h>
#include <stddef.h>
#include "nike_params.h"

typedef struct { uint16_t coeffs[NIKE_MAX_N]; } nike_poly;

void nike_poly_zero(nike_poly *a, const nike_params *params);
void nike_poly_reduce(nike_poly *a, const nike_params *params);
int nike_poly_equal(const nike_poly *a, const nike_poly *b, const nike_params *params);
void nike_poly_quantize(nike_poly *out, const nike_poly *in, const nike_poly *d, unsigned t, const nike_params *params);
void nike_poly_dequantize(nike_poly *out, const nike_poly *in, const nike_poly *d, unsigned t, const nike_params *params);
int nike_poly_pack_bits(uint8_t *out, size_t outlen, const nike_poly *in, unsigned t, const nike_params *params);
int nike_poly_unpack_bits(nike_poly *out, const uint8_t *in, size_t inlen, unsigned t, const nike_params *params);
void nike_poly_mul_schoolbook(nike_poly *out, const nike_poly *a, const nike_poly *b, const nike_params *params);
void nike_poly_mul_ntt(nike_poly *out, const nike_poly *a, const nike_poly *b, const nike_params *params);
void nike_poly_mul(nike_poly *out, const nike_poly *a, const nike_poly *b, const nike_params *params, int backend);
void nike_poly_gen_public(nike_poly *a, nike_poly *dpk, const uint8_t rho[32], const nike_params *p);
void nike_poly_gen_dither(nike_poly *du, nike_poly *dv, const uint8_t mu[32], const nike_params *p);
void nike_poly_sample_secret(nike_poly *s, const uint8_t seed[32], uint8_t nonce, const nike_params *p);


#endif
