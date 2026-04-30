#ifndef NIKE_H
#define NIKE_H

#include <stddef.h>
#include "newhope.h"
#include "params.h"

typedef poly nike_state;

typedef struct {
  const char *name;
  unsigned sec_bits;
  unsigned ss_bytes;
  unsigned ma_bytes;
  unsigned mb_bytes;
} nike_params;

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

#endif
