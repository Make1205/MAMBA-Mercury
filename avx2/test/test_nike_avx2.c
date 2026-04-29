#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "../cpucycles.h"
#include "../../ref/nike.h"

static void run_profile(const nike_params *p, unsigned iters) {
  unsigned char MA[4096], MB[4096], KA[64], KB[64];
  nike_state st;
  unsigned ok=0;
  unsigned long long c0,c1, init_sum=0, resp_sum=0, derive_sum=0;
  nike_set_mul_backend(NIKE_MUL_BACKEND_NTT);
  for(unsigned i=0;i<iters;i++){
    c0=cpucycles(); nike_init(&st,MA,p); c1=cpucycles(); init_sum += (c1-c0);
    c0=cpucycles(); nike_resp(MB,KB,MA,p); c1=cpucycles(); resp_sum += (c1-c0);
    c0=cpucycles(); nike_derive(KA,&st,MA,MB); c1=cpucycles(); derive_sum += (c1-c0);
    if(memcmp(KA,KB,p->ss_bytes)==0) ok++;
  }
  const char *impl = (p->n==1024)?"avx2-native":"avx2-mixed";
  printf("%s,%s,%s,%llu,%llu,%llu,%zu,%zu,%zu,%u\n",
    p->name, impl, (ok==iters?"success":"fail"),
    init_sum/iters, resp_sum/iters, derive_sum/iters,
    nike_ma_bytes(p), nike_mb_bytes(p), nike_ma_bytes(p)+nike_mb_bytes(p), p->ss_bytes);
}

int main(int argc, char **argv){
  unsigned iters=1000;
  if(argc>1) iters=(unsigned)strtoul(argv[1],NULL,10);
  printf("profile,implementation,correctness,init_cycles,resp_cycles,derive_cycles,MA_bytes,MB_bytes,total_bytes,ss_bytes\n");
  run_profile(&NIKE_128,iters);
  run_profile(&NIKE_192,iters);
  run_profile(&NIKE_256,iters);
  run_profile(&NIKE_384,iters);
  run_profile(&NIKE_512,iters);
  return 0;
}
