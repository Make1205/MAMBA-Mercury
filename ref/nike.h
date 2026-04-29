#ifndef NIKE_H
#define NIKE_H
#include <stddef.h>
#include <stdint.h>
#include "nike_params.h"
#include "poly.h"
#include "nike_poly.h"

typedef struct { const nike_params *p; nike_poly s; } nike_state;

#define NIKE_128 NIKE_128_PARAMS
#define NIKE_192 NIKE_192_PARAMS
#define NIKE_256 NIKE_256_PARAMS
#define NIKE_384 NIKE_384_PARAMS
#define NIKE_512 NIKE_512_PARAMS

size_t nike_ma_bytes(const nike_params *p);
size_t nike_mb_bytes(const nike_params *p);
void nike_init(nike_state *st, unsigned char *M_A, const nike_params *p);
void nike_resp(unsigned char *M_B, unsigned char *K_B, const unsigned char *M_A, const nike_params *p);
void nike_derive(unsigned char *K_A, const nike_state *st, const unsigned char *M_A, const unsigned char *M_B);
void nike_pack_bits(unsigned char *out, const poly *a, unsigned t);
void nike_unpack_bits(poly *a, const unsigned char *in, unsigned t);
void nike_gen_public(poly *a, poly *dpk, const unsigned char rho[32]);
void nike_gen_dither(poly *du, poly *dv, const unsigned char mu[32]);
void nike_mul_coeff_ntt(poly *out,const poly *a,const poly *b);
void nike_mul_schoolbook(poly *out, const poly *a, const poly *b, const nike_params *params);
void nike_mul_coeff(poly *out,const poly *a,const poly *b);
typedef enum { NIKE_MUL_BACKEND_NTT=0, NIKE_MUL_BACKEND_SCHOOLBOOK=1 } nike_mul_backend;
void nike_set_mul_backend(nike_mul_backend backend);

#endif
