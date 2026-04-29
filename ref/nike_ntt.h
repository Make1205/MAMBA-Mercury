#ifndef NIKE_NTT_H
#define NIKE_NTT_H
#include <stdint.h>
#include "poly.h"

int nike_ntt_init(void);
uint16_t nike_ntt_psi(void);
void nike_cyclic_ntt(uint16_t *a);
void nike_cyclic_intt(uint16_t *a);
void nike_mul_negacyclic_ntt(poly *out, const poly *a, const poly *b);

#endif
