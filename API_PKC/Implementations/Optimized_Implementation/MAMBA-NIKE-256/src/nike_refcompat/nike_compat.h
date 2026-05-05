#ifndef NIKE_COMPAT_H
#define NIKE_COMPAT_H
#include "poly.h"
#include "nike_poly.h"
int nike_poly_to_oldpoly_1024(poly *out, const nike_poly *in, const nike_params *params);
int oldpoly_to_nike_poly_1024(nike_poly *out, const poly *in, const nike_params *params);
#endif
