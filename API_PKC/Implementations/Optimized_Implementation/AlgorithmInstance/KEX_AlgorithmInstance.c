#include "KEX_AlgorithmInstance.h"
#include "drng.h"
#include "../NIKE/nike.h"
#include <string.h>
extern DRNG_ctx drng_algorithm;
static const nike_params* params(void){
#if NIKE_PROFILE==128
return &NIKE_128_PARAMS;
#elif NIKE_PROFILE==192
return &NIKE_192_PARAMS;
#elif NIKE_PROFILE==256
return &NIKE_256_PARAMS;
#elif NIKE_PROFILE==384
return &NIKE_384_PARAMS;
#else
return &NIKE_512_PARAMS;
#endif
}
unsigned long long kex_get_passes_num(){return 3;} unsigned long long kex_get_pk_len_bytes(){return 0;} unsigned long long kex_get_sk_len_bytes(){return 0;}
unsigned long long kex_get_sta_len_bytes(){return sizeof(nike_state)+4096;} unsigned long long kex_get_stb_len_bytes(){return 0;}
unsigned long long kex_get_ss_len_bytes(){return params()->ss_bytes;} unsigned long long kex_get_total_msg_len_bytes(){return params()->ma_bytes+params()->mb_bytes;}
int kex_init_a(unsigned char *pka,unsigned long long *pka_len,unsigned char *ska,unsigned long long *ska_len,unsigned char *sta,unsigned long long *sta_len){(void)pka;(void)ska;*pka_len=*ska_len=0;*sta_len=0;return 0;}
int kex_init_b(unsigned char *pkb,unsigned long long *pkb_len,unsigned char *skb,unsigned long long *skb_len,unsigned char *stb,unsigned long long *stb_len){(void)pkb;(void)skb;(void)stb;*pkb_len=*skb_len=*stb_len=0;return 0;}
typedef struct { nike_state st; unsigned char ma[2848]; } kex_sta;
int kex_generate_pass1_msg_a(unsigned char *ska,unsigned long long ska_len,unsigned char *pkb,unsigned long long pkb_len,unsigned char *sta,unsigned long long *sta_len,unsigned char *m1,unsigned long long *m1_len){(void)ska;(void)ska_len;(void)pkb;(void)pkb_len; kex_sta loc; nike_init(&loc.st,m1,params()); memcpy(loc.ma,m1,params()->ma_bytes); memcpy(sta,&loc,sizeof(loc)); *sta_len=sizeof(loc); *m1_len=params()->ma_bytes; return 0;}
int kex_generate_pass2_msg_b(unsigned char *skb,unsigned long long skb_len,unsigned char *pka,unsigned long long pka_len,unsigned char *m1,unsigned long long m1_len,unsigned char *stb,unsigned long long *stb_len,unsigned char *m2,unsigned long long *m2_len){(void)skb;(void)skb_len;(void)pka;(void)pka_len;(void)stb;(void)stb_len; if(m1_len!=params()->ma_bytes) return -1; unsigned char kb[64]; nike_resp(m2,kb,m1,params()); *m2_len=params()->mb_bytes; return 0;}
int kex_generate_pass3_msg_a(unsigned char *ska,unsigned long long ska_len,unsigned char *pkb,unsigned long long pkb_len,unsigned char *m2,unsigned long long m2_len,unsigned char *sta,unsigned long long *sta_len,unsigned char *m3,unsigned long long *m3_len){(void)ska;(void)ska_len;(void)pkb;(void)pkb_len;(void)m2;(void)m2_len;(void)sta;(void)sta_len;(void)m3;*m3_len=0;return 0;}
int kex_derive_ss_a(unsigned char *ska,unsigned long long ska_len,unsigned char *pkb,unsigned long long pkb_len,unsigned char *mb,unsigned long long mb_len,unsigned char *sta,unsigned long long sta_len,unsigned char *ssa,unsigned long long *ssa_len){(void)ska;(void)ska_len;(void)pkb;(void)pkb_len;(void)sta_len; if(mb_len!=params()->mb_bytes) return -1; kex_sta loc; memcpy(&loc,sta,sizeof(loc)); nike_derive(ssa,&loc.st,loc.ma,mb); *ssa_len=params()->ss_bytes; return 0;}
int kex_derive_ss_b(unsigned char *skb,unsigned long long skb_len,unsigned char *pka,unsigned long long pka_len,unsigned char *ma,unsigned long long ma_len,unsigned char *stb,unsigned long long stb_len,unsigned char *ssb,unsigned long long *ssb_len){(void)skb;(void)skb_len;(void)pka;(void)pka_len;(void)stb;(void)stb_len; if(ma_len!=params()->ma_bytes) return -1; unsigned char mb[4096]; nike_resp(mb,ssb,ma,params()); *ssb_len=params()->ss_bytes; return 0;}
void randombytes(unsigned char *x, unsigned long long xlen){ get_random_number(&drng_algorithm,x,xlen*8); }
