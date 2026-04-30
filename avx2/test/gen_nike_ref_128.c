#include <stdio.h>
#include <string.h>
#include "../../ref/nike.h"
extern unsigned long long rb_call_count; extern unsigned long long rb_call_lens[256];
int main(){ unsigned char MA[4096],MB[4096],KA[64],KB[64]; nike_state st; const nike_params*p=&NIKE_128; nike_init(&st,MA,p); nike_resp(MB,KB,MA,p); nike_derive(KA,&st,MA,MB); printf("M1_Len=%zu\nM2_Len=%zu\nSS_Len=%u\n",nike_ma_bytes(p),nike_mb_bytes(p),p->ss_bytes); printf("RNG_Calls=%llu\n",rb_call_count); for(unsigned i=0;i<rb_call_count && i<32;i++) printf("%llu%s",rb_call_lens[i], (i+1<rb_call_count && i<31)?",":"\n"); if(rb_call_count==0) printf("\n"); for(size_t i=0;i<nike_ma_bytes(p);i++) printf("%02x",MA[i]); printf("\n"); for(size_t i=0;i<nike_mb_bytes(p);i++) printf("%02x",MB[i]); printf("\n"); for(unsigned i=0;i<p->ss_bytes;i++) printf("%02x",KA[i]); printf("\n"); return memcmp(KA,KB,p->ss_bytes)!=0; }
