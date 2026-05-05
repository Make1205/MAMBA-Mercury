#include <string.h>
#include "nike_compat.h"
#include "nike_poly.h"
#include "nike.h"
#include "poly.h"
#include "nike_ntt.h"
#include "randombytes.h"
#include "prg_api_pkc.h"

static uint16_t modq(int64_t x, unsigned q){ int64_t v=x%(int64_t)q; if(v<0) v+=q; return (uint16_t)v; }
int nike_poly_to_oldpoly_1024(poly *out, const nike_poly *in, const nike_params *params){ if(params->n!=1024) return -1; for(unsigned i=0;i<1024;i++) out->coeffs[i]=in->coeffs[i]%params->q; return 0; }
int oldpoly_to_nike_poly_1024(nike_poly *out, const poly *in, const nike_params *params){ if(params->n!=1024) return -1; for(unsigned i=0;i<1024;i++) out->coeffs[i]=in->coeffs[i]%params->q; return 0; }
void nike_poly_zero(nike_poly *a, const nike_params *params){ for(unsigned i=0;i<params->n;i++) a->coeffs[i]=0; }
void nike_poly_reduce(nike_poly *a, const nike_params *params){ for(unsigned i=0;i<params->n;i++) a->coeffs[i]%=params->q; }
int nike_poly_equal(const nike_poly *a,const nike_poly *b,const nike_params *params){ for(unsigned i=0;i<params->n;i++) if((a->coeffs[i]%params->q)!=(b->coeffs[i]%params->q)) return 0; return 1; }
void nike_poly_quantize(nike_poly *out,const nike_poly *in,const nike_poly *d,unsigned t,const nike_params *params){ uint32_t p=1u<<t; for(unsigned i=0;i<params->n;i++){ uint32_t s=in->coeffs[i]+d->coeffs[i]; out->coeffs[i]=((p*s+params->q/2)/params->q)&(p-1); }}
void nike_poly_dequantize(nike_poly *out,const nike_poly *in,const nike_poly *d,unsigned t,const nike_params *params){ uint32_t p=1u<<t; for(unsigned i=0;i<params->n;i++){ uint32_t r=((uint32_t)params->q*in->coeffs[i]+p/2)/p; out->coeffs[i]=modq((int64_t)r-d->coeffs[i],params->q);} }
int nike_poly_pack_bits(uint8_t *out,size_t outlen,const nike_poly *in,unsigned t,const nike_params *params){ size_t need=(size_t)params->n*t/8; if(outlen<need) return -1; for(size_t i=0;i<need;i++) out[i]=0; unsigned b=0; for(unsigned i=0;i<params->n;i++){ unsigned v=in->coeffs[i]&((1u<<t)-1); for(unsigned j=0;j<t;j++,b++) out[b>>3]|=((v>>j)&1u)<<(b&7);} return 0; }
int nike_poly_unpack_bits(nike_poly *out,const uint8_t *in,size_t inlen,unsigned t,const nike_params *params){ size_t need=(size_t)params->n*t/8; if(inlen<need) return -1; unsigned b=0; for(unsigned i=0;i<params->n;i++){ unsigned v=0; for(unsigned j=0;j<t;j++,b++) v|=((in[b>>3]>>(b&7))&1u)<<j; out->coeffs[i]=v;} return 0; }
void nike_poly_mul_schoolbook(nike_poly *out,const nike_poly *a,const nike_poly *b,const nike_params *params){ static int64_t acc[NIKE_MAX_N]; for(unsigned i=0;i<params->n;i++) acc[i]=0; for(unsigned i=0;i<params->n;i++) for(unsigned j=0;j<params->n;j++){ unsigned k=i+j; int64_t v=(int64_t)a->coeffs[i]*b->coeffs[j]; if(k<params->n) acc[k]+=v; else acc[k-params->n]-=v;} for(unsigned i=0;i<params->n;i++) out->coeffs[i]=modq(acc[i],params->q); }
void nike_poly_mul_ntt(nike_poly *out,const nike_poly *a,const nike_poly *b,const nike_params *params){ nike_mul_negacyclic_ntt_n(out->coeffs,a->coeffs,b->coeffs,params->n); }
void nike_poly_mul(nike_poly *out,const nike_poly *a,const nike_poly *b,const nike_params *params,int backend){ if(backend==1) nike_poly_mul_schoolbook(out,a,b,params); else nike_poly_mul_ntt(out,a,b,params); }

static void nike_domain_uniform(nike_poly *r,const unsigned char *seed,const char *tag,const nike_params *p){
  uint64_t st[25]; uint8_t in[64]={0}; memcpy(in,seed,32); size_t tl=strlen(tag); memcpy(in+32,tag,tl>31?31:tl);
  shake128_absorb(st,in,64); unsigned ctr=0,pos=0; uint8_t buf[SHAKE128_RATE]; shake128_squeezeblocks(buf,1,st);
  while(ctr<p->n){ uint16_t v=(buf[pos]|((uint16_t)buf[pos+1]<<8))&0x3fff; if(v<p->q) r->coeffs[ctr++]=v; pos+=2; if(pos>SHAKE128_RATE-2){ shake128_squeezeblocks(buf,1,st); pos=0; }}
}

void nike_poly_gen_public(nike_poly *a, nike_poly *dpk, const uint8_t rho[32], const nike_params *p){ nike_domain_uniform(a,rho,"NIKE-A",p); nike_domain_uniform(dpk,rho,"NIKE-DPK",p); }
void nike_poly_gen_dither(nike_poly *du, nike_poly *dv, const uint8_t mu[32], const nike_params *p){ nike_domain_uniform(du,mu,"NIKE-DU",p); nike_domain_uniform(dv,mu,"NIKE-DV",p); }
void nike_poly_sample_secret(nike_poly *s, const uint8_t seed[32], uint8_t nonce, const nike_params *p){
  unsigned char buf[4*NIKE_MAX_N]; uint32_t *tp=(uint32_t*)buf,t,d,a,b; unsigned char n[8]={0}; n[0]=nonce; crypto_stream_chacha20(buf,4*p->n,n,seed);
  for(unsigned i=0;i<p->n;i++){ t=tp[i]; d=0; for(int j=0;j<8;j++) d += (t>>j)&0x01010101; a=((d>>8)&0xff)+(d&0xff); b=(d>>24)+((d>>16)&0xff); s->coeffs[i]=(a + p->q - b)%p->q; }
}
