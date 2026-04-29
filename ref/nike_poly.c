#include "nike_compat.h"
#include "nike_poly.h"
#include "nike.h"
#include "poly.h"
#include "nike_ntt.h"

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
