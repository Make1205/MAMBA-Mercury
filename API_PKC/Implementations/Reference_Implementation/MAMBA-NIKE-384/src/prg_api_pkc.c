#include "prg_api_pkc.h"
#include "xof_api_pkc.h"
#include <string.h>
void prg_api_pkc(unsigned char*out,size_t outlen,const unsigned char*key,size_t keylen,const unsigned char*nonce,size_t noncelen,unsigned char domain){
  unsigned char in[128]={0}; size_t off=0;
  if(keylen>64) keylen=64; if(noncelen>32) noncelen=32;
  in[off++]=domain; in[off++]=(unsigned char)keylen; in[off++]=(unsigned char)noncelen;
  memcpy(in+off,key,keylen); off+=keylen; memcpy(in+off,nonce,noncelen); off+=noncelen;
  xof_api_pkc(out,outlen,in,off);
}
