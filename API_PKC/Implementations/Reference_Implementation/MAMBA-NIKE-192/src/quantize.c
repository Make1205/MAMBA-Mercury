#include "quantize.h"
#include "params.h"

uint16_t coeff_quantize(uint16_t x, uint16_t d, unsigned t) {
  uint32_t p = 1u << t;
  uint32_t s = (uint32_t)x + (uint32_t)d;
  uint32_t qidx = (p * s + PARAM_Q / 2u) / PARAM_Q;
  return (uint16_t)(qidx & (p - 1u));
}

uint16_t coeff_dequantize(uint16_t c, uint16_t d, unsigned t) {
  uint32_t p = 1u << t;
  uint32_t recon = ((uint32_t)PARAM_Q * (uint32_t)c + p / 2u) / p;
  int32_t v = (int32_t)recon - (int32_t)d;
  v %= PARAM_Q;
  if (v < 0) v += PARAM_Q;
  return (uint16_t)v;
}

void poly_quantize(poly *out, const poly *in, const poly *d, unsigned t) {
  for (int i = 0; i < PARAM_N; ++i) out->coeffs[i] = coeff_quantize(in->coeffs[i], d->coeffs[i], t);
}

void poly_dequantize(poly *out, const poly *in, const poly *d, unsigned t) {
  for (int i = 0; i < PARAM_N; ++i) out->coeffs[i] = coeff_dequantize(in->coeffs[i], d->coeffs[i], t);
}
