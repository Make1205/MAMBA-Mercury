#include "kdf_api_pkc.h"
#include <string.h>
void kdf_api_pkc(unsigned char*out,size_t outlen,const unsigned char*in,size_t inlen){
  unsigned char tmp[128];
  size_t done=0; unsigned ctr=0;
  while(done<outlen){
    unsigned char msg[1024]; size_t m=inlen; if(m>1000) m=1000;
    memcpy(msg,in,m); msg[m++]=(unsigned char)ctr;
    pseudohash(1024,msg,m*8,tmp);
    size_t take=outlen-done; if(take>128) take=128;
    memcpy(out+done,tmp,take); done+=take; ctr++;
  }
}
