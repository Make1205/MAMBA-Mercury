#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "../nike.h"
#include "../cpucycles.h"
#include "../quantize.h"
#include "../error_correction.h"
#include "../randombytes.h"

static int centered(int x){ x%=PARAM_Q; if(x<0)x+=PARAM_Q; if(x>PARAM_Q/2)x-=PARAM_Q; return x; }
static void rand_poly(poly *a){ unsigned char b[2]; for(int i=0;i<PARAM_N;i++){ randombytes(b,2); a->coeffs[i]=((unsigned)b[0]|((unsigned)b[1]<<8))%PARAM_Q; } }
static void schoolbook(poly *o,const poly *a,const poly *b){ int32_t t[PARAM_N]={0}; for(int i=0;i<PARAM_N;i++) for(int j=0;j<PARAM_N;j++){ int idx=i+j; int32_t v=(int32_t)a->coeffs[i]*b->coeffs[j]; if(idx>=PARAM_N){ idx-=PARAM_N; v=-v; } t[idx]+=v; } for(int i=0;i<PARAM_N;i++){ int32_t v=t[i]%PARAM_Q; if(v<0)v+=PARAM_Q; o->coeffs[i]=v; }}

static void self_tests(){
  poly a,b,c,d; unsigned char buf[1500]; int ok=1;
  for(unsigned t=10;t<=11;t++){
    for(int i=0;i<PARAM_N;i++) a.coeffs[i]=rand()&((1u<<t)-1u);
    nike_pack_bits(buf,&a,t); nike_unpack_bits(&b,buf,t);
    for(int i=0;i<PARAM_N;i++) if(a.coeffs[i]!=b.coeffs[i]) ok=0;
  }
  printf("pack/unpack t=10/11: %s\n",ok?"PASS":"FAIL");
  ok=1; for(int k=0;k<5000;k++){ unsigned t=(k%3==0)?10:((k%3==1)?11:6); uint16_t x=rand()%PARAM_Q,dv=rand()%PARAM_Q; uint16_t y=coeff_dequantize(coeff_quantize(x,dv,t),dv,t); int e=abs(centered((int)y-(int)x)); int bound=(PARAM_Q + (1<<(t+1))-1)/(1<<(t+1))+1; if(e>bound) ok=0; }
  printf("quantize/dequantize t=10/11/6: %s\n",ok?"PASS":"FAIL");
  unsigned char seed[32]={7}; poly du1,dv1,du2,dv2,ap1,dp1,ap2,dp2; nike_gen_dither(&du1,&dv1,seed); nike_gen_dither(&du2,&dv2,seed); nike_gen_public(&ap1,&dp1,seed); nike_gen_public(&ap2,&dp2,seed);
  printf("GenDither reproducibility: %s\n",memcmp(&du1,&du2,sizeof(poly))||memcmp(&dv1,&dv2,sizeof(poly))?"FAIL":"PASS");
  printf("GenPublic reproducibility: %s\n",memcmp(&ap1,&ap2,sizeof(poly))||memcmp(&dp1,&dp2,sizeof(poly))?"FAIL":"PASS");
  rand_poly(&a); rand_poly(&b); nike_mul_schoolbook(&d,&a,&b,&NIKE_256); nike_mul_coeff_ntt(&c,&a,&b); ok=!memcmp(&c,&d,sizeof(poly));
  printf("NTT mul vs schoolbook(1 sample): %s\n",ok?"PASS":"FAIL");
  for(int ki=0;ki<3;ki++){ unsigned kappa=(ki==0)?128:(ki==1)?192:256; unsigned char key1[32]={0},key2[32]={0},no[32]={0}; rand_poly(&a); helprec_kappa(&b,&a,no,0,kappa); rec_kappa(key1,&a,&b,kappa); rec_kappa(key2,&a,&b,kappa); printf("D4 identity kappa=%u: %s\n",kappa,memcmp(key1,key2,32)?"FAIL":"PASS"); }
  ok=1; for(int t=0;t<100;t++){ rand_poly(&a); rand_poly(&b); nike_mul_schoolbook(&d,&a,&b,&NIKE_256); nike_mul_coeff_ntt(&c,&a,&b); for(int i=0;i<PARAM_N;i++){ if(d.coeffs[i]!=c.coeffs[i]){ printf("ntt mismatch idx=%d ref=%u got=%u diff=%d\n",i,d.coeffs[i],c.coeffs[i],centered((int)c.coeffs[i]-(int)d.coeffs[i])); ok=0; t=100; break; } } }
  printf("NTT mul vs schoolbook (100 pairs): %s\n",ok?"PASS":"FAIL");

  memset(&a,0,sizeof(a)); a.coeffs[0]=1; rand_poly(&b); nike_mul_coeff_ntt(&c,&a,&b); ok=!memcmp(&b,&c,sizeof(poly)); printf("NTT special a=1: %s\n",ok?"PASS":"FAIL");
  memset(&a,0,sizeof(a)); a.coeffs[1]=1; rand_poly(&b); nike_mul_schoolbook(&d,&a,&b,&NIKE_256); nike_mul_coeff_ntt(&c,&a,&b); ok=!memcmp(&d,&c,sizeof(poly)); printf("NTT special a=X: %s\n",ok?"PASS":"FAIL");
  memset(&a,0,sizeof(a)); memset(&b,0,sizeof(b)); a.coeffs[PARAM_N-1]=1; b.coeffs[1]=1; nike_mul_coeff_ntt(&c,&a,&b); ok=(c.coeffs[0]==PARAM_Q-1); printf("NTT special X^{n-1}*X=-1: %s\n",ok?"PASS":"FAIL");

}

static int run_mode_once(const nike_params *p,int mode){
 poly a,dpk,du,dv,s,r,b,bhat,u,uhat,v,vhat,w; unsigned char rho[32],mu[32],n[32]={0},k1[32]={0},k2[32]={0};
 randombytes(rho,32); randombytes(mu,32); randombytes(n,32); nike_gen_public(&a,&dpk,rho); nike_gen_dither(&du,&dv,mu); poly_getnoise(&s,n,0); poly_getnoise(&r,n,1);
 nike_mul_coeff(&b,&a,&s); if(mode>=2){ poly tb=b; poly_quantize(&b,&tb,&dpk,p->t_pk); poly_dequantize(&bhat,&b,&dpk,p->t_pk);} else bhat=b;
 nike_mul_coeff(&u,&a,&r); if(mode>=2){ poly tu=u; poly_quantize(&u,&tu,&du,p->t_u); poly_dequantize(&uhat,&u,&du,p->t_u);} else uhat=u;
 nike_mul_coeff(&v,&bhat,&r); if(mode==1||mode==3){ poly tv=v; poly_quantize(&v,&tv,&dv,p->t_v); poly_dequantize(&vhat,&v,&dv,p->t_v);} else vhat=v;
 helprec_kappa(&b,&vhat,n,2,p->kappa); rec_kappa(k1,&vhat,&b,p->kappa); nike_mul_coeff(&w,&uhat,&s); rec_kappa(k2,&w,&b,p->kappa); return memcmp(k1,k2,32)==0;
}

static void run_backend(const nike_params *p, nike_mul_backend be,const char*name){
  nike_set_mul_backend(be);
  printf("backend=%s %s\n",name,p->name);
  for(int m=0;m<4;m++){ int ok=0; for(int i=0;i<1000;i++) ok+=run_mode_once(p,m); printf("%s mode%d success %d/1000\n",p->name,m,ok);}
  printf("|M_A|=%zu |M_B|=%zu total=%zu\n",nike_ma_bytes(p),nike_mb_bytes(p),nike_ma_bytes(p)+nike_mb_bytes(p));
}

static void run(const nike_params *p){
  run_backend(p,NIKE_MUL_BACKEND_SCHOOLBOOK,"schoolbook");
  run_backend(p,NIKE_MUL_BACKEND_NTT,"ntt");
}
int main(){ self_tests(); run(&NIKE_128); run(&NIKE_192); run(&NIKE_256); return 0; }
