#ifndef QUANTIZE_H
#define QUANTIZE_H

#include <stdint.h>
#include "poly.h"

uint16_t coeff_quantize(uint16_t x, uint16_t d, unsigned t);
uint16_t coeff_dequantize(uint16_t c, uint16_t d, unsigned t);
void poly_quantize(poly *out, const poly *in, const poly *d, unsigned t);
void poly_dequantize(poly *out, const poly *in, const poly *d, unsigned t);

#endif
