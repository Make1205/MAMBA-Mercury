#include "nike_ntt.h"
#include "params.h"
#include <string.h>

static int inited=0; static uint16_t psi,omega,invpsi,invomega,invn;
static uint16_t powmod(uint16_t a, uint32_t e){ uint32_t r=1,b=a; while(e){ if(e&1) r=(r*b)%PARAM_Q; b=(b*b)%PARAM_Q; e>>=1;} return (uint16_t)r; }
static uint16_t invmod(uint16_t a){ return powmod(a, PARAM_Q-2); }
static void bitrev(uint16_t *a){ for(unsigned i=1,j=0;i<PARAM_N;i++){ unsigned bit=PARAM_N>>1; for(; j&bit; bit>>=1) j^=bit; j^=bit; if(i<j){ uint16_t t=a[i]; a[i]=a[j]; a[j]=t; } } }

int nike_ntt_init(void){ if(inited) return 0; for(uint16_t c=2;c<PARAM_Q;c++){ if(powmod(c,PARAM_N)==PARAM_Q-1 && powmod(c,2*PARAM_N)==1){ psi=c; break; } } if(!psi) return -1; omega=powmod(psi,2); invpsi=invmod(psi); invomega=invmod(omega); invn=invmod(PARAM_N); inited=1; return 0; }
uint16_t nike_ntt_psi(void){ if(!inited) nike_ntt_init(); return psi; }

void nike_cyclic_ntt(uint16_t *a){ if(!inited) nike_ntt_init(); bitrev(a); for(unsigned len=2; len<=PARAM_N; len<<=1){ uint16_t wlen=powmod(omega, PARAM_N/len); for(unsigned i=0;i<PARAM_N;i+=len){ uint16_t w=1; for(unsigned j=0;j<len/2;j++){ uint16_t u=a[i+j]; uint16_t v=(uint32_t)a[i+j+len/2]*w%PARAM_Q; uint16_t x=u+v; if(x>=PARAM_Q) x-=PARAM_Q; int32_t y=(int32_t)u-(int32_t)v; if(y<0) y+=PARAM_Q; a[i+j]=x; a[i+j+len/2]=(uint16_t)y; w=(uint32_t)w*wlen%PARAM_Q; } } } }
void nike_cyclic_intt(uint16_t *a){ if(!inited) nike_ntt_init(); bitrev(a); for(unsigned len=2; len<=PARAM_N; len<<=1){ uint16_t wlen=powmod(invomega, PARAM_N/len); for(unsigned i=0;i<PARAM_N;i+=len){ uint16_t w=1; for(unsigned j=0;j<len/2;j++){ uint16_t u=a[i+j]; uint16_t v=(uint32_t)a[i+j+len/2]*w%PARAM_Q; uint16_t x=u+v; if(x>=PARAM_Q) x-=PARAM_Q; int32_t y=(int32_t)u-(int32_t)v; if(y<0) y+=PARAM_Q; a[i+j]=x; a[i+j+len/2]=(uint16_t)y; w=(uint32_t)w*wlen%PARAM_Q; } } } for(int i=0;i<PARAM_N;i++) a[i]=(uint32_t)a[i]*invn%PARAM_Q; }

void nike_mul_negacyclic_ntt(poly *out,const poly *a,const poly *b){ if(!inited) nike_ntt_init(); uint16_t ta[PARAM_N],tb[PARAM_N]; for(int i=0;i<PARAM_N;i++){ ta[i]=(uint32_t)a->coeffs[i]*powmod(psi,i)%PARAM_Q; tb[i]=(uint32_t)b->coeffs[i]*powmod(psi,i)%PARAM_Q; } nike_cyclic_ntt(ta); nike_cyclic_ntt(tb); for(int i=0;i<PARAM_N;i++) ta[i]=(uint32_t)ta[i]*tb[i]%PARAM_Q; nike_cyclic_intt(ta); for(int i=0;i<PARAM_N;i++) out->coeffs[i]=(uint32_t)ta[i]*powmod(invpsi,i)%PARAM_Q; }
