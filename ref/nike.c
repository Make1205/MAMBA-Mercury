#include "nike.h"
#include "quantize.h"
#include "error_correction.h"
#include "fips202.h"
#include "randombytes.h"
#include "poly.h"
#include "nike_ntt.h"
#include <string.h>

const nike_params NIKE_128={"NIKE-128",10,10,6,128};
const nike_params NIKE_192={"NIKE-192",11,11,6,192};
const nike_params NIKE_256={"NIKE-256",11,11,6,256};
const nike_params NIKE_384={"NIKE-384",11,11,6,384};
const nike_params NIKE_512={"NIKE-512",11,11,6,512};

static nike_mul_backend g_backend = NIKE_MUL_BACKEND_NTT;
void nike_set_mul_backend(nike_mul_backend backend){ g_backend = backend; }

size_t nike_ma_bytes(const nike_params *p){ return 32 + PARAM_N*p->t_pk/8; }
size_t nike_mb_bytes(const nike_params *p){ return 32 + PARAM_N*p->t_u/8 + p->kappa; }

void nike_pack_bits(unsigned char *out, const poly *a, unsigned t){ memset(out,0,PARAM_N*t/8); unsigned b=0; for(int i=0;i<PARAM_N;i++){ unsigned v=a->coeffs[i]&((1u<<t)-1u); for(unsigned j=0;j<t;j++,b++) out[b>>3]|=((v>>j)&1u)<<(b&7);} }
void nike_unpack_bits(poly *a,const unsigned char *in,unsigned t){ unsigned b=0; for(int i=0;i<PARAM_N;i++){ unsigned v=0; for(unsigned j=0;j<t;j++,b++) v|=((in[b>>3]>>(b&7))&1u)<<j; a->coeffs[i]=v; } }

static void domain_uniform(poly *r,const unsigned char *seed,const char *tag){ uint64_t st[25]; uint8_t in[64]={0}; memcpy(in,seed,32); size_t tl=strlen(tag); memcpy(in+32,tag,tl>31?31:tl); shake128_absorb(st,in,64); unsigned ctr=0,pos=0; uint8_t buf[SHAKE128_RATE]; shake128_squeezeblocks(buf,1,st); while(ctr<PARAM_N){ uint16_t v=(buf[pos]|((uint16_t)buf[pos+1]<<8))&0x3fff; if(v<PARAM_Q) r->coeffs[ctr++]=v; pos+=2; if(pos>SHAKE128_RATE-2){ shake128_squeezeblocks(buf,1,st); pos=0; } } }

void nike_gen_public(poly *a, poly *dpk, const unsigned char rho[32]){ domain_uniform(a,rho,"NIKE-A"); domain_uniform(dpk,rho,"NIKE-DPK"); }
void nike_gen_dither(poly *du, poly *dv, const unsigned char mu[32]){ domain_uniform(du,mu,"NIKE-DU"); domain_uniform(dv,mu,"NIKE-DV"); }
void nike_mul_coeff_ntt(poly *out,const poly *a,const poly *b){ nike_mul_negacyclic_ntt(out,a,b); }

void nike_mul_schoolbook(poly *out, const poly *a, const poly *b, const nike_params *params){ (void)params; int64_t acc[PARAM_N]={0}; for(int i=0;i<PARAM_N;i++){ for(int j=0;j<PARAM_N;j++){ int k=i+j; int64_t v=(int64_t)a->coeffs[i]*(int64_t)b->coeffs[j]; if(k<PARAM_N) acc[k]+=v; else acc[k-PARAM_N]-=v; } } for(int i=0;i<PARAM_N;i++){ int64_t v=acc[i]%PARAM_Q; if(v<0) v+=PARAM_Q; out->coeffs[i]=(uint16_t)v; }}

void nike_mul_coeff(poly *out,const poly *a,const poly *b){ if(g_backend==NIKE_MUL_BACKEND_SCHOOLBOOK) nike_mul_schoolbook(out,a,b,&NIKE_256); else nike_mul_coeff_ntt(out,a,b); }

void nike_init(nike_state *st, unsigned char *M_A, const nike_params *p){ unsigned char rho[32],noise[32]; poly a,dpk,tmp,b; randombytes(rho,32); randombytes(noise,32); st->p=*p; nike_gen_public(&a,&dpk,rho); poly_getnoise(&st->s,noise,0); nike_mul_coeff(&tmp,&a,&st->s); poly_quantize(&b,&tmp,&dpk,p->t_pk); memcpy(M_A,rho,32); nike_pack_bits(M_A+32,&b,p->t_pk); }

void nike_resp(unsigned char *M_B, unsigned char *K_B, const unsigned char *M_A, const nike_params *p){ unsigned char rho[32],mu[32],noise[32],raw[64]; poly a,dpk,b,bhat,r,du,dv,tmpu,u,uhat,tmpv,v,vhat,h; memcpy(rho,M_A,32); nike_gen_public(&a,&dpk,rho); nike_unpack_bits(&b,M_A+32,p->t_pk); poly_dequantize(&bhat,&b,&dpk,p->t_pk); randombytes(mu,32); randombytes(noise,32); poly_getnoise(&r,noise,0); nike_gen_dither(&du,&dv,mu); nike_mul_coeff(&tmpu,&a,&r); poly_quantize(&u,&tmpu,&du,p->t_u); poly_dequantize(&uhat,&u,&du,p->t_u); nike_mul_coeff(&tmpv,&bhat,&r); poly_quantize(&v,&tmpv,&dv,p->t_v); poly_dequantize(&vhat,&v,&dv,p->t_v); helprec_kappa(&h,&vhat,noise,1,p->kappa); rec_kappa(raw,&vhat,&h,p->kappa); memcpy(M_B,mu,32); nike_pack_bits(M_B+32,&u,p->t_u); for(unsigned i=0;i<p->kappa;i++) M_B[32+PARAM_N*p->t_u/8+i]=h.coeffs[i]|(h.coeffs[i+p->kappa]<<2)|(h.coeffs[i+2*p->kappa]<<4)|(h.coeffs[i+3*p->kappa]<<6); sha3256(K_B,raw,(p->kappa+7)/8); }

void nike_derive(unsigned char *K_A, const nike_state *st, const unsigned char *M_A, const unsigned char *M_B){ const nike_params *p=&st->p; unsigned char raw[64]; poly du,dv,u,uhat,w,h; (void)M_A; nike_gen_dither(&du,&dv,M_B); nike_unpack_bits(&u,M_B+32,p->t_u); poly_dequantize(&uhat,&u,&du,p->t_u); nike_mul_coeff(&w,&uhat,&st->s); for(unsigned i=0;i<p->kappa;i++){ unsigned char b=M_B[32+PARAM_N*p->t_u/8+i]; h.coeffs[i]=b&3; h.coeffs[i+p->kappa]=(b>>2)&3; h.coeffs[i+2*p->kappa]=(b>>4)&3; h.coeffs[i+3*p->kappa]=(b>>6)&3; } rec_kappa(raw,&w,&h,p->kappa); sha3256(K_A,raw,(p->kappa+7)/8); }
