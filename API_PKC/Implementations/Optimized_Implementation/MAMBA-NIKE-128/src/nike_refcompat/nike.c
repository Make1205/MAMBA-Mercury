#include "nike.h"
#include "error_correction.h"
#include "fips202.h"
#include "randombytes.h"
#include "nike_compat.h"
#include "poly.h"
#include "../hash_api_pkc.h"
#include <string.h>
#include <stdlib.h>
#include "crypto_stream_chacha20.h"

const nike_params NIKE_128_PARAMS={"NIKE-128",128,80,1024,12289,5,5,10,10,6,128,16,1312,1440};
const nike_params NIKE_192_PARAMS={"NIKE-192",192,96,1024,12289,5,5,11,10,6,192,24,1440,1504};
const nike_params NIKE_256_PARAMS={"NIKE-256",256,128,1024,12289,5,5,11,10,6,256,32,1440,1568};
const nike_params NIKE_384_PARAMS={"NIKE-384",384,192,2048,12289,5,5,11,11,6,384,48,2848,3232};
const nike_params NIKE_512_PARAMS={"NIKE-512",512,256,2048,12289,5,5,11,11,6,512,64,2848,3360};

static nike_mul_backend g_backend = NIKE_MUL_BACKEND_NTT;
static int nike_kdf(unsigned char *out,size_t outlen,const unsigned char *nu,size_t nulen,const unsigned char *ma,size_t malen,const unsigned char *mb,size_t mblen){
  const char lbl[]="NIKE-KDF"; unsigned ctr=0; size_t done=0; unsigned char block[32];
  size_t inlen=(sizeof(lbl)-1)+2+4+nulen+malen+mblen; unsigned char *in=(unsigned char*)malloc(inlen); if(!in) return -1;
  while(done<outlen){ size_t off=0; memcpy(in+off,lbl,sizeof(lbl)-1); off+=sizeof(lbl)-1; in[off++]=outlen&0xff; in[off++]=(outlen>>8)&0xff; in[off++]=ctr&0xff; in[off++]=(ctr>>8)&0xff; in[off++]=(ctr>>16)&0xff; in[off++]=(ctr>>24)&0xff; memcpy(in+off,nu,nulen); off+=nulen; memcpy(in+off,ma,malen); off+=malen; memcpy(in+off,mb,mblen); off+=mblen; hash_api_pkc(block,in,off); size_t take=(outlen-done<32)?(outlen-done):32; memcpy(out+done,block,take); done+=take; ctr++; }
  free(in); return 0;
}


static int32_t nabs32(int32_t v){ int32_t mask = v >> 31; return (v ^ mask) - mask; }
static int32_t nf(int32_t *v0, int32_t *v1, int32_t x){
  int32_t xit, t, r, b;
  b = x*2730;
  t = b >> 25;
  b = x - t*12289;
  b = 12288 - b;
  b >>= 31;
  t -= b;
  r = t & 1;
  xit = (t>>1);
  *v0 = xit+r;
  t -= 1;
  r = t & 1;
  *v1 = (t>>1)+r;
  return nabs32(x-((*v0)*2*12289));
}
static int32_t ng(int32_t x){
  int32_t t,c,b;
  b = x*2730;
  t = b >> 27;
  b = x - t*49156;
  b = 49155 - b;
  b >>= 31;
  t -= b;
  c = t & 1;
  t = (t >> 1) + c;
  t *= 8*12289;
  return nabs32(t - x);
}
static int16_t nLDDecode(int32_t a,int32_t b,int32_t c,int32_t d){ int32_t t=ng(a)+ng(b)+ng(c)+ng(d)-8*12289; t>>=31; return t&1; }
static void nike_helprec_kappa_poly(nike_poly *c,const nike_poly *v,const unsigned char *seed,unsigned char nonce,const nike_params *p){ int32_t v0[4],v1[4],vt[4],k; unsigned char rand[32],n[8]={0}; n[7]=nonce; crypto_stream_chacha20(rand,32,n,seed); for(unsigned i=0;i<p->kappa;i++){ unsigned char rbit=(rand[i>>3]>>(i&7))&1; k=nf(v0,v1,8*v->coeffs[i]+4*rbit); k+=nf(v0+1,v1+1,8*v->coeffs[p->kappa+i]+4*rbit); k+=nf(v0+2,v1+2,8*v->coeffs[2*p->kappa+i]+4*rbit); k+=nf(v0+3,v1+3,8*v->coeffs[3*p->kappa+i]+4*rbit); k=(2*(int32_t)p->q-1-k)>>31; vt[0]=((~k)&v0[0])^(k&v1[0]); vt[1]=((~k)&v0[1])^(k&v1[1]); vt[2]=((~k)&v0[2])^(k&v1[2]); vt[3]=((~k)&v0[3])^(k&v1[3]); c->coeffs[i]=(vt[0]-vt[3])&3; c->coeffs[p->kappa+i]=(vt[1]-vt[3])&3; c->coeffs[2*p->kappa+i]=(vt[2]-vt[3])&3; c->coeffs[3*p->kappa+i]=(-k+2*vt[3])&3; }}
static void nike_rec_kappa_poly(unsigned char *key,const nike_poly *v,const nike_poly *c,const nike_params *p){ for(unsigned i=0;i<p->kappa/8;i++) key[i]=0; for(unsigned i=0;i<p->kappa;i++){ int32_t t0=16*p->q+8*(int32_t)v->coeffs[i]-p->q*(2*c->coeffs[i]+c->coeffs[3*p->kappa+i]); int32_t t1=16*p->q+8*(int32_t)v->coeffs[p->kappa+i]-p->q*(2*c->coeffs[p->kappa+i]+c->coeffs[3*p->kappa+i]); int32_t t2=16*p->q+8*(int32_t)v->coeffs[2*p->kappa+i]-p->q*(2*c->coeffs[2*p->kappa+i]+c->coeffs[3*p->kappa+i]); int32_t t3=16*p->q+8*(int32_t)v->coeffs[3*p->kappa+i]-p->q*(c->coeffs[3*p->kappa+i]); key[i>>3]|=nLDDecode(t0,t1,t2,t3)<<(i&7); }}

void nike_set_mul_backend(nike_mul_backend backend){ g_backend = backend; }
size_t nike_ma_bytes(const nike_params *p){ return 32 + (size_t)p->n*p->t_pk/8; }
size_t nike_mb_bytes(const nike_params *p){ return 32 + (size_t)p->n*p->t_u/8 + p->kappa; }

static int nike_helprec_poly(nike_poly *h, const nike_poly *vhat, const unsigned char *seed, unsigned char nonce, const nike_params *p){ nike_helprec_kappa_poly(h,vhat,seed,nonce,p); return 0; }

static int nike_rec_poly(unsigned char *raw, const nike_poly *vhat, const nike_poly *h, const nike_params *p){ nike_rec_kappa_poly(raw,vhat,h,p); return 0; }

/* Legacy n=1024 compatibility wrapper. Do not use for NIKE-384/512 or p->n=2048. */
void nike_pack_bits(unsigned char *out, const poly *a, unsigned t){
  nike_poly x; oldpoly_to_nike_poly_1024(&x,a,&NIKE_256_PARAMS);
  nike_poly_pack_bits(out,1024u*t/8u,&x,t,&NIKE_256_PARAMS);
}

/* Legacy n=1024 compatibility wrapper. Do not use for NIKE-384/512 or p->n=2048. */
void nike_unpack_bits(poly *a,const unsigned char *in,unsigned t){
  nike_poly x; nike_poly_unpack_bits(&x,in,1024u*t/8u,t,&NIKE_256_PARAMS);
  nike_poly_to_oldpoly_1024(a,&x,&NIKE_256_PARAMS);
}

/* Legacy n=1024 compatibility wrapper. Do not use for NIKE-384/512 or p->n=2048. */
void nike_gen_public(poly *a, poly *dpk, const unsigned char rho[32]){
  nike_poly na,nd; nike_poly_gen_public(&na,&nd,rho,&NIKE_256_PARAMS);
  nike_poly_to_oldpoly_1024(a,&na,&NIKE_256_PARAMS); nike_poly_to_oldpoly_1024(dpk,&nd,&NIKE_256_PARAMS);
}

/* Legacy n=1024 compatibility wrapper. Do not use for NIKE-384/512 or p->n=2048. */
void nike_gen_dither(poly *du, poly *dv, const unsigned char mu[32]){
  nike_poly nu,nv; nike_poly_gen_dither(&nu,&nv,mu,&NIKE_256_PARAMS);
  nike_poly_to_oldpoly_1024(du,&nu,&NIKE_256_PARAMS); nike_poly_to_oldpoly_1024(dv,&nv,&NIKE_256_PARAMS);
}

/* Legacy n=1024 compatibility wrapper. Do not use for NIKE-384/512 or p->n=2048. */
/* Legacy n=1024 compatibility wrapper. Do not use for NIKE-384/512 or p->n=2048. */
void nike_mul_coeff_ntt(poly *out,const poly *a,const poly *b){ nike_mul_negacyclic_ntt(out,a,b); }

/* Legacy n=1024 compatibility wrapper. Do not use for NIKE-384/512 or p->n=2048. */
void nike_mul_schoolbook(poly *out, const poly *a, const poly *b, const nike_params *params){
  nike_poly na,nb,no; oldpoly_to_nike_poly_1024(&na,a,params); oldpoly_to_nike_poly_1024(&nb,b,params);
  nike_poly_mul_schoolbook(&no,&na,&nb,params); nike_poly_to_oldpoly_1024(out,&no,params);
}

void nike_mul_coeff(poly *out,const poly *a,const poly *b){ if(g_backend==NIKE_MUL_BACKEND_SCHOOLBOOK) nike_mul_schoolbook(out,a,b,&NIKE_256_PARAMS); else nike_mul_coeff_ntt(out,a,b); }

void nike_init(nike_state *st, unsigned char *M_A, const nike_params *p){
  unsigned char rho[32],seed[32];
  nike_poly a,dpk,s,tmp,b;
  randombytes(rho,32); randombytes(seed,32);
  nike_poly_gen_public(&a,&dpk,rho,p);
  nike_poly_sample_secret(&s,seed,0,p);
  nike_poly_mul(&tmp,&a,&s,p,g_backend);
  nike_poly_quantize(&b,&tmp,&dpk,p->t_pk,p);
  memcpy(M_A,rho,32);
  nike_poly_pack_bits(M_A+32,(size_t)p->n*p->t_pk/8,&b,p->t_pk,p);
  st->p=p; st->s=s;
}

void nike_resp(unsigned char *M_B, unsigned char *K_B, const unsigned char *M_A, const nike_params *p){
  unsigned char rho[32],sigma[32],mu[32],raw[64];
  nike_poly a,dpk,b,bhat,r,du,dv,tmpu,u,uhat,tmpv,v,vhat,h;
  memcpy(rho,M_A,32);
  nike_poly_gen_public(&a,&dpk,rho,p);
  nike_poly_unpack_bits(&b,M_A+32,(size_t)p->n*p->t_pk/8,p->t_pk,p);
  nike_poly_dequantize(&bhat,&b,&dpk,p->t_pk,p);
  randombytes(sigma,32); randombytes(mu,32);
  nike_poly_sample_secret(&r,sigma,0,p);
  nike_poly_gen_dither(&du,&dv,mu,p);
  nike_poly_mul(&tmpu,&a,&r,p,g_backend);
  nike_poly_quantize(&u,&tmpu,&du,p->t_u,p);
  nike_poly_dequantize(&uhat,&u,&du,p->t_u,p);
  nike_poly_mul(&tmpv,&bhat,&r,p,g_backend);
  nike_poly_quantize(&v,&tmpv,&dv,p->t_v,p);
  nike_poly_dequantize(&vhat,&v,&dv,p->t_v,p);
  nike_helprec_poly(&h,&vhat,sigma,1,p);
  nike_rec_poly(raw,&vhat,&h,p);
  memcpy(M_B,mu,32);
  nike_poly_pack_bits(M_B+32,(size_t)p->n*p->t_u/8,&u,p->t_u,p);
  for(unsigned i=0;i<p->kappa;i++) M_B[32 + (size_t)p->n*p->t_u/8 + i]=(uint8_t)(h.coeffs[i]|(h.coeffs[i+p->kappa]<<2)|(h.coeffs[i+2*p->kappa]<<4)|(h.coeffs[i+3*p->kappa]<<6));
  nike_kdf(K_B,p->ss_bytes,raw,p->kappa/8,M_A,nike_ma_bytes(p),M_B,nike_mb_bytes(p));
}

void nike_derive(unsigned char *K_A, const nike_state *st, const unsigned char *M_A, const unsigned char *M_B){
  const nike_params *p=st->p;
  unsigned char raw[64],mu[32];
  nike_poly du,dv,u,uhat,w,h;
  memcpy(mu,M_B,32);
  nike_poly_gen_dither(&du,&dv,mu,p);
  nike_poly_unpack_bits(&u,M_B+32,(size_t)p->n*p->t_u/8,p->t_u,p);
  nike_poly_dequantize(&uhat,&u,&du,p->t_u,p);
  nike_poly_mul(&w,&uhat,&st->s,p,g_backend);
  for(unsigned i=0;i<p->kappa;i++){ unsigned char hb=M_B[32 + (size_t)p->n*p->t_u/8 + i]; h.coeffs[i]=hb&3; h.coeffs[i+p->kappa]=(hb>>2)&3; h.coeffs[i+2*p->kappa]=(hb>>4)&3; h.coeffs[i+3*p->kappa]=(hb>>6)&3; }
  nike_rec_poly(raw,&w,&h,p);
  nike_kdf(K_A,p->ss_bytes,raw,p->kappa/8,M_A,nike_ma_bytes(p),M_B,nike_mb_bytes(p));
}
