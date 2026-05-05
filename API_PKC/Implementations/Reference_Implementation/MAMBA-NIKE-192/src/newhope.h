#ifndef NEWHOPE_H
#define NEWHOPE_H

#include "poly.h"
#include "randombytes.h"
#include "prg_api_pkc.h"
#include "error_correction.h"
#include <math.h>
#include <stdio.h>

void newhope_keygen(unsigned char *send, poly *sk);
void newhope_sharedb(unsigned char *sharedkey, unsigned char *send, const unsigned char *received);
void newhope_shareda(unsigned char *sharedkey, const poly *ska, const unsigned char *received);

#endif
