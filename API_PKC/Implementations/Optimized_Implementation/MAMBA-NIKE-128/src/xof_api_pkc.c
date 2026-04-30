#include "xof_api_pkc.h"
#include "../auxfunc.h"
void xof_api_pkc(unsigned char*out,size_t outlen,const unsigned char*in,size_t inlen){ pseudoXOF(outlen*8,in,inlen*8,out); }
