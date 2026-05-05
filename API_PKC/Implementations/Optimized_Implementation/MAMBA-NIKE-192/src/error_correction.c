#include "crypto_stream.h"
#include "error_correction.h"

//See paper for details on the error reconciliation

extern void hr(poly *c, const poly *v, unsigned char rand[32]);

void helprec(poly *c, const poly *v, const unsigned char *seed, unsigned char nonce)
{
  unsigned char rand[32];
  unsigned char n[CRYPTO_STREAM_NONCEBYTES];
  int i;

  for(i=0;i<CRYPTO_STREAM_NONCEBYTES-1;i++)
    n[i] = 0;
  n[CRYPTO_STREAM_NONCEBYTES-1] = nonce;

  crypto_stream(rand,32,n,seed);

  hr(c, v, rand);
}
