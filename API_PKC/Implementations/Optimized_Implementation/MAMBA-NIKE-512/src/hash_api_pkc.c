#include "hash_api_pkc.h"
#include "../auxfunc.h"
void hash_api_pkc(unsigned char*out,const unsigned char*in,size_t inlen){ sm3hash(256,in,inlen*8,out); }
