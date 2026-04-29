#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "../nike.h"
#include "../cpucycles.h"
#include "../quantize.h"
#include "../error_correction.h"
#include "../randombytes.h"
#include "../nike_ntt.h"
#include "../nike_poly.h"
#include "../nike_compat.h"
#include "../fips202.h"

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
  { unsigned char out1[64],out2[64],in[64]={1}; int okk=1; for(int i=0;i<5;i++){ unsigned L[5]={16,24,32,48,64}; sha3256(out1,in,64); sha3256(out2,in,64); size_t cmp=L[i]<32?L[i]:32; if(memcmp(out1,out2,cmp)) okk=0; in[0]^=1; sha3256(out2,in,64); if(!memcmp(out1,out2,cmp)) okk=0; in[0]^=1;} printf("KDF variable output self-test: %s\n",okk?"PASS":"FAIL"); }
  uint16_t v[PARAM_N],w[PARAM_N]; ok=1; for(int t=0;t<100;t++){ for(int i=0;i<PARAM_N;i++) v[i]=rand()%PARAM_Q; memcpy(w,v,sizeof(v)); nike_cyclic_ntt(w); nike_cyclic_intt(w); for(int i=0;i<PARAM_N;i++) if(v[i]!=w[i]){ ok=0; break; } if(!ok) break;}
  printf("cyclic NTT roundtrip (100): %s\n",ok?"PASS":"FAIL");
  printf("primitive psi (n=1024): %u\n", nike_ntt_psi());
  rand_poly(&a); rand_poly(&b); nike_mul_schoolbook(&d,&a,&b,&NIKE_256); nike_mul_coeff_ntt(&c,&a,&b); ok=!memcmp(&c,&d,sizeof(poly));
  printf("NTT mul vs schoolbook(1 sample): %s\n",ok?"PASS":"FAIL");
  for(int ki=0;ki<3;ki++){ unsigned kappa=(ki==0)?128:(ki==1)?192:256; unsigned char key1[32]={0},key2[32]={0},no[32]={0}; rand_poly(&a); helprec_kappa(&b,&a,no,0,kappa); rec_kappa(key1,&a,&b,kappa); rec_kappa(key2,&a,&b,kappa); printf("D4 identity kappa=%u: %s\n",kappa,memcmp(key1,key2,32)?"FAIL":"PASS"); }
  ok=1; for(int t=0;t<100;t++){ rand_poly(&a); rand_poly(&b); nike_mul_schoolbook(&d,&a,&b,&NIKE_256); nike_mul_coeff_ntt(&c,&a,&b); for(int i=0;i<PARAM_N;i++){ if(d.coeffs[i]!=c.coeffs[i]){ printf("ntt mismatch idx=%d ref=%u got=%u diff=%d\n",i,d.coeffs[i],c.coeffs[i],centered((int)c.coeffs[i]-(int)d.coeffs[i])); ok=0; t=100; break; } } }
  printf("NTT mul vs schoolbook (100 pairs): %s\n",ok?"PASS":"FAIL");

  memset(&a,0,sizeof(a)); a.coeffs[0]=1; rand_poly(&b); nike_mul_coeff_ntt(&c,&a,&b); ok=!memcmp(&b,&c,sizeof(poly)); printf("NTT special a=1: %s\n",ok?"PASS":"FAIL");
  memset(&a,0,sizeof(a)); a.coeffs[1]=1; rand_poly(&b); nike_mul_schoolbook(&d,&a,&b,&NIKE_256); nike_mul_coeff_ntt(&c,&a,&b); ok=!memcmp(&d,&c,sizeof(poly)); printf("NTT special a=X: %s\n",ok?"PASS":"FAIL");
  memset(&a,0,sizeof(a)); memset(&b,0,sizeof(b)); a.coeffs[PARAM_N-1]=1; b.coeffs[1]=1; nike_mul_coeff_ntt(&c,&a,&b); ok=(c.coeffs[0]==PARAM_Q-1); printf("NTT special X^{n-1}*X=-1: %s\n",ok?"PASS":"FAIL");

  uint16_t a2[2048],b2[2048],o2[2048],ref2[2048],tmp2[2048];
  ok=1; for(int t=0;t<100;t++){ for(int i=0;i<2048;i++) a2[i]=rand()%PARAM_Q; memcpy(tmp2,a2,sizeof(a2)); nike_cyclic_ntt_n(tmp2,2048); nike_cyclic_intt_n(tmp2,2048); for(int i=0;i<2048;i++) if(a2[i]!=tmp2[i]){ok=0;break;} if(!ok) break; }
  printf("n=2048 cyclic NTT roundtrip (100): %s\n",ok?"PASS":"FAIL");
  printf("primitive psi (n=2048): %u\n", nike_ntt_psi_n(2048));
  uint16_t psi=nike_ntt_psi_n(2048); uint16_t q=PARAM_Q; 
  uint16_t pown=1,pow2n=1,omega=1,pown2=1; for(int i=0;i<2048;i++) pown=(uint32_t)pown*psi%q; for(int i=0;i<4096;i++) pow2n=(uint32_t)pow2n*psi%q; omega=(uint32_t)psi*psi%q; for(int i=0;i<2048;i++) pown2=(uint32_t)pown2*omega%q; 
  printf("n=2048 root sanity: %s\n", (pown==q-1 && pow2n==1 && pown2==1)?"PASS":"FAIL");
  ok=1; for(int t=0;t<20;t++){ static int64_t acc2[2048]; for(int i=0;i<2048;i++){ a2[i]=rand()%PARAM_Q; b2[i]=rand()%PARAM_Q; acc2[i]=0; tmp2[i]=a2[i]; o2[i]=b2[i]; } nike_cyclic_ntt_n(tmp2,2048); nike_cyclic_ntt_n(o2,2048); for(int i=0;i<2048;i++) tmp2[i]=(uint32_t)tmp2[i]*o2[i]%PARAM_Q; nike_cyclic_intt_n(tmp2,2048); for(int i=0;i<2048;i++) for(int j=0;j<2048;j++){ int k=i+j; int64_t v=(int64_t)a2[i]*b2[j]; if(k<2048) acc2[k]+=v; else acc2[k-2048]+=v; } for(int i=0;i<2048;i++){ int64_t z=acc2[i]%PARAM_Q; if(z<0) z+=PARAM_Q; if(tmp2[i]!=(uint16_t)z){ ok=0; break; } } if(!ok) break; }
  printf("n=2048 cyclic convolution theorem (20): %s\n",ok?"PASS":"FAIL");
  for(int i=0;i<2048;i++) b2[i]=rand()%PARAM_Q; memset(a2,0,sizeof(a2)); a2[0]=1; nike_mul_negacyclic_ntt_n(o2,a2,b2,2048); ok=1; for(int i=0;i<2048;i++) if(o2[i]!=b2[i]) ok=0; printf("n=2048 special a=1: %s\n",ok?"PASS":"FAIL");
  memset(a2,0,sizeof(a2)); a2[1]=1; nike_mul_negacyclic_ntt_n(o2,a2,b2,2048); ok=(o2[0]==(PARAM_Q-b2[2047])%PARAM_Q); for(int i=1;i<2048&&ok;i++) if(o2[i]!=b2[i-1]) ok=0; printf("n=2048 special a=X: %s\n",ok?"PASS":"FAIL");
  memset(a2,0,sizeof(a2)); memset(b2,0,sizeof(b2)); a2[2047]=1; b2[1]=1; nike_mul_negacyclic_ntt_n(o2,a2,b2,2048); ok=(o2[0]==PARAM_Q-1); for(int i=1;i<2048&&ok;i++) if(o2[i]!=0) ok=0; printf("n=2048 special X^{n-1}*X=-1: %s\n",ok?"PASS":"FAIL");
  ok=1; for(int t=0;t<20;t++){ static int64_t acc[2048]; for(int i=0;i<2048;i++){ a2[i]=rand()%PARAM_Q; b2[i]=rand()%PARAM_Q; acc[i]=0; } for(int i=0;i<2048;i++) for(int j=0;j<2048;j++){ int k=i+j; int64_t v=(int64_t)a2[i]*b2[j]; if(k<2048) acc[k]+=v; else acc[k-2048]-=v; } for(int i=0;i<2048;i++){ int64_t z=acc[i]%PARAM_Q; if(z<0) z+=PARAM_Q; ref2[i]=(uint16_t)z; } nike_mul_negacyclic_ntt_n(o2,a2,b2,2048); int mism=0,maxd=0,first=-1; for(int i=0;i<2048;i++){ int d=centered((int)o2[i]-(int)ref2[i]); if(d<0) d=-d; if(d>maxd) maxd=d; if(o2[i]!=ref2[i]){ mism++; if(first<0) first=i; }} if(mism){ ok=0; printf("n=2048 mismatch test=%d idx=%d ref=%u got=%u diff=%d mism=%d maxabs=%d\n",t,first,ref2[first],o2[first],centered((int)o2[first]-(int)ref2[first]),mism,maxd); printf("a[0..7]="); for(int i=0;i<8;i++) printf(" %u",a2[i]); printf("\n"); printf("b[0..7]="); for(int i=0;i<8;i++) printf(" %u",b2[i]); printf("\n"); printf("ref[0..7]="); for(int i=0;i<8;i++) printf(" %u",ref2[i]); printf("\n"); printf("ntt[0..7]="); for(int i=0;i<8;i++) printf(" %u",o2[i]); printf("\n"); break; } }
  printf("n=2048 NTT vs schoolbook (20): %s\n",ok?"PASS":"FAIL");
}

static int run_mode_once(const nike_params *p,int mode){
 poly a,dpk,du,dv,s,r,b,bhat,u,uhat,v,vhat,w; unsigned char rho[32],mu[32],n[32]={0},k1[64]={0},k2[64]={0};
 randombytes(rho,32); randombytes(mu,32); randombytes(n,32); nike_gen_public(&a,&dpk,rho); nike_gen_dither(&du,&dv,mu); poly_getnoise(&s,n,0); poly_getnoise(&r,n,1);
 nike_mul_coeff(&b,&a,&s); if(mode>=2){ poly tb=b; poly_quantize(&b,&tb,&dpk,p->t_pk); poly_dequantize(&bhat,&b,&dpk,p->t_pk);} else bhat=b;
 nike_mul_coeff(&u,&a,&r); if(mode>=2){ poly tu=u; poly_quantize(&u,&tu,&du,p->t_u); poly_dequantize(&uhat,&u,&du,p->t_u);} else uhat=u;
 nike_mul_coeff(&v,&bhat,&r); if(mode==1||mode==3){ poly tv=v; poly_quantize(&v,&tv,&dv,p->t_v); poly_dequantize(&vhat,&v,&dv,p->t_v);} else vhat=v;
 helprec_kappa(&b,&vhat,n,2,p->kappa); rec_kappa(k1,&vhat,&b,p->kappa); nike_mul_coeff(&w,&uhat,&s); rec_kappa(k2,&w,&b,p->kappa); return memcmp(k1,k2,p->ss_bytes)==0;
}

static void run_backend(const nike_params *p, nike_mul_backend be,const char*name){
  nike_set_mul_backend(be);
  printf("backend=%s %s\n",name,p->name);
  for(int m=0;m<4;m++){ int ok=0; for(int i=0;i<1000;i++) ok+=run_mode_once(p,m); printf("%s mode%d success %d/1000\n",p->name,m,ok);}
  printf("ss_bytes=%u |M_A|=%zu |M_B|=%zu total=%zu\n",p->ss_bytes,nike_ma_bytes(p),nike_mb_bytes(p),nike_ma_bytes(p)+nike_mb_bytes(p));
}

static void run(const nike_params *p){
  run_backend(p,NIKE_MUL_BACKEND_SCHOOLBOOK,"schoolbook");
  run_backend(p,NIKE_MUL_BACKEND_NTT,"ntt");
}

static void nike_poly_layer_tests(){
  nike_poly a,b,d,c; uint8_t buf[4096]; poly old;
  const nike_params* arr[5]={&NIKE_128,&NIKE_192,&NIKE_256,&NIKE_384,&NIKE_512};
  for(int pi=0;pi<5;pi++){ const nike_params*p=arr[pi]; for(unsigned i=0;i<p->n;i++){ a.coeffs[i]=rand()%p->q; d.coeffs[i]=rand()%p->q; }
    if(p->n==1024){ int r1=nike_poly_to_oldpoly_1024(&old,&a,p); int r2=oldpoly_to_nike_poly_1024(&b,&old,p); printf("boundary %s: %s\n",p->name,(r1==0&&r2==0&&nike_poly_equal(&a,&b,p))?"PASS":"FAIL"); }
    else { int r1=nike_poly_to_oldpoly_1024(&old,&a,p); int r2=oldpoly_to_nike_poly_1024(&b,&old,p); printf("boundary %s reject: %s\n",p->name,(r1!=0&&r2!=0)?"PASS":"FAIL"); }
  }
  for(int ni=0;ni<2;ni++){ const nike_params*p=(ni==0)?&NIKE_256:&NIKE_512; for(int ti=0;ti<2;ti++){ unsigned t=(ti==0)?10:11; for(unsigned i=0;i<p->n;i++) a.coeffs[i]=rand()&((1u<<t)-1); nike_poly_pack_bits(buf,sizeof(buf),&a,t,p); nike_poly_unpack_bits(&b,buf,(size_t)p->n*t/8,t,p); printf("nike_poly pack %s t=%u: %s\n",p->name,t,nike_poly_equal(&a,&b,p)?"PASS":"FAIL"); }}
  for(int ni=0;ni<2;ni++){ const nike_params*p=(ni==0)?&NIKE_256:&NIKE_512; for(int ti=0;ti<3;ti++){ unsigned t=(ti==0)?10:(ti==1)?11:6; for(unsigned i=0;i<p->n;i++){ a.coeffs[i]=rand()%p->q; d.coeffs[i]=rand()%p->q;} nike_poly_quantize(&c,&a,&d,t,p); nike_poly_dequantize(&b,&c,&d,t,p); int ok=1; for(unsigned i=0;i<p->n;i++){ int e=centered((int)b.coeffs[i]-(int)a.coeffs[i]); if(e<0)e=-e; int bound=(p->q + (1<<(t+1))-1)/(1<<(t+1))+1; if(e>bound){ok=0;break;}} printf("nike_poly qdq %s t=%u: %s\n",p->name,t,ok?"PASS":"FAIL"); }}
  for(int pi=0;pi<5;pi++){ const nike_params*p=arr[pi]; uint8_t seed[32]={9}; nike_poly_gen_public(&a,&b,seed,p); nike_poly_gen_public(&c,&d,seed,p); printf("nike_poly gen public %s: %s\n",p->name,nike_poly_equal(&a,&c,p)&&nike_poly_equal(&b,&d,p)?"PASS":"FAIL"); nike_poly_gen_dither(&a,&b,seed,p); nike_poly_gen_dither(&c,&d,seed,p); printf("nike_poly gen dither %s: %s\n",p->name,nike_poly_equal(&a,&c,p)&&nike_poly_equal(&b,&d,p)?"PASS":"FAIL"); }
}

int main(){ self_tests(); nike_poly_layer_tests(); run(&NIKE_128); run(&NIKE_192); run(&NIKE_256); return 0; }
