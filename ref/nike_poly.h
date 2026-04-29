#ifndef NIKE_POLY_H
#define NIKE_POLY_H
#include <stdint.h>

#define NIKE_MAX_N 2048

typedef struct { uint16_t coeffs[NIKE_MAX_N]; } nike_poly;

#endif
