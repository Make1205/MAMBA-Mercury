#include <stdio.h>
#include <string.h>
#include "../nike.h"
#include "../cpucycles.h"

static void run(const nike_params *p){
  unsigned ok=0; unsigned char MA[3000],MB[4000],KA[32],KB[32]; nike_state st;
  uint64_t c0=cpucycles();
  for(int i=0;i<1000;i++){
    nike_init(&st,MA,p); nike_resp(MB,KB,MA,p); nike_derive(KA,&st,MA,MB);
    if(!memcmp(KA,KB,32)) ok++;
  }
  uint64_t c1=cpucycles();
  printf("%s success %u/1000\n",p->name,ok);
  printf("|M_A|=%zu |M_B|=%zu total=%zu\n",nike_ma_bytes(p),nike_mb_bytes(p),nike_ma_bytes(p)+nike_mb_bytes(p));
  printf("cycles(total loop): %llu\n",(unsigned long long)(c1-c0));
}
int main(){ run(&NIKE_128); run(&NIKE_192); run(&NIKE_256); return 0; }
