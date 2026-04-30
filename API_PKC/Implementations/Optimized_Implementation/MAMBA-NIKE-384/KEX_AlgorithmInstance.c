#include "KEX_AlgorithmInstance.h"
#include "src/nike.h"
#include <string.h>

typedef struct { nike_state st; unsigned char ma[4000]; unsigned long long ma_len; int ready; } sta_t;
typedef struct { unsigned char mb[4000]; unsigned char ss[64]; unsigned long long mb_len; int ready; } stb_t;

static const nike_params* P(void){ return &NIKE_384; }
unsigned long long kex_get_passes_num(){ return 2; }
unsigned long long kex_get_pk_len_bytes(){ return 0; }
unsigned long long kex_get_sk_len_bytes(){ return 0; }
unsigned long long kex_get_sta_len_bytes(){ return sizeof(sta_t); }
unsigned long long kex_get_stb_len_bytes(){ return sizeof(stb_t); }
unsigned long long kex_get_ss_len_bytes(){ return P()->ss_bytes; }
unsigned long long kex_get_total_msg_len_bytes(){ return nike_mb_bytes(P())>nike_ma_bytes(P())?nike_mb_bytes(P()):nike_ma_bytes(P()); }
int kex_init_a(unsigned char *pka,unsigned long long *pka_len_bytes,unsigned char *ska,unsigned long long *ska_len_bytes,unsigned char *sta,unsigned long long *sta_len_bytes){ (void)pka;(void)ska; *pka_len_bytes=0;*ska_len_bytes=0; memset(sta,0,sizeof(sta_t));*sta_len_bytes=sizeof(sta_t); return 0; }
int kex_init_b(unsigned char *pkb,unsigned long long *pkb_len_bytes,unsigned char *skb,unsigned long long *skb_len_bytes,unsigned char *stb,unsigned long long *stb_len_bytes){ (void)pkb;(void)skb;*pkb_len_bytes=0;*skb_len_bytes=0; memset(stb,0,sizeof(stb_t));*stb_len_bytes=sizeof(stb_t); return 0; }
int kex_generate_pass1_msg_a(unsigned char *ska,unsigned long long ska_len_bytes,unsigned char *pkb,unsigned long long pkb_len_bytes,unsigned char *sta,unsigned long long *sta_len_bytes,unsigned char *m1,unsigned long long *m1_len_bytes){ (void)ska;(void)ska_len_bytes;(void)pkb;(void)pkb_len_bytes; sta_t*s=(sta_t*)sta; s->ma_len=nike_ma_bytes(P()); nike_init(&s->st,m1,P()); memcpy(s->ma,m1,s->ma_len); s->ready=1; *m1_len_bytes=s->ma_len; *sta_len_bytes=sizeof(sta_t); return 0; }
int kex_generate_pass2_msg_b(unsigned char *skb,unsigned long long skb_len_bytes,unsigned char *pka,unsigned long long pka_len_bytes,unsigned char *m1,unsigned long long m1_len_bytes,unsigned char *stb,unsigned long long *stb_len_bytes,unsigned char *m2,unsigned long long *m2_len_bytes){ (void)skb;(void)skb_len_bytes;(void)pka;(void)pka_len_bytes; if(m1_len_bytes!=nike_ma_bytes(P())) return -1; stb_t*s=(stb_t*)stb; nike_resp(m2,s->ss,m1,P()); s->mb_len=nike_mb_bytes(P()); memcpy(s->mb,m2,s->mb_len); s->ready=1; *m2_len_bytes=s->mb_len; *stb_len_bytes=sizeof(stb_t); return 1; }
int kex_generate_pass3_msg_a(unsigned char *ska,unsigned long long ska_len_bytes,unsigned char *pkb,unsigned long long pkb_len_bytes,unsigned char *m2,unsigned long long m2_len_bytes,unsigned char *sta,unsigned long long *sta_len_bytes,unsigned char *m3,unsigned long long *m3_len_bytes){ (void)ska;(void)ska_len_bytes;(void)pkb;(void)pkb_len_bytes;(void)m2;(void)m2_len_bytes;(void)sta;(void)sta_len_bytes;(void)m3; *m3_len_bytes=0; return 1; }
int kex_derive_ss_a(unsigned char *ska,unsigned long long ska_len_bytes,unsigned char *pkb,unsigned long long pkb_len_bytes,unsigned char *mb,unsigned long long mb_len_bytes,unsigned char *sta,unsigned long long sta_len_bytes,unsigned char *ssa,unsigned long long *ssa_len_bytes){ (void)ska;(void)ska_len_bytes;(void)pkb;(void)pkb_len_bytes;(void)sta_len_bytes; sta_t*s=(sta_t*)sta; if(!s->ready||mb_len_bytes!=nike_mb_bytes(P())) return -1; nike_derive(ssa,&s->st,s->ma,mb); *ssa_len_bytes=P()->ss_bytes; return 0; }
int kex_derive_ss_b(unsigned char *skb,unsigned long long skb_len_bytes,unsigned char *pka,unsigned long long pka_len_bytes,unsigned char *ma,unsigned long long ma_len_bytes,unsigned char *stb,unsigned long long stb_len_bytes,unsigned char *ssb,unsigned long long *ssb_len_bytes){ (void)skb;(void)skb_len_bytes;(void)pka;(void)pka_len_bytes;(void)stb_len_bytes; stb_t*s=(stb_t*)stb; if(!s->ready||ma_len_bytes!=nike_ma_bytes(P())) return -1; memcpy(ssb,s->ss,P()->ss_bytes); *ssb_len_bytes=P()->ss_bytes; return 0; }
