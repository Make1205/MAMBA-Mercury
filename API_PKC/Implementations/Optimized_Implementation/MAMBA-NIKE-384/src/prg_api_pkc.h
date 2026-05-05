#ifndef PRG_API_PKC_H
#define PRG_API_PKC_H
#include <stddef.h>
void prg_api_pkc(unsigned char*out,size_t outlen,const unsigned char*key,size_t keylen,const unsigned char*nonce,size_t noncelen,unsigned char domain);
#endif
