#ifndef NIKE_NTT_H
#define NIKE_NTT_H
#include <stdint.h>
#include "poly.h"
#include "nike_poly.h"

int nike_ntt_init_n(unsigned n);
uint16_t nike_ntt_psi_n(unsigned n);
void nike_cyclic_ntt_n(uint16_t *a, unsigned n);
void nike_cyclic_intt_n(uint16_t *a, unsigned n);
void nike_mul_negacyclic_ntt_n(uint16_t *out, const uint16_t *a, const uint16_t *b, unsigned n);

int nike_ntt_init(void);
uint16_t nike_ntt_psi(void);
void nike_cyclic_ntt(uint16_t *a);
void nike_cyclic_intt(uint16_t *a);
void nike_mul_negacyclic_ntt(poly *out, const poly *a, const poly *b);

#endif
